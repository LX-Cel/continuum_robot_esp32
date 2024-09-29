//
// Created by 86183 on 2024/9/29.
//

#ifndef LWIP_H
#define LWIP_H

#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#define LWIP_SEND_DATA              0X80    /* 定义有数据发送 */
extern uint8_t g_lwip_send_flag;            /* 数据发送标志位 */

void lwip_config(void);


#endif //LWIP_H
