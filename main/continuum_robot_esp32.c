#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "usart.h"
#include "nvs_flash.h"
#include "step.h"
#include "wifi.h"
#include "lwip.h"

void start_main_task(void *pvParameters);

void app_main(void)
{
    esp_err_t ret;
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    usart_init(115200);

    wifi_sta_init();


    xTaskCreatePinnedToCore(start_main_task, "main_task", 2048, NULL, 5, NULL, 1);
}

void start_main_task(void *pvParameters)
{
    // const char* str = "Hello world!\r\n";
    // uint8_t str[4] = {0};
    // str[0] = 0x01;
    // str[1] = 0x02;
    // str[2] = 0x03;
    // str[3] = 0x04;

    while (1)
    {
        // uart_write_bytes(USART_UX, str, sizeof(str));   /* 写数据 */
        Traj_Position_Control(1, 1, 1000, 1000, 2000.0f, 3600.0f, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}