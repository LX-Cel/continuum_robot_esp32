//
// Created by 86183 on 2024/9/29.
//

#include "softap_sta.h"

static const char *TAG_AP = "WiFi SoftAP";
static const char *TAG_STA = "WiFi Sta";

static int s_retry_num = 0;     /* 表示失败重连次数 */

/* FreeRTOS事件组，用于我们在连接时或断开连接时发出信号 */
static EventGroupHandle_t s_wifi_event_group;


/* 事件标志 */
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1


/**
 * @brief       WiFi连接回调函数
 * @param       arg:传入网卡控制模块
 * @param       event_base:WiFi事件
 * @param       event_id:事件ID
 * @param       event_data:事件数据
 * @retval      无
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    /* AP模式：基站成功连接，当基站成功连接到esp32时，记录连接的基站的MAC地址和AID */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
        ESP_LOGI(TAG_AP, "Station "MACSTR" left, AID=%d", MAC2STR(event->mac), event->aid);
    }
    /* AP模式：基站成功断开，当基站断开连接时，记录断开连接的基站的MAC地址和AID */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
        ESP_LOGI(TAG_AP, "Station "MACSTR" left, AID=%d", MAC2STR(event->mac), event->aid);
    }
    /* STA模式：esp32开始连接到WIFI，当STA模式启动时，尝试连接到预先设定的WIFI */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        ESP_LOGI(TAG_STA, "Station started");
    }
    /* STA模式：esp32获取IP地址，当成功获取IP地址时，记录获取到的IP地址，并设置事件组位为1，表示连接成功，同时将失败重连次数清零 */
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG_STA, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/**
 * @brief       初始化AP模式
 * @param       无
 * @retval      无
 */
esp_netif_t *wifi_init_softap(void) {
    /* 创建默认的WIFI AP接口 */
    esp_netif_t *esp_netif_ap = esp_netif_create_default_wifi_ap();

    /* 配置AP参数 */
    wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid = ESP_WIFI_AP_SSID,
            .ssid_len = strlen(ESP_WIFI_AP_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_AP_PASSWD,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = false,
            },
        },
    };

    /* 如果密码为空，则将认证模式设置为开放 */
    if (strlen(ESP_WIFI_AP_PASSWD) == 0) {
        wifi_ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    /* 应用配置到WiFi接口 */
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));

    ESP_LOGI(TAG_AP, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             ESP_WIFI_AP_SSID, ESP_WIFI_AP_PASSWD, ESP_WIFI_CHANNEL);

    return esp_netif_ap;
}

/**
 * @brief       初始化STA模式
 * @param       无
 * @retval      无
 */
esp_netif_t *wifi_init_sta(void)
{
    esp_netif_t *esp_netif_sta = esp_netif_create_default_wifi_sta();

    wifi_config_t wifi_sta_config = {
        .sta = {
            .ssid = ESP_WIFI_STA_SSID,
            .password = ESP_WIFI_STA_PASSWD,
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .failure_retry_cnt = ESP_MAXIMUM_RETRY,
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config) );

    ESP_LOGI(TAG_STA, "wifi_init_sta finished.");

    return esp_netif_sta;
}

/**
 * @brief       启动WiFi
 * @param       无
 * @retval      无
 */
void wifi_start(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());



    /* 创建一个事件标志组 */
    s_wifi_event_group = xEventGroupCreate();

    /* 注册事件句柄 */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    ESP_EVENT_ANY_ID,
                    &wifi_event_handler,
                    NULL,
                    NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                    IP_EVENT_STA_GOT_IP,
                    &wifi_event_handler,
                    NULL,
                    NULL));

    /*初始化WiFi */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* 设置WIFI模式为AP_STA */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    /* 初始化AP */
    ESP_LOGI(TAG_AP, "ESP_WIFI_MODE_AP");
    esp_netif_t *esp_netif_ap = wifi_init_softap();

    /* 初始化STA */
    ESP_LOGI(TAG_STA, "ESP_WIFI_MODE_STA");
    esp_netif_t *esp_netif_sta = wifi_init_sta();

    /* 启动WiFi */
    ESP_ERROR_CHECK(esp_wifi_start() );



    // /* 等待直到连接成功或超过最大重连次数 */
    // EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
    //                                        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
    //                                        pdFALSE,
    //                                        pdFALSE,
    //                                        portMAX_DELAY);
    //
    // /* xEventGroupWaitBits 返回的是在调用该函数之前事件组中的事件标志位的值即bits。
    //  * WIFI_CONNECTED_BIT或WIFI_FAIL_BIT只要有1个发生，则bits和WIFI_CONNECTED_BIT按位与后值为1
    //  */
    // if (bits & WIFI_CONNECTED_BIT) {
    //     ESP_LOGI(TAG_STA, "connected to ap SSID:%s password:%s",
    //              ESP_WIFI_STA_SSID, ESP_WIFI_STA_PASSWD);
    // } else if (bits & WIFI_FAIL_BIT) {
    //     ESP_LOGI(TAG_STA, "Failed to connect to SSID:%s, password:%s",
    //              ESP_WIFI_STA_SSID, ESP_WIFI_STA_PASSWD);
    // } else {
    //     ESP_LOGE(TAG_STA, "UNEXPECTED EVENT");
    //     return;
    // }

    /* 设置sta为默认网络接口 */
    esp_netif_set_default_netif(esp_netif_sta);

    /* 启用NAPT */
    if (esp_netif_napt_enable(esp_netif_ap) != ESP_OK) {
        ESP_LOGE(TAG_STA, "NAPT not enabled on the netif: %p", esp_netif_ap);
    }

    ESP_LOGI("TEST", "hello world");
}



// void wifi_ap_init(void) {
//     /* 初始化网卡 */
//     ESP_ERROR_CHECK(esp_netif_init());
//
//     /* 创建新的事件循环 */
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//
//     /* 使用默认配置初始化包括netif的WiFi */
//     esp_netif_create_default_wifi_ap();
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));
//
//     ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
//     /* 配置WiFi */
//     wifi_config_t wifi_config = {
//         .ap = {
//             .ssid = WIFI_SSID,
//             .ssid_len = strlen(WIFI_SSID),
//             .password = WIFI_PASS,
//             .max_connection = MAX_STA_CONN,
//             .authmode = WIFI_AUTH_WPA_WPA2_PSK
//         },
//     };
//
//     if (strlen(WIFI_PASS) == 0) {
//         wifi_config.ap.authmode = WIFI_AUTH_OPEN;
//     }
//
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
//     ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());
//
//     esp_netif_ip_info_t ip_info;
//     esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);
//
//     char ip_addr[16];
//     inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
//     ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s ip:%s", WIFI_SSID, WIFI_PASS, ip_addr);
// }
