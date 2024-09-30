//
// Created by 86183 on 2024/9/29.
//

#include "wifi.h"

static const char *TAG = "AP";

/* 定义wifi名称 */
#define WIFI_SSID "continuum_robot"
/* 定义wifi密码 */
#define WIFI_PASS "12345678"
/* 定义wifi最大连接数 */
#define EXAMPLE_MAX_STA_CONN 5
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
static char lcd_buff[100] = {0};


/* 事件标志 */
static EventGroupHandle_t wifi_event;
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

network_connect_info network_connect;



/**
 * @brief       WiFi连接回调函数
 * @param       arg:传入网卡控制模块
 * @param       event_base:WiFi事件
 * @param       event_id:事件ID
 * @param       event_data:事件数据
 * @retval      无
 */
void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    static int s_retry_num = 0;

    /* 设备连接 */
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
        sprintf(lcd_buff, "MACSTR:" MACSTR, MAC2STR(event->mac));
    }
    /* 设备断开 */
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
        sprintf(lcd_buff, "Device disconnected:" MACSTR, MAC2STR(event->mac));
    }
}

/**
 * @brief       WiFi初始化
 * @param       无
 * @retval      无
 */
void wifi_ap_init(void) {
    /* 初始化网卡 */
    ESP_ERROR_CHECK(esp_netif_init());

    /* 创建新的事件循环 */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* 使用默认配置初始化包括netif的WiFi */
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    /* 配置WiFi */
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .password = WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    if (strlen(WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

    char ip_addr[16];
    inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s ip:%s", WIFI_SSID, WIFI_PASS, ip_addr);
}
