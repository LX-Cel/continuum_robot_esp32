#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "usart.h"
#include "nvs_flash.h"

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

    usart_init(115200);

    xTaskCreatePinnedToCore(start_main_task, "main_task", 2048, NULL, 5, NULL, 1);
}

void start_main_task(void *pvParameters)
{
    const char* str = "Hello world!\n";;
    while (1)
    {
        uart_write_bytes(USART_UX, str, strlen(str));   /* 写数据 */
        ESP_LOGI("main_task", "Hello world!");
        vTaskDelay(pdMS_TO_TICKS(1000));

    }
}