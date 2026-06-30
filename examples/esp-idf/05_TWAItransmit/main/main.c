/*****************************************************************************
 * | File         :   main.c
 * | Author       :   Waveshare team
 * | Function     :   Main function for CAN communication
 * | Info         :
 * |                 Basic implementation for handling CAN bus communication.
 * ----------------
 * | This version :   V1.0
 * | Date         :   2024-11-28
 * | Info         :   Basic version for initializing CAN communication and
 * |                 handling CAN alerts.
 *
 ******************************************************************************/

#include "freertos/FreeRTOS.h"  // FreeRTOS main include file
#include "freertos/task.h"      // FreeRTOS task management functions
#include "can.h"                // Include CAN driver for CAN communication

// Main application entry point
void app_main()
{
    // TWAI configuration settings
    static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();                                                 // Set CAN bus speed to 500 kbps
    static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();                                              // Accept all incoming CAN messages
    static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL); // General configuration, set TX/RX GPIOs and mode

    // Initialize the CAN communication interface
    can_init(t_config, f_config, g_config);  // Initialize CAN with specified configurations

    twai_message_t message;
    message.identifier = 0x123;  // 设置 CAN ID
    message.extd = 0;            // 标准帧
    message.rtr = 0;             // 数据帧
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
        can_write_Byte(message); // 发送 CAN 帧
        vTaskDelay(pdMS_TO_TICKS(1000)); // 每 1 秒发送一次
    }
}
