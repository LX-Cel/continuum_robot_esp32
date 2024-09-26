//
// Created by li'xiao on 2024/9/25.
//

#include "usart.h"

/**
 * @brief       串口初始化
 * @param       baudrate        波特率
 * @retval      无
 */
void usart_init(uint32_t baudrate) {
    uart_config_t uart1;                                //串口配置句柄

    uart1.baud_rate = 115200;                           //波特率
    uart1.data_bits = UART_DATA_8_BITS;                 //8位数据格式
    uart1.parity = UART_PARITY_DISABLE;                 //无奇偶校验
    uart1.stop_bits = UART_STOP_BITS_1;                 //一个停止位
    uart1.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;         //无硬件流控
    uart1.source_clk = UART_SCLK_APB;                   //时钟源
    uart1.rx_flow_ctrl_thresh = 122;                    //硬件流控阈值
    ESP_ERROR_CHECK(uart_param_config(USART_UX, &uart1));

    /* 配置uart引脚 */
    uart_set_pin(USART_UX, USART_TX_GPIO_PIN, USART_RX_GPIO_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    /* 安装串口驱动 */
    uart_driver_install(USART_UX, RX_BUF_SIZE * 2, RX_BUF_SIZE * 2, 20, NULL, 0);
}
