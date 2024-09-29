//
// Created by 86183 on 2024/9/29.
//

#include "wifi.h"

/* 定义wifi名称 */
#define WIFI_SSID "continuum_robot"
/* 定义wifi密码 */
#define WIFI_PASS "12345678"

/* 事件标志 */
static EventGroupHandle_t wifi_event;
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

network_connect_info network_connect;
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
 * @param       arg:传入网卡控制模块
 * @param       event_base:WiFi事件
 * @param       event_id:事件ID
 * @param       event_data:事件数据
 * @retval      无
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    static int s_retry_num = 0;

    /* 扫描到要连接的WiFi事件 */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        network_connect.connect_state |= 0x04;
        network_connect.fun(network_connect.connect_state);
        esp_wifi_connect();
    }
    /* 连接WiFi事件 */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        network_connect.connect_state |= 0x80;
        network_connect.fun(network_connect.connect_state);
    }
    /* 连接WiFi失败事件 */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        network_connect.connect_state &= 0x02;
        /* 尝试连接 */
        if (s_retry_num < 20) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(wifi_event, WIFI_FAIL_BIT);
            network_connect.fun(network_connect.connect_state);
        }
    }
    /* 工作站从连接的AP获得IP */
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        network_connect.connect_state |= 0x01;
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        sprintf(network_connect.ip_buf, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        network_connect.fun(network_connect.connect_state);
        xEventGroupSetBits(wifi_event, WIFI_CONNECTED_BIT);
    }
}

/**
 * @brief       WiFi初始化
 * @param       无
 * @retval      无
 */
void wifi_sta_init(void) {
    static esp_netif_t *sta_netif = NULL;
    network_connect.connect_state = 0x00;

    wifi_event = xEventGroupCreate(); /* 创建事件标志组 */
    /* 网卡初始化 */
    ESP_ERROR_CHECK(esp_netif_init());
    /* 创建新的事件循环 */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = WIFICONFIG();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* 等待连接成功后，ip生成 */
    EventBits_t bits = xEventGroupWaitBits(wifi_event, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE,
                                           portMAX_DELAY);

    /* 判断连接事件 */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", WIFI_SSID, WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", WIFI_SSID, WIFI_PASS);
    } else {
        ESP_LOGI(TAG, "UNEXPECTED EVENT");
    }

    vEventGroupDelete(wifi_event);
}
