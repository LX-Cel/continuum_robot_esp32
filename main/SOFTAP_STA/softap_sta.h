//
// Created by 86183 on 2024/9/29.
//

#ifndef SOFTAP_STA_H
#define SOFTAP_STA_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "netdb.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#if IP_NAPT
#include "lwip/lwip_napt.h"
#endif

/* 带CONFIG前缀的表示可以使用menuconfig来进行配置参数 */

/* STA 配置
 * 可以使用menuconfig来配置STA模式下ESP32要连接到的WIFI的名称和密码
 * 若不想使用menuconfig进行参数配置，可将带CONFIG前缀的改为自定义的字符串
 * 如#define ESP_WIFI_STA_SSID "mywifissid"
 */
#define ESP_WIFI_STA_SSID           CONFIG_ESP_WIFI_REMOTE_AP_SSID
#define ESP_WIFI_STA_PASSWD         CONFIG_ESP_WIFI_REMOTE_AP_PASSWORD
#define ESP_MAXIMUM_RETRY           CONFIG_ESP_MAXIMUM_STA_RETRY

#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD   WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD   WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD   WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD   WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD   WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD   WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD   WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD   WIFI_AUTH_WAPI_PSK
#endif

/* AP 配置
 * 可以使用menuconfig来配置AP模式下ESP32产生的热点的名称和密码
 * 若不想使用menuconfig进行参数配置，可将带CONFIG前缀的改为自定义的字符串
 * 如#define ESP_WIFI_AP_SSID "mywifissid"
 */
#define ESP_WIFI_AP_SSID            CONFIG_ESP_WIFI_AP_SSID        /* 配置热点的名称 */
#define ESP_WIFI_AP_PASSWD          CONFIG_ESP_WIFI_AP_PASSWORD    /* 配置热点的密码 */
#define ESP_WIFI_CHANNEL            CONFIG_ESP_WIFI_AP_CHANNEL    /* 配置WIFI接入点的信道 */
#define MAX_STA_CONN                CONFIG_ESP_MAX_STA_CONN_AP    /* 配置可连接到该热点的最大基站数 */

/* 设置事件组，其中BIT0即第0位表示WIFI成功连接事件，BIT1即第1位表示在经过最大重试次数后WIFI连接失败事件 */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

void wifi_start(void);


#endif //SOFTAP_STA_H
