#ifndef _UART_H_
#define _UART_H_

#include "hal_uart.h"

void app_uart_loop(void);
void process_data_loop(void);
void app_uart_irq_proc(void);

#endif