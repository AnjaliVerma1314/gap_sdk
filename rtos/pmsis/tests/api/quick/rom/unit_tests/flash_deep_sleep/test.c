#include <stdio.h>
#include <pmsis.h>
#include "testbench.h"
#include <bsp/bsp.h>



#ifdef USE_HYPERFLASH
#include <bsp/flash/hyperflash.h>
#else
#include <bsp/flash/spiflash.h>
#endif


#if defined(__PLATFORM_RTL__)
#define NB_EVENTS 20
#else
#define NB_EVENTS 5
#endif


static inline void get_rtc_info(int *div, uint32_t *count)
{
#if defined(__PLATFORM_RTL__)
#if !defined(RTC_VERSION) || RTC_VERSION == 1
    *div = 0x20;
    *count = 0x1;
#else
    *div = 0;
    *count = 1*32768;
#endif
#else
#if !defined(RTC_VERSION) || RTC_VERSION == 1
    *div = 0x8000;
    *count = 1;
#else
    *div = 0;
    *count = 1*32768;
#endif
#endif
}


void testbench_prepare_pads()
{
    pi_testbench_prepare_pads(NULL);
}


void testbench_exit(int status)
{
  pi_testbench_set_status(pi_testbench_get(), status);
  while(1);
}



static PI_L2 char counts[NB_EVENTS];

static int flash_read(struct pi_device *flash)
{
  pi_flash_read(flash, 0x00080000, (void *)counts, sizeof(counts));
  for (int i=0; i<NB_EVENTS; i++)
  {
    if (counts[i] != 0)
      return i;
  }
}

static void flash_write(struct pi_device *flash, int count)
{
  counts[count-1] = 0;
  pi_flash_program(flash, 0x00080000, (void *)counts, sizeof(counts));
}


static int open_flash(struct pi_device *flash)
{
#ifdef USE_HYPERFLASH
  struct pi_hyperflash_conf flash_conf;
  pi_hyperflash_conf_init(&flash_conf);
#elif USE_MRAM
  struct pi_mram_conf flash_conf;
  pi_mram_conf_init(&flash_conf);
#else
#if defined(CONFIG_ATXP032)
  struct pi_atxp032_conf flash_conf;
  pi_atxp032_conf_init(&flash_conf);
#else
  struct pi_spiflash_conf flash_conf;
  pi_spiflash_conf_init(&flash_conf);
#endif
#endif

  pi_open_from_conf(flash, &flash_conf);

  if (pi_flash_open(flash))
    return -1;

  return 0;
} 


int main()
{
  struct pi_device flash;

  // Setup the Pads now to avoid triggering some random communication with the testbench
  testbench_prepare_pads();

  // And then release the outputs that we forced in case we come back from deep sleep
  pi_pad_sleep_cfg_force(0);

  if (pi_pmu_get_prev_state(PI_PMU_DOMAIN_CHIP) == PI_PMU_DOMAIN_STATE_RESET)
  {
    printf("STA\n");


    // Open flash and put it to deep sleep to check that the ROM can exit deep sleep
    if (open_flash(&flash))
      return -1;

    //pi_atxp032_deep_sleep_enter(&flash);
    pi_hyperflash_deep_sleep_enter(&flash);

    struct pi_rtc_conf conf;
    uint32_t countdown;
    struct pi_device rtc;
    pi_rtc_conf_init(&conf);

    get_rtc_info(&conf.clk_div, &countdown);

    pi_open_from_conf(&rtc, &conf);

    if (pi_rtc_open(&rtc))
        return -1;

    pi_rtc_timer_set(&rtc, countdown);
    pi_rtc_ioctl(&rtc, PI_RTC_TIMER_START, (void *)1);

    // Force outputs during deep sleep to avoid random communication
    pi_pad_sleep_cfg_force(1);

    pi_pmu_wakeup_control(PI_PMU_WAKEUP_RTC, 0);

    pi_pmu_set_state(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP, 0);
  }
  else
  {
    printf("TOK\n");
    testbench_exit(0);
    return 0;
  }

  printf("TKO\n");
  testbench_exit(-1);
  return -1;
}
