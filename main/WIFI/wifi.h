//
// Created by 86183 on 2024/9/29.
//

#ifndef WIFI_H
#define WIFI_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "netdb.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

typedef struct _network_connect_info_t
{
    uint8_t connet_state;   // 网络连接状态
    char ip_buf[100];       // 分配的ip地址
    char mac_buf[100];      // 子网掩码
    void (*fun)(uint8_t x); // 函数指针
} network_connect_info;

#endif //WIFI_H
