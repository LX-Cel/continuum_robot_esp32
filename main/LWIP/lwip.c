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
int g_socket_conn = 0;
int g_lwip_connect_state = 0;

static void lwip_send_thread(void *arg);

/**
 * @brief       发送数据线程
 * @param       无
 * @retval      无
 */
void lwip_send_data(void) {
    xTaskCreatePinnedToCore(lwip_send_thread, "lwip_send_thread", 4096, NULL, LWIP_SEND_THREAD_PRIO, NULL, 1);
}

void lwip_config(void) {
    struct sockaddr_in server_addr; /* 服务器地址 */
    struct sockaddr_in conn_addr;   /* 连接地址 */
    socklen_t addr_len;             /* 地址长度 */
    int err;
    int length;
    int sock_fd;
    char *tbuf;

    lwip_send_data();                                                           /* 创建一个发送线程 */

    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);     /* 建立一个新的socket连接 */
    memset(&server_addr, 0, sizeof(server_addr));                              /* 将服务器地址清空 */
    server_addr.sin_family = AF_INET;                                          /* 地址家族 */
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);                           /* 注意转化为网络字节序 */
    server_addr.sin_port = htons(LWIP_PORT);                                   /* 使用SERVER_PORT指定为程序头设定的端口号 */

    tbuf = malloc(200);
    sprintf((char *)tbuf, "Port:%d", LWIP_PORT);                               /* 客户端端口号 */

    err = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));  /* 绑定socket */

    if (err < 0) {                                                                     /* 如果绑定失败则关闭套接字 */
        closesocket(sock_fd);                                                          /* 关闭套接字 */
        free(tbuf);
    }

    err = listen(sock_fd, 4);                                                 /* 监听连接请求 */

    if (err < 0) {                                                                     /* 如果监听失败则关闭套接字 */
        closesocket(sock_fd);                                                          /* 关闭套接字 */
    }

    while (1) {
        g_lwip_connect_state = 0;
        addr_len = sizeof(struct sockaddr_in);                                        /* 将连接地址赋值给addr_len */

        g_socket_conn = accept(sock_fd, (struct sockaddr *)&conn_addr, &addr_len);    /* 接受连接请求,状态赋值给socket */

        if (g_socket_conn < 0) {                                                      /* 状态小于0代表连接故障，此时关闭套接字 */
            closesocket(sock_fd);
        } else {
            g_lwip_connect_state = 1;
        }

        while (1) {
            memset(g_lwip_rx_buf, 0, LWIP_RX_BUF_SIZE);                               /* 将接收数据缓冲区清空 */
            length = recv(g_socket_conn, g_lwip_rx_buf, sizeof(g_lwip_rx_buf), 0);         /* 接收数据 */

            if (length <= 0) {
                break;
            }

            printf("%s\r\n",g_lwip_rx_buf);
        }

        if (g_socket_conn >= 0) {
            closesocket(g_socket_conn);
        }

        g_socket_conn = -1;
    }
}

/**
 * @brief       发送数据线程
 * @param       无
 * @retval      无
 */
void lwip_send_thread(void *pvParameters) {
    pvParameters = pvParameters;                                                                         /* 防止FreeRTOS编译报错 */

    while (1) {
        if (((g_lwip_send_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) && (g_lwip_connect_state == 1)) {   /* 如果有数据要发送 */
            send(g_socket_conn, g_lwip_send_data, sizeof(g_lwip_send_data), 0);               /* 发送数据 */
            g_lwip_send_flag &= ~LWIP_SEND_DATA;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
