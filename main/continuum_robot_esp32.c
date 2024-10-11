#include <data_rev.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "usart.h"
#include "nvs_flash.h"
#include "step.h"
#include "softap_sta.h"
#include "tcp_server.h"



void start_main_task(void *pvParameters);

void app_main(void)
{

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    usart_init(115200);

    wifi_start();

    tcp_task_start();

    xTaskCreatePinnedToCore(start_main_task, "main_task", 2048, NULL, 5, NULL, 1);
}

void start_main_task(void *pvParameters)
{
    // char* str = "Hello world!\r\n";
    // uint8_t str[4] = {0};
    // str[0] = 0x01;
    // str[1] = 0x02;
    // str[2] = 0x03;
    // str[3] = 0x04;
    initRingBuff();

    StepMotor_t stepMotor;

    StepMotor_t stepMotor_1;
    StepMotor_t stepMotor_2;
    StepMotor_t stepMotor_3;
    StepMotor_t stepMotor_4;

    while (1)
    {
        // char str[6];
        // char str_1[6];

        initDataPos();
        initDataPosMul();
        initGetResult();
        initGetResultMul();

        isDataTrue();

        if (usize >= 8) {
            if (u(0) == 'U') {
                if (u(1) == '1') {
                    dataDivision();
                    convertResult();

                    stepMotor.num = u(dataPos.count_1 - 2);
                    stepMotor.dir = u(dataPos.count_1 - 1);

                    stepMotor.speed = getResult.result_1;
                    stepMotor.angle = getResult.result_2;

                    // 将数字转换为字符
                    // sprintf(str, "%d", getResult.result_1);
                    // sprintf(str_1, "%d", getResult.result_2);
                    if (stepMotor.angle != 0) {
                        pulseOutput_single(stepMotor.dir, stepMotor.speed, stepMotor.angle, stepMotor.num);
                    }
                    udelete(8);
                }
                else if (u(1) == '0') {
                    Stop_Now(0, 0);
                    udelete(8);
                }
            }
            else if (u(0) == 'V') {
                if (u(1) == '1') {
                    dataDivisionMul();
                    convertResultMul();

                    stepMotor_1.num = u(dataPosMul.countMul_1 - 2);
                    stepMotor_1.dir = u(dataPosMul.countMul_1 - 1);
                    stepMotor_2.num = u(dataPosMul.countMul_3 - 2);
                    stepMotor_2.dir = u(dataPosMul.countMul_3 - 1);
                    stepMotor_3.num = u(dataPosMul.countMul_5 - 2);
                    stepMotor_3.dir = u(dataPosMul.countMul_5 - 1);
                    stepMotor_4.num = u(dataPosMul.countMul_7 - 2);
                    stepMotor_4.dir = u(dataPosMul.countMul_7 - 1);

                    stepMotor_1.speed = getResultMul.resultMul_1;
                    stepMotor_1.angle = getResultMul.resultMul_2;
                    stepMotor_2.speed = getResultMul.resultMul_3;
                    stepMotor_2.angle = getResultMul.resultMul_4;
                    stepMotor_3.speed = getResultMul.resultMul_5;
                    stepMotor_3.angle = getResultMul.resultMul_6;
                    stepMotor_4.speed = getResultMul.resultMul_7;
                    stepMotor_4.angle = getResultMul.resultMul_8;

                    if (stepMotor_1.angle != 0 && stepMotor_2.angle != 0 && stepMotor_3.angle != 0 && stepMotor_4.angle != 0) {
                        pulseOutput(stepMotor_1.dir, stepMotor_1.num, stepMotor_1.speed, stepMotor_1.angle);
                        pulseOutput(stepMotor_2.dir, stepMotor_2.num, stepMotor_2.speed, stepMotor_2.angle);
                        pulseOutput(stepMotor_3.dir, stepMotor_3.num, stepMotor_3.speed, stepMotor_3.angle);
                        pulseOutput(stepMotor_4.dir, stepMotor_4.num, stepMotor_4.speed, stepMotor_4.angle);
                        // Traj_Position_Control(stepNumStrToData(stepMotor_1.num), stepDirStrToData(stepMotor_1.dir), 200, 200, (float)stepMotor_1.speed, (float)stepMotor_1.angle, 0, 1);
                        // Traj_Position_Control(stepNumStrToData(stepMotor_2.num), stepDirStrToData(stepMotor_2.dir), 200, 200, (float)stepMotor_2.speed, (float)stepMotor_2.angle, 0, 1);
                        // Traj_Position_Control(stepNumStrToData(stepMotor_3.num), stepDirStrToData(stepMotor_3.dir), 200, 200, (float)stepMotor_3.speed, (float)stepMotor_3.angle, 0, 1);
                        // Traj_Position_Control(stepNumStrToData(stepMotor_4.num), stepDirStrToData(stepMotor_4.dir), 200, 200, (float)stepMotor_4.speed, (float)stepMotor_4.angle, 0, 1);
                        Synchronous_motion(0);
                    }
                    else if (stepMotor_1.angle == 0 && stepMotor_2.angle != 0 && stepMotor_3.angle != 0 && stepMotor_4.angle != 0) {
                        pulseOutput(stepMotor_2.dir, stepMotor_2.num, stepMotor_2.speed, stepMotor_2.angle);
                        pulseOutput(stepMotor_3.dir, stepMotor_3.num, stepMotor_3.speed, stepMotor_3.angle);
                        pulseOutput(stepMotor_4.dir, stepMotor_4.num, stepMotor_4.speed, stepMotor_4.angle);
                        Synchronous_motion(0);
                    }
                    else if (stepMotor_1.angle != 0 && stepMotor_2.angle == 0 && stepMotor_3.angle != 0 && stepMotor_4.angle != 0) {
                        pulseOutput(stepMotor_1.dir, stepMotor_1.num, stepMotor_1.speed, stepMotor_1.angle);
                        pulseOutput(stepMotor_3.dir, stepMotor_3.num, stepMotor_3.speed, stepMotor_3.angle);
                        pulseOutput(stepMotor_4.dir, stepMotor_4.num, stepMotor_4.speed, stepMotor_4.angle);
                        Synchronous_motion(0);
                    }
                    else if (stepMotor_1.angle != 0 && stepMotor_2.angle != 0 && stepMotor_3.angle == 0 && stepMotor_4.angle != 0) {
                        pulseOutput(stepMotor_1.dir, stepMotor_1.num, stepMotor_1.speed, stepMotor_1.angle);
                        pulseOutput(stepMotor_2.dir, stepMotor_2.num, stepMotor_2.speed, stepMotor_2.angle);
                        pulseOutput(stepMotor_4.dir, stepMotor_4.num, stepMotor_4.speed, stepMotor_4.angle);
                        Synchronous_motion(0);
                    }
                    else if (stepMotor_1.angle != 0 && stepMotor_2.angle != 0 && stepMotor_3.angle != 0 && stepMotor_4.angle == 0) {
                        pulseOutput(stepMotor_1.dir, stepMotor_1.num, stepMotor_1.speed, stepMotor_1.angle);
                        pulseOutput(stepMotor_2.dir, stepMotor_2.num, stepMotor_2.speed, stepMotor_2.angle);
                        pulseOutput(stepMotor_3.dir, stepMotor_3.num, stepMotor_3.speed, stepMotor_3.angle);
                        Synchronous_motion(0);
                    }
                    else if (stepMotor_1.angle == 0 && stepMotor_2.angle == 0 && stepMotor_3.angle != 0 && stepMotor_4.angle != 0) {
                        pulseOutput(stepMotor_3.dir, stepMotor_3.num, stepMotor_3.speed, stepMotor_3.angle);
                        pulseOutput(stepMotor_4.dir, stepMotor_4.num, stepMotor_4.speed, stepMotor_4.angle);
                        Synchronous_motion(0);
                    }
                    else if (stepMotor_1.angle == 0 && stepMotor_2.angle != 0 && stepMotor_3.angle == 0 && stepMotor_4.angle != 0) {
                        pulseOutput(stepMotor_2.dir, stepMotor_2.num, stepMotor_2.speed, stepMotor_2.angle);
                        pulseOutput(stepMotor_4.dir, stepMotor_4.num, stepMotor_4.speed, stepMotor_4.angle);
                        Synchronous_motion(0);
                    }
                    else if (stepMotor_1.angle == 0 && stepMotor_2.angle != 0 && stepMotor_3.angle != 0 && stepMotor_4.angle == 0) {
                        pulseOutput(stepMotor_2.dir, stepMotor_2.num, stepMotor_2.speed, stepMotor_2.angle);
                        pulseOutput(stepMotor_3.dir, stepMotor_3.num, stepMotor_3.speed, stepMotor_3.angle);
                        Synchronous_motion(0);
                    }
                    else if (stepMotor_1.angle != 0 && stepMotor_2.angle == 0 && stepMotor_3.angle == 0 && stepMotor_4.angle != 0) {
                        pulseOutput(stepMotor_1.dir, stepMotor_1.num, stepMotor_1.speed, stepMotor_1.angle);
                        pulseOutput(stepMotor_4.dir, stepMotor_4.num, stepMotor_4.speed, stepMotor_4.angle);
                        Synchronous_motion(0);
                    }
                    else if (stepMotor_1.angle != 0 && stepMotor_2.angle == 0 && stepMotor_3.angle != 0 && stepMotor_4.angle == 0) {
                        pulseOutput(stepMotor_1.dir, stepMotor_1.num, stepMotor_1.speed, stepMotor_1.angle);
                        pulseOutput(stepMotor_3.dir, stepMotor_3.num, stepMotor_3.speed, stepMotor_3.angle);
                        Synchronous_motion(0);
                    }
                    else if (stepMotor_1.angle != 0 && stepMotor_2.angle != 0 && stepMotor_3.angle == 0 && stepMotor_4.angle == 0) {
                        pulseOutput(stepMotor_1.dir, stepMotor_1.num, stepMotor_1.speed, stepMotor_1.angle);
                        pulseOutput(stepMotor_2.dir, stepMotor_2.num, stepMotor_2.speed, stepMotor_2.angle);
                        Synchronous_motion(0);
                    }
                    else if (stepMotor_1.angle == 0 && stepMotor_2.angle == 0 && stepMotor_3.angle == 0 && stepMotor_4.angle != 0) {
                        pulseOutput(stepMotor_4.dir, stepMotor_4.num, stepMotor_4.speed, stepMotor_4.angle);
                    }
                    else if (stepMotor_1.angle == 0 && stepMotor_2.angle == 0 && stepMotor_3.angle != 0 && stepMotor_4.angle == 0) {
                        pulseOutput(stepMotor_3.dir, stepMotor_3.num, stepMotor_3.speed, stepMotor_3.angle);
                    }
                    else if (stepMotor_1.angle == 0 && stepMotor_2.angle != 0 && stepMotor_3.angle == 0 && stepMotor_4.angle == 0) {
                        pulseOutput(stepMotor_2.dir, stepMotor_2.num, stepMotor_2.speed, stepMotor_2.angle);
                    }
                    else if (stepMotor_1.angle != 0 && stepMotor_2.angle == 0 && stepMotor_3.angle == 0 && stepMotor_4.angle == 0) {
                        pulseOutput(stepMotor_1.dir, stepMotor_1.num, stepMotor_1.speed, stepMotor_1.angle);
                    }
                    else if (stepMotor_1.angle == 0 && stepMotor_2.angle == 0 && stepMotor_3.angle == 0 && stepMotor_4.angle == 0) {
                        Stop_Now(0, 0);
                    }
                    udelete(8);
                }
                else if (u(1) == '0') {
                    Stop_Now(0, 0);
                    udelete(8);
                }
            }
            else if (u(0) == 'W') {
                if (u(1) == '1') {
                    Traj_Position_Control(1, 1, 200, 200, 500.0f, 0, 1, 1);
                    Traj_Position_Control(2, 1, 200, 200, 500.0f, 0, 1, 1);
                    Traj_Position_Control(3, 1, 200, 200, 500.0f, 0, 1, 1);
                    Traj_Position_Control(4, 1, 200, 200, 500.0f, 0, 1, 1);
                    Traj_Position_Control(5, 1, 200, 200, 500.0f, 0, 1, 1);
                    Traj_Position_Control(6, 1, 200, 200, 500.0f, 0, 1, 1);
                    Traj_Position_Control(7, 1, 200, 200, 500.0f, 0, 1, 1);
                    Traj_Position_Control(8, 1, 200, 200, 500.0f, 0, 1, 1);
                    Synchronous_motion(0);
                    udelete(8);
                }
                else if (u(1) == '0') {
                    Stop_Now(0, 0);
                    udelete(8);
                }
            }

        }

        // uart_write_bytes(USART_UX, str, sizeof(str));   /* 写数据 */
        // Traj_Position_Control(1, 1, 1000, 1000, 2000.0f, 3600.0f, 0, 0);
        // send_message(str, strlen(str));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}