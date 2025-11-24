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

    uint32_t alerts_triggered; 
    twai_message_t message;

    // Main loop to monitor and handle CAN bus events
    while (1)
    {
        alerts_triggered = can_read_alerts();  // Check for any triggered CAN bus alerts
        
        // If new CAN data is received, read and transmit it
        if (alerts_triggered & TWAI_ALERT_RX_DATA)
        {
            message = can_read_Byte();      // Read the received CAN message
            can_write_Byte(message);        // Re-transmit the received message
        }
    }
}
