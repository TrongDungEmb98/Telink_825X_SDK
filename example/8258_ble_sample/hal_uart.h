#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#include "app_config.h"
#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "tl_common.h"
#include "vendor/common/user_config.h"
#include <stdint.h>

// (SERIAL_DMA_RX_BUFFER_SIZE + 4) must 16 byte aligned
#define SERIAL_DMA_RX_BUFFER_SIZE 508 // 512

typedef struct {
    uint32_t dma_len;  // dma len must be 4 byte
    char data[SERIAL_DMA_RX_BUFFER_SIZE]; 
} uart_data_t; /* sizeof this struct must be 16 byte aligned */

void hal_uart_init(void);
void at_print(char* str);
void hal_uart_send(char * data, u32 len);

#endif 