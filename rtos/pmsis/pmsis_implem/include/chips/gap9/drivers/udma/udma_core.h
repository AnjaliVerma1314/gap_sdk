/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once


/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if !defined(__FREERTOS__)
#define UDMA_CHAN_LIN(id)              ( UDMA_LIN_ADDRGEN_ADDR((id)) )
#define UDMA_CHAN_2D(id)
#define UDMA_CHAN_FIFO(id)
#endif  /* __FREERTOS__ */


#define __PI_NB_UDMA_CHAN_PER_REG      ( 32 )
#define __PI_NB_UDMA_CHAN_PER_REG_LOG2 ( 5 )
#define PI_NB_UDMA_CHAN_LIN            ( UDMA_NB_CHAN_LIN )
#define PI_NB_UDMA_CHAN_LIN_REGS       ( (PI_NB_UDMA_CHAN_LIN + (__PI_NB_UDMA_CHAN_PER_REG - 1)) >> __PI_NB_UDMA_CHAN_PER_REG_LOG2 )

/*******************************************************************************
 * Driver data
 ******************************************************************************/

extern PI_FC_TINY uint32_t __pi_udma_chan_lin[];
extern PI_FC_TINY uint32_t __pi_udma_chan_2d;
extern PI_FC_TINY uint32_t __pi_udma_chan_fifo;

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

static inline void pi_udma_core_channels_init(void)
{
    uint32_t mask = 0;
    /* UDMA_CHAN_LIN */
    uint32_t nb_udma_lin = (uint32_t) UDMA_NB_CHAN_LIN;
    for (uint32_t reg=0; reg<(uint32_t) PI_NB_UDMA_CHAN_LIN_REGS; reg++)
    {
        uint32_t shift = nb_udma_lin > 32 ? 32 : nb_udma_lin;
        mask = (1ULL << shift) - 1;
        __pi_udma_chan_lin[reg] = mask;
        nb_udma_lin -= 32;
    }
    __pi_udma_chan_lin[0] = __BITCLR_R(__pi_udma_chan_lin[0], 1, 0); /* Reserved. */
    /* UDMA_CHAN_2D */
    uint32_t nb_udma_2d = (uint32_t) UDMA_NB_CHAN_2D;
    mask = (1 << nb_udma_2d) - 1;
    __pi_udma_chan_2d = mask;
    __pi_udma_chan_2d = __BITCLR_R(__pi_udma_chan_2d, 1, 0); /* Reserved. */

    /* UDMA_CHAN_FIFO */
    uint32_t nb_udma_fifo = (uint32_t) UDMA_NB_CHAN_FIFO;
    mask = (1 << nb_udma_fifo) - 1;
    __pi_udma_chan_fifo = mask;
}

/**
 * UDMA_CHANNEL_LINEAR
 */
static inline uint32_t pi_udma_core_lin_addr_get(int32_t chan_id)
{
    return UDMA_CHAN_LIN(chan_id);
}

static inline int32_t pi_udma_core_lin_alloc(void)
{
    int32_t chan_id = -1;
    uint32_t reg_status = 0;
    for (uint32_t reg=0; reg<(uint32_t) PI_NB_UDMA_CHAN_LIN_REGS; reg++)
    {
        reg_status = __pi_udma_chan_lin[reg];
        if (0x0 != reg_status)
        {
            chan_id = __FF1(reg_status);
            __pi_udma_chan_lin[reg] = __BITCLR_R(reg_status, 1, chan_id);
            return (chan_id + (reg << __PI_NB_UDMA_CHAN_PER_REG_LOG2));
        }
    }
    return chan_id;
}

static inline void pi_udma_core_lin_free(int32_t chan_id)
{
    if (-1 != chan_id)
    {
        uint32_t chan_reg = (chan_id >> __PI_NB_UDMA_CHAN_PER_REG_LOG2);
        uint32_t chan_pos = (chan_id & 0x1F);
        __pi_udma_chan_lin[chan_reg] = __BITSET_R(__pi_udma_chan_lin[chan_reg], 1, chan_pos);
    }
}


/**
 * UDMA_CHANNEL_2D
 */
static inline uint32_t pi_udma_core_2d_addr_get(int32_t chan_id)
{
    return UDMA_CHAN_2D(chan_id);
}

static inline int32_t pi_udma_core_2d_alloc(void)
{
    int32_t chan_id = -1;
    uint32_t reg_status = __pi_udma_chan_2d;
    if (0x0 != reg_status)
    {
        chan_id = __FF1(reg_status);
        return (chan_id + UDMA_CHAN_2D_ID(0));
    }
    return chan_id;
}

static inline void pi_udma_core_2d_free(int32_t chan_id)
{
    if (-1 != chan_id)
    {
        __pi_udma_chan_2d = __BITSET_R(__pi_udma_chan_2d, 1, chan_id - UDMA_CHAN_2D_ID(0));
    }
}


/**
 * UDMA_CHANNEL_FIFO
 */
static inline uint32_t pi_udma_core_fifo_addr_get(int32_t chan_id)
{
    return UDMA_CHAN_FIFO(chan_id);
}

static inline int32_t pi_udma_core_fifo_alloc(void)
{
    int32_t chan_id = -1;
    uint32_t reg_status = __pi_udma_chan_fifo;
    if (0x0 != reg_status)
    {
        chan_id = __FF1(reg_status);
        return (chan_id + UDMA_CHAN_FIFO_ID(0));
    }
    return chan_id;
}

static inline void pi_udma_core_fifo_free(int32_t chan_id)
{
    if (-1 != chan_id)
    {
        __pi_udma_chan_fifo = __BITSET_R(__pi_udma_chan_fifo, 1, chan_id - UDMA_CHAN_FIFO_ID(0));
    }
}
