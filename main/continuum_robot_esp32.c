#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

void start_main_task(void *pvParameters);

void app_main(void)
{
    xTaskCreatePinnedToCore(start_main_task, "main_task", 2048, NULL, 5, NULL, 1);
}

void start_main_task(void *pvParameters)
{
    while (1)
    {
        ESP_LOGI("main_task", "Hello world!");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}