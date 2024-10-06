//
// Created by li'xiao on 2024/10/5.
//

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "esp_log.h"
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"


#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>


#define PORT                        CONFIG_PORT                 /* 配置端口号 */
#define KEEPALIVE_IDLE              CONFIG_KEEPALIVE_IDLE       /* 配置TCP连接的空闲时间 */
#define KEEPALIVE_INTERVAL          CONFIG_KEEPALIVE_INTERVAL   /* 配置TCP连接的检测时间 */
#define KEEPALIVE_COUNT             CONFIG_KEEPALIVE_COUNT      /* 配置TCP连接的检测次数 */

void tcp_task_start();
esp_err_t send_message(char* buf, int _size);

#endif //TCP_SERVER_H
