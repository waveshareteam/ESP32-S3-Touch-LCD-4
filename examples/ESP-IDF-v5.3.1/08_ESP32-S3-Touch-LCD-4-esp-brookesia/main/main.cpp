/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "bsp/touch.h"
#include "esp_brookesia.hpp"

/* 这些是 `esp-brookesia` 库中的内置应用示例 */
#include "app_examples/phone/simple_conf/src/phone_app_simple_conf.hpp"
#include "app_examples/phone/complex_conf/src/phone_app_complex_conf.hpp"
#include "app_examples/phone/squareline/src/phone_app_squareline.hpp"
#include "apps.h"
#include "can.hpp"
#define ECHO_TEST_TXD (44)
#define ECHO_TEST_RXD (43)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM ((uart_port_t)1)
#define ECHO_UART_BAUD_RATE (115200)
#define ECHO_TASK_STACK_SIZE (2048)

static const char *TAG = "main";

#define BUF_SIZE (1024)

void can_communication_task(void *pvParameters)
{
    // TWAI configuration settings
    static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN::TX_GPIO_NUM, CAN::RX_GPIO_NUM, TWAI_MODE_NORMAL);

    // Initialize the CAN communication interface
    esp_err_t err = CAN::init(t_config, f_config, g_config);
    if (err != ESP_OK)
    {
        // 处理初始化失败的情况
        vTaskDelete(NULL); // 删除当前任务
        return;
    }

    twai_message_t message;
    message.identifier = 0x123;   // 设置 CAN ID
    message.extd = 0;             // 标准帧
    message.rtr = 0;              // 数据帧
    message.data_length_code = 8; // 数据长度 8 字节
    message.data[0] = 0x11;
    message.data[1] = 0x22;
    message.data[2] = 0x33;
    message.data[3] = 0x44;
    message.data[4] = 0x55;
    message.data[5] = 0x66;
    message.data[6] = 0x77;
    message.data[7] = 0x88;

    // 持续发送 CAN 消息
    while (1)
    {
        CAN::write_message(message);           // 发送 CAN 帧
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 每 1 秒发送一次
    }
    vTaskDelete(NULL);
}

extern "C" void echo_task(void *arg)
{
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

    const char *response = "RS485 OK!";
    int len = strlen(response);

    while (1)
    {
        uart_write_bytes(ECHO_UART_PORT_NUM, response, len);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 每秒发送一次
    }

    vTaskDelete(NULL);
}

extern "C" void app_main(void)
{
    esp_io_expander_handle_t expander = bsp_io_expander_init();
    if (expander == NULL)
    {
        ESP_LOGE("IO Expander", "Failed to initialize IO expander");
        return;
    }

    esp_io_expander_set_dir(expander, IO_EXPANDER_PIN_NUM_5 | IO_EXPANDER_PIN_NUM_0 | IO_EXPANDER_PIN_NUM_2, IO_EXPANDER_OUTPUT);
    esp_rom_gpio_pad_select_gpio(GPIO_NUM_16);
    gpio_set_direction(GPIO_NUM_16, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_16, 0);
    esp_io_expander_set_level(expander, IO_EXPANDER_PIN_NUM_5, 1);
    esp_io_expander_set_level(expander, IO_EXPANDER_PIN_NUM_0, 0);
    esp_io_expander_set_level(expander, IO_EXPANDER_PIN_NUM_2, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    esp_io_expander_set_level(expander, IO_EXPANDER_PIN_NUM_5, 0);
    esp_io_expander_set_level(expander, IO_EXPANDER_PIN_NUM_0, 1);
    esp_io_expander_set_level(expander, IO_EXPANDER_PIN_NUM_2, 1);

    lv_display_t *disp = bsp_display_start();

    // if (disp != NULL)
    // {
    //     bsp_display_rotate(disp, LV_DISPLAY_ROTATION_180);
    // }

    bsp_display_lock(0);

    ESP_Brookesia_Phone *phone = new ESP_Brookesia_Phone(disp);
    assert(phone != nullptr && "Failed to create phone");

    ESP_Brookesia_PhoneStylesheet_t *stylesheet = nullptr;
    stylesheet = new ESP_Brookesia_PhoneStylesheet_t ESP_BROOKESIA_PHONE_480_480_DARK_STYLESHEET();
    ESP_BROOKESIA_CHECK_NULL_EXIT(stylesheet, "Create stylesheet failed");

    if (stylesheet != nullptr)
    {
        ESP_LOGI(TAG, "Using stylesheet (%s)", stylesheet->core.name);
        ESP_BROOKESIA_CHECK_FALSE_EXIT(phone->addStylesheet(stylesheet), "Add stylesheet failed");
        ESP_BROOKESIA_CHECK_FALSE_EXIT(phone->activateStylesheet(stylesheet), "Activate stylesheet failed");
        delete stylesheet;
    }

    ESP_BROOKESIA_CHECK_FALSE_EXIT(phone->setTouchDevice(bsp_display_get_input_dev()), "Set touch device failed");
    phone->registerLvLockCallback((ESP_Brookesia_LvLockCallback_t)(bsp_display_lock), 0);
    phone->registerLvUnlockCallback((ESP_Brookesia_LvUnlockCallback_t)(bsp_display_unlock));
    ESP_BROOKESIA_CHECK_FALSE_EXIT(phone->begin(), "Begin failed");

    Calculator *calculator = new Calculator();
    assert(calculator != nullptr && "Failed to create calculator");
    assert((phone->installApp(calculator) >= 0) && "Failed to begin calculator");

    Drawpanel *drawpanel = new Drawpanel();
    assert(drawpanel != nullptr && "Failed to create drawpanel");
    assert((phone->installApp(drawpanel) >= 0) && "Failed to begin drawpanel");

    PhoneAppSimpleConf *app_simple_conf = new PhoneAppSimpleConf();
    ESP_BROOKESIA_CHECK_NULL_EXIT(app_simple_conf, "Create app simple conf failed");
    ESP_BROOKESIA_CHECK_FALSE_EXIT((phone->installApp(app_simple_conf) >= 0), "Install app simple conf failed");

    // PhoneAppComplexConf *app_complex_conf = new PhoneAppComplexConf();
    // ESP_BROOKESIA_CHECK_NULL_EXIT(app_complex_conf, "Create app complex conf failed");
    // ESP_BROOKESIA_CHECK_FALSE_EXIT((phone->installApp(app_complex_conf) >= 0), "Install app complex conf failed");

    PhoneAppSquareline *app_squareline = new PhoneAppSquareline();
    ESP_BROOKESIA_CHECK_NULL_EXIT(app_squareline, "Create app squareline failed");
    ESP_BROOKESIA_CHECK_FALSE_EXIT((phone->installApp(app_squareline) >= 0), "Install app squareline failed");

    bsp_display_unlock();

    xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
    xTaskCreate(can_communication_task, "CAN_Communication_Task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
}
