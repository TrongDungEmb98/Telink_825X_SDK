/********************************************************************************************************
 * @file     app_uart.c
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     May. 12, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and
 *proprietary property of Telink Semiconductor (Shanghai) Co., Ltd. and is
 *available under the terms of Commercial License Agreement between Telink
 *Semiconductor (Shanghai) Co., Ltd. and the licensee in separate contract or
 *the terms described here-in. This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the
 *information in this file under Mutual Non-Disclosure Agreement. NO WARRENTY of
 *ANY KIND is provided.
 *
 ******************************************************************************/
#include "hal_uart.h"
#include "drivers.h"
#include "tl_common.h"

uart_data_t rec_buff = {0, {0}};
uart_data_t trans_buff = {0, {0}};

void hal_uart_init(void)
{
    // note: dma addr must be set first before any other uart initialization!
    // (confirmed by sihui)
    uart_recbuff_init((unsigned short *)&rec_buff, sizeof(rec_buff));
    // uart tx/rx pin set uart setting must set after this reset
    uart_gpio_set(UART_TX_PB1, UART_RX_PA0);

// baud rate: 115200
#if (CLOCK_SYS_CLOCK_HZ == 16000000)
    uart_init(9, 13, PARITY_NONE, STOP_BIT_ONE); /* this run */
#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
    uart_init(12, 15, PARITY_NONE, STOP_BIT_ONE);
#endif

    // uart data in hardware buffer moved by dma, so we need enable them first
    uart_dma_enable(1, 1);
    irq_set_mask(FLD_IRQ_DMA_EN);

    // uart Rx/Tx dma irq enable
    dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 1);
    uart_irq_enable(0, 0);  // uart Rx/Tx irq no need, disable them
    irq_enable();
}

#define USE_HW_UART 1

void at_print(char *str)
{
#if USE_HW_UART
    while (*str) {
        trans_buff.data[trans_buff.dma_len] = *str++;
        trans_buff.dma_len += 1;
        if (trans_buff.dma_len == 12) {
            uart_dma_send((unsigned char *)&trans_buff);
            trans_buff.dma_len = 0;
            WaitMs(2);
        }
    }

    if (trans_buff.dma_len) {
        uart_dma_send((unsigned char *)&trans_buff);
        trans_buff.dma_len = 0;
        WaitMs(2);
    }
#else
	while (*str) {
        putchar(*str);
		str++;
    }
#endif
}

void puts(char *s) { at_print(s); }

void hal_uart_send(char *data, u32 len)
{
    while (len > SERIAL_DMA_RX_BUFFER_SIZE) {
        memcpy(trans_buff.data, data, SERIAL_DMA_RX_BUFFER_SIZE);
        data += SERIAL_DMA_RX_BUFFER_SIZE;
        len -= SERIAL_DMA_RX_BUFFER_SIZE;

        trans_buff.dma_len = SERIAL_DMA_RX_BUFFER_SIZE;

        uart_dma_send((unsigned char *)&trans_buff);
        trans_buff.dma_len = 0;
        WaitMs(2);
    }

    if (len > 0) {
        memcpy(trans_buff.data, data, len);
        trans_buff.dma_len = len;
        uart_dma_send((unsigned char *)&trans_buff);
        trans_buff.dma_len = 0;
        WaitMs(2);
    }
}
