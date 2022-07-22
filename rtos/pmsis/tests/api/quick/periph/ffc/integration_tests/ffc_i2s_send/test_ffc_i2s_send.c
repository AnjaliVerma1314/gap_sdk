/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and
 * GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pmsis.h"
#include "testbench.h"

#if !defined(FFC_ID)
#define FFC_ID (0)
#endif

#define BUFFER_SIZE (64)

static uint32_t test_buffer[BUFFER_SIZE];
static float verif_buffer[BUFFER_SIZE];

static struct pi_device i2s;

static void initialize_verif_data(float start, float incr)
{
    float value = start;
    for (size_t i = 0; i < BUFFER_SIZE; i++)
    {
        verif_buffer[i] = value;
        value += incr;
    }
}

static int open_testbench()
{
    printf("Opening testbench\n");
    // Plug I2S testbench verif to I2S interface
    pi_testbench_i2s_verif_config_t i2s_config = {
        .word_size=32,
        .nb_slots=1,
        .is_full_duplex=1,
        .ws_delay=1
    };
    if (pi_testbench_i2s_verif_open(pi_testbench_get(), 0, &i2s_config))
    {
        return -1;
    }

    const float start = 0;
    const float incr = 16;
    initialize_verif_data(start, incr);

    // Configure TX slot
    pi_testbench_i2s_verif_slot_config_t config_tx = {
        .is_rx=0,
        .word_size=32,
        .format=1
    };
    if (pi_testbench_i2s_verif_slot_open(pi_testbench_get(), 0, 0, &config_tx))
    {
        return -1;
    }

    // Start file dumper on TX slot
    pi_testbench_i2s_verif_slot_start_config_t start_config_tx = {
        .type=PI_TESTBENCH_I2S_VERIF_TX_FILE_DUMPER,
        .tx_file_dumper= {
            .nb_samples=-1,
            .filepath=(uint32_t)"outfile.txt"
        }
    };
    if (pi_testbench_i2s_verif_slot_start(pi_testbench_get(), 0, 0, &start_config_tx))
    {
        return -1;
    }

    return 0;
}


static int open_i2s(struct pi_device *i2s)
{
    printf("Opening i2s interface\n");
    // First open I2S interface
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    i2s_conf.frame_clk_freq = 44100;
    i2s_conf.itf = 0;
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_I2S;
    i2s_conf.word_size = 32;
    i2s_conf.channels = 1;
    i2s_conf.options = PI_I2S_OPT_FULL_DUPLEX | PI_I2S_OPT_TDM;

    pi_open_from_conf(i2s, &i2s_conf);

    if (pi_i2s_open(i2s))
    {
        printf("Failed to open I2S\n");
        return -1;
    }
    printf("I2S opened successfully\n");

    printf("Opening FFC %d\n", FFC_ID);

    pi_ffc_conf_t ffc_conf;
    pi_device_t ffc_device;

    pi_ffc_conf_init(&ffc_conf);

    ffc_conf.itf = FFC_ID;
    ffc_conf.mode = PI_FFC_FLOAT_TO_FIXED;
    ffc_conf.io_mode = PI_FFC_MEMORY_IN_STREAM_OUT;
    ffc_conf.float_type = PI_FFC_FLOAT_FP32;
    ffc_conf.fixed_type = PI_FFC_FIXED_32;
    ffc_conf.fixed_precision = 0;
    ffc_conf.fixed_scale = 0;

    pi_open_from_conf(&ffc_device, &ffc_conf);

    /* open */
    if(PI_OK != pi_ffc_open(&ffc_device))
    {
        printf("Failed to open FFC\n");
        return -1;
    }

    // Then configure slot 0 for TX
    struct pi_i2s_channel_conf i2s_channel_conf;
    pi_i2s_channel_conf_init(&i2s_channel_conf);
    i2s_channel_conf.options = PI_I2S_OPT_IS_TX | PI_I2S_OPT_ENABLED;
    i2s_channel_conf.asrc_channel = 18 + FFC_ID; // Driver will add 0xe0, so put 16 to get 0xf2
    i2s_channel_conf.word_size = 32;
    i2s_channel_conf.format = PI_I2S_FMT_DATA_FORMAT_I2S | PI_I2S_CH_FMT_DATA_ORDER_MSB;

    printf("Setting I2S configuration\n");
    if (pi_i2s_channel_conf_set(i2s, 0, &i2s_channel_conf))
    {
        printf("Failed to set i2s configuration via ioctl\n");
        return -1;
    }
    printf("I2S configuration set\n");

#if 0 // only useful when debugging
    printf("FFC converting verif data\n");
    pi_ffc_ioctl(&ffc_device, PI_FFC_IOCTL_SET_IO_MODE, (void*) PI_FFC_MEMORY_IN_MEMORY_OUT);
    pi_ffc_convert(&ffc_device, verif_buffer, test_buffer, BUFFER_SIZE);
    pi_ffc_ioctl(&ffc_device, PI_FFC_IOCTL_SET_IO_MODE, (void*) PI_FFC_MEMORY_IN_STREAM_OUT);
    printf("Done converting verif data\n");

    printf("Expected Data\n");
    for (size_t i = 0; i < 10; i++)
    {
        printf("[%lu]: %x\n", i, test_buffer[i]);
    }
#endif

    /* set ffc as continuous mode */
    pi_ffc_ioctl(&ffc_device, PI_FFC_IOCTL_CONTINUOUS_ENABLE, (void*) 1);

    printf("FFC convert async\n");
    pi_task_t block;
    pi_task_t block1;
    pi_task_block(&block);
    pi_task_block(&block1);
    pi_ffc_convert_async(&ffc_device, verif_buffer, test_buffer, BUFFER_SIZE, &block);
    // Start sampling, could be delayed until FFC is ready to not drop first samples
    if (pi_i2s_ioctl(i2s, PI_I2S_IOCTL_START, NULL))
    {
        printf("Failed to start I2S\n");
        return -1;
    }

    printf("I2S Started\n");


    printf("Wait for FFC convert end\n");

    for (int i = 0; i < 5; i++)
    {
        pi_task_block(&block1);
        pi_ffc_convert_async(&ffc_device, verif_buffer, test_buffer, BUFFER_SIZE, &block1);

        pi_task_wait_on(&block);

        pi_task_block(&block);
        pi_ffc_convert_async(&ffc_device, verif_buffer, test_buffer, BUFFER_SIZE, &block);

        pi_task_wait_on(&block1);
    }

    pi_task_wait_on(&block);

    printf("FFC convert done\n");

    /* wait for data inside ffc to be flushed in I2S */
    pi_time_wait_us(2000);

    /* stop continuous mode */
    pi_ffc_ioctl(&ffc_device, PI_FFC_IOCTL_CONTINUOUS_ENABLE, (void*) 0);

    if (pi_i2s_ioctl(i2s, PI_I2S_IOCTL_STOP, NULL))
    {
        printf("Failed to stop I2S\n");
        return -1;
    }

    return 0;
}


static int test_entry()
{
    printf("Entering main controller\n");

    if (open_testbench())
    {
        printf("Failed to open testbench\n");
        return -1;
    }

    if (open_i2s(&i2s))
    {
        printf("Failed to open I2S\n");
        return -1;
    }

    return 0;
}


void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    printf("========= PMSIS I2S/FFC TX TESTS =========\n");
    return pmsis_kickoff((void *)test_kickoff);
}
