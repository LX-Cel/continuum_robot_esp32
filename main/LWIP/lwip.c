//
// Created by 86183 on 2024/9/29.
//

#include "lwip.h"

#define LWIP_RX_BUF_SIZE            200                        /* 最大接收数据长度 */
#define LWIP_PORT                   8080                        /* 连接的本地端口号 */
#define LWIP_SEND_THREAD_PRIO       ( tskIDLE_PRIORITY + 3 )    /* 发送数据线程优先级 */
#define LWIP_SEND_DATA              0x80                        /* 发送数据标志位 */

/* 接收数据缓冲区 */
uint8_t g_lwip_rx_buf[LWIP_RX_BUF_SIZE];
/* 发送数据内容 */
uint8_t g_lwip_send_data[] = "Hello, this is lwip server!";

/* 数据发送标志位 */
uint8_t g_lwip_send_flag = 0;
int g_socket = -1;
int g_lwip_connect_state = 0;
static void lwip_send_dara(void *arg);
