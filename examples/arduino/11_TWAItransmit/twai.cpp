/*****************************************************************************
 * | File         :   can.c
 * | Author       :   Waveshare team
 * | Function     :   CAN driver code for CAN communication
 * | Info         :
 * ----------------
 * | This version :   V1.0
 * | Date         :   2024-11-28
 * | Info         :   Basic version, includes functions to initialize, 
 * |                 read alerts, and transmit/receive CAN messages.
 *
 ******************************************************************************/

#include "twai.h"  // Include header file for CAN driver functions

/**
 * @brief Initializes the CAN (TWAI) interface.
 *
 * This function installs and starts the TWAI driver with the specified
 * configurations. It also sets up alert conditions to monitor the CAN bus.
 *
 * @param t_config Timing configuration for CAN communication.
 * @param f_config Filter configuration for CAN messages.
 * @param g_config General configuration for the CAN interface.
 *
 * @return ESP_OK on success, ESP_FAIL otherwise.
 */
esp_err_t can_init(twai_timing_config_t t_config, twai_filter_config_t f_config, twai_general_config_t g_config)
{
    // Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        ESP_LOGI(CAN_TAG, "Driver installed"); // Log driver installation success
    }
    else
    {
        ESP_LOGI(CAN_TAG, "Failed to install driver"); // Log driver installation failure
        return ESP_FAIL;
    }

    // Start TWAI driver
    if (twai_start() == ESP_OK)
    {
        ESP_LOGI(CAN_TAG, "Driver started"); // Log driver start success
    }
    else
    {
        ESP_LOGI(CAN_TAG, "Failed to start driver"); // Log driver start failure
        return ESP_FAIL;
    }

    // Configure alerts for specific CAN events
    uint32_t alerts_to_enable = TWAI_ALERT_TX_SUCCESS | TWAI_ALERT_TX_FAILED |
                                TWAI_ALERT_RX_DATA | TWAI_ALERT_RX_QUEUE_FULL |
                                TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR;
    if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK)
    {
        ESP_LOGI(CAN_TAG, "CAN Alerts reconfigured"); // Log alert reconfiguration success
    }
    else
    {
        ESP_LOGI(CAN_TAG, "Failed to reconfigure alerts"); // Log alert reconfiguration failure
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief Reads and handles alerts from the CAN interface.
 *
 * This function reads triggered alerts and provides detailed logs for each alert type.
 *
 * @return A 32-bit value representing the triggered alert type.
 */
uint32_t can_read_alerts()
{
    uint32_t alerts_triggered;                                           // Variable to store triggered alerts
    twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(POLLING_RATE_MS)); // Read alerts
    twai_status_info_t twaistatus;                                       // Variable to store TWAI status information
    twai_get_status_info(&twaistatus);                                   // Get TWAI status information

    if (alerts_triggered & TWAI_ALERT_ERR_PASS)
    {
        ESP_LOGI(CAN_TAG, "Alert: TWAI controller is in error passive state.");
        return TWAI_ALERT_ERR_PASS;
    }

    if (alerts_triggered & TWAI_ALERT_BUS_ERROR)
    {
        ESP_LOGI(CAN_TAG, "Alert: Bus error occurred.");
        ESP_LOGI(CAN_TAG, "Bus error count: %" PRIu32, twaistatus.bus_error_count);
        return TWAI_ALERT_BUS_ERROR;
    }

    if (alerts_triggered & TWAI_ALERT_TX_FAILED)
    {
        ESP_LOGI(CAN_TAG, "Alert: Transmission failed.");
        ESP_LOGI(CAN_TAG, "TX buffered: %" PRIu32, twaistatus.msgs_to_tx);
        return TWAI_ALERT_TX_FAILED;
    }

    if (alerts_triggered & TWAI_ALERT_TX_SUCCESS)
    {
        ESP_LOGI(CAN_TAG, "Alert: Transmission successful.");
        return TWAI_ALERT_TX_SUCCESS;
    }

    if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL)
    {
        ESP_LOGI(CAN_TAG, "Alert: RX queue full, frame lost.");
        return TWAI_ALERT_RX_QUEUE_FULL;
    }

    if (alerts_triggered & TWAI_ALERT_RX_DATA)
    {
        return TWAI_ALERT_RX_DATA;
    }

    return 0;
}

/**
 * @brief Transmits a CAN message.
 *
 * This function queues a CAN message for transmission and logs the result.
 *
 * @param message The CAN message to be transmitted.
 */
void can_write_Byte(twai_message_t message)
{
    if (twai_transmit(&message, portMAX_DELAY) == ESP_OK)
    {
        printf("Message queued for transmission\n"); // Log success
    }
    else
    {
        printf("Failed to queue message for transmission\n"); // Log failure
    }
}

/**
 * @brief Receives a CAN message.
 *
 * This function reads and logs received messages from the CAN interface.
 *
 * @return The received CAN message.
 */
twai_message_t can_read_Byte()
{
    twai_message_t message; // Variable to hold received message
    while (twai_receive(&message, 0) == ESP_OK)
    {
        if (message.extd)
        {
            ESP_LOGI(CAN_TAG, "Message is in Extended Format");
        }
        else
        {
            ESP_LOGI(CAN_TAG, "Message is in Standard Format");
        }

        printf("ID: %lx\nByte:", message.identifier);
        if (!message.rtr)
        {
            for (int i = 0; i < message.data_length_code; i++)
            {
                printf(" %d = %02x,", i, message.data[i]);
            }
            printf("\n");
        }
    }
    return message;
}
