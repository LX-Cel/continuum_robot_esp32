//
// Created by li'xiao on 2024/9/25.
//

#ifndef USART_H
#define USART_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/uart_select.h"
#include "driver/gpio.h"

#define USART_UX            UART_NUM_0
#define USART_TX_GPIO_PIN   GPIO_NUM_43
#define USART_RX_GPIO_PIN   GPIO_NUM_44

#define RX_BUF_SIZE         1024

void usart_init(uint32_t baudrate);

#endif //USART_H
