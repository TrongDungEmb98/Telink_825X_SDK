#include "app_uart.h"

// #include <stdlib.h>
#include <string.h>
#include "hal_uart.h"
#include "tl_common.h"

extern uart_data_t rec_buff;

void app_uart_init(void)
{
    hal_uart_init();
}

/* uart isr */
void app_uart_irq_proc(void)
{
    unsigned char uart_dma_irqsrc;
    
    /* interrupt flag have already been cleared */
    uart_dma_irqsrc = dma_chn_irq_status_get();

    if (uart_dma_irqsrc & FLD_DMA_CHN_UART_RX) {
        // Received uart data in rec_buff, user can copy data here
        printf("%s\n", rec_buff.data);
        dma_chn_irq_status_clr(FLD_DMA_CHN_UART_RX);
    }
    if (uart_dma_irqsrc & FLD_DMA_CHN_UART_TX) {
        dma_chn_irq_status_clr(FLD_DMA_CHN_UART_TX);
    }
}
