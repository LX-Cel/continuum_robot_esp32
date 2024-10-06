//
// Created by li'xiao on 2024/10/5.
//

#include "tcp_server.h"

static const char *TAG = "TCP_SERVER";
static int sock = 0;
static int listen_sock;

static void tcp_retransmit(const int sock)
{
    int len;
    char rx_buffer[128];

    do {
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0) {
            ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);      /* 记录错误信息 */
        } else if (len == 0) {                                                      /* 记录警告信息 */
            ESP_LOGW(TAG, "Connection closed");
        } else {
            rx_buffer[len] = 0;                                                     /* 将接收到的内容以空字符终止，并将其视为字符串 */
            ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);                 /* 记录接收到的数据 */

            /*send 函数在发送数据时，可能由于网络拥塞或其他原因，无法一次性发送所有提供的数据。
             *为了确保所有数据都被发送，需要在一个循环中反复调用 send，直到所有数据都发送完毕
             */
            int to_write = len;
            while (to_write > 0) {
                int written = send(sock, rx_buffer + (len - to_write), to_write, 0);
                if (written < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);    /* 记录错误信息 */
                    // Failed to retransmit, giving up
                    return;
                }
                to_write -= written;
            }
        }
    } while (len > 0);
}

// int tcp_retransmit(int sock) {
//     int len;
//     char rx_buffer[128];
//     int temp_sock = sock;
//
//     len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
//
//
//     if (len < 0) {
//         ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno); /* 记录错误信息 */
//         temp_sock = 0;
//         return temp_sock;
//     } else if (len == 0) {
//         ESP_LOGW(TAG, "Connection closed"); /* 记录警告信息 */
//         temp_sock = 0;
//         return temp_sock;
//     } else {
//         rx_buffer[len] = 0; /* 将接收到的内容以空字符终止，并将其视为字符串 */
//         ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer); /* 记录接收到的数据 */
//         int to_write = len;
//         while (to_write > 0) {
//             int written = send(sock, rx_buffer + (len - to_write), to_write, 0);
//             if (written < 0) {
//                 ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno); /* 记录错误信息 */
//                 temp_sock = 0;
//                 return temp_sock;
//             }
//             to_write -= written;
//         }
//     }
//     return temp_sock;
// }

static void tcp_server_task(void *pvParameters) {

    char* str = "Hello world!\r\n";

    char addr_str[128];
    int addr_family = (int) pvParameters;
    int ip_protocol = 0;
    int keepAlive = 1;
    int keepIdle = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount = KEEPALIVE_COUNT;
    struct sockaddr_storage dest_addr;

#ifdef CONFIG_IPV4
    if (addr_family == AF_INET) {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;
    }
#endif
#ifdef CONFIG_IPV6
    if (addr_family == AF_INET6) {
        struct sockaddr_in6 *dest_addr_ip6 = (struct sockaddr_in6 *)&dest_addr;
        bzero(&dest_addr_ip6->sin6_addr.un, sizeof(dest_addr_ip6->sin6_addr.un));
        dest_addr_ip6->sin6_family = AF_INET6;
        dest_addr_ip6->sin6_port = htons(PORT);
        ip_protocol = IPPROTO_IPV6;
    }
#endif

    listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#if defined(CONFIG_IPV4) && defined(CONFIG_IPV6)
    /* 注意默认情况下IPV6绑定两种协议时，需要被禁用 */
    setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
#endif

    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *) &dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1) {
        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *) &source_addr, &addr_len);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
        // Convert ip address to string
#ifdef CONFIG_IPV4
        if (source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }
#endif
#ifdef CONFIG_IPV6
        if (source_addr.ss_family == PF_INET6) {
            inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
        }
#endif
        ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

        tcp_retransmit(sock);
        // while (1) {
        //     int ret = send(sock, str, strlen(str), 0);
        //     if (ret < 0) {
        //         ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        //         break;
        //     }
        //     vTaskDelay(pdMS_TO_TICKS(2000));
        // }
        // send(sock, str, strlen(str), 0);
        shutdown(sock, 0);
        close(sock);
        // vTaskDelay(pdMS_TO_TICKS(2000));

    }



CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}

void tcp_task_start() {

    ESP_LOGI(TAG, "tcp_task_creating");
#ifdef CONFIG_IPV4
    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)AF_INET, 5, NULL);
#endif
#ifdef CONFIG_IPV6
    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)AF_INET6, 5, NULL);
#endif

}

esp_err_t send_message(char* buf, int _size) {

    ESP_LOGE(TAG, "Tring");
    if (sock <= 0) {
        ESP_LOGE(TAG, "Socket error");
        return ESP_FAIL;
    }
    int ret = send(sock, buf, _size, 0);
    if (ret < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        return ESP_FAIL;
    }
    return ESP_OK;
}


