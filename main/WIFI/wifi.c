//
// Created by 86183 on 2024/9/29.
//

#include "wifi.h"

/* 定义wifi名称 */
#define WIFI_SSID "continuum_robot"
/* 定义wifi密码 */
#define WIFI_PASS "12345678"

/* 事件标志 */
static EventGroupHandle_t   wifi_event;
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

network_connect_info network_connet;
static const char *TAG = "static_ip";

/* WiFi默认配置 */
#define WIFICONFIG() {                            \
    .sta = {                                      \
        .ssid = WIFI_SSID,                        \
        .password = WIFI_PASS,                    \
        .threshold.authmode = WIFI_AUTH_WPA2_PSK, \
    }, \
};


/**
 * @brief       WiFi连接回调函数
 * @param       arg：传入网卡控制模块
 * @param       event_base：WiFi事件
 * @param       event_id：事件ID
 * @param       event_data：事件数据
 * @retval      无
 */
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    static int s_retry_num = 0;

    /* 扫描到要连接的WiFi事件 */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        network_connet.connet_state |= 0x04;
        network_connet.fun(network_connet.connet_state);
        esp_wifi_connect();
    }
    /* 连接WiFi事件 */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        network_connet.connet_state |= 0x80;
        network_connet.fun(network_connet.connet_state);
    }
    /* 连接WiFi失败事件 */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        network_connet.connet_state &= 0x02;
        /* 尝试连接 */
        if (s_retry_num < 20) {
            esp_wifi_connect();
            s_retry_num ++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(wifi_event, WIFI_FAIL_BIT);
            network_connet.fun(network_connet.connet_state);
        }
    }
}

