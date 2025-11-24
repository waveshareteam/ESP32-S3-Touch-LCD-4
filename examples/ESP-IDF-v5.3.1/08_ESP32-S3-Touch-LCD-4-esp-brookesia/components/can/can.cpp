#include "can.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include <inttypes.h>

namespace CAN {

esp_err_t init(const twai_timing_config_t& t_config,
              const twai_filter_config_t& f_config,
              const twai_general_config_t& g_config)
{
    // Install TWAI driver
    esp_err_t ret = twai_driver_install(&g_config, &t_config, &f_config);
    if (ret != ESP_OK) {
        ESP_LOGI(CAN_TAG, "Failed to install driver: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(CAN_TAG, "Driver installed");

    // Start TWAI driver
    if ((ret = twai_start()) != ESP_OK) {
        ESP_LOGI(CAN_TAG, "Failed to start driver: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(CAN_TAG, "Driver started");

    // Configure alerts
    constexpr uint32_t alerts_to_enable = TWAI_ALERT_TX_SUCCESS | 
                                        TWAI_ALERT_TX_FAILED |
                                        TWAI_ALERT_RX_DATA | 
                                        TWAI_ALERT_RX_QUEUE_FULL |
                                        TWAI_ALERT_ERR_PASS | 
                                        TWAI_ALERT_BUS_ERROR;
    
    if ((ret = twai_reconfigure_alerts(alerts_to_enable, nullptr)) != ESP_OK) {
        ESP_LOGI(CAN_TAG, "Failed to reconfigure alerts: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(CAN_TAG, "CAN Alerts reconfigured");

    return ESP_OK;
}

uint32_t read_alerts()
{
    uint32_t alerts_triggered;
    twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(POLLING_RATE_MS));
    
    twai_status_info_t status;
    twai_get_status_info(&status);

    if (alerts_triggered & TWAI_ALERT_ERR_PASS) {
        ESP_LOGI(CAN_TAG, "Controller in error passive state");
    }
    if (alerts_triggered & TWAI_ALERT_BUS_ERROR) {
        ESP_LOGI(CAN_TAG, "Bus error count: %" PRIu32, status.bus_error_count);
    }
    if (alerts_triggered & TWAI_ALERT_TX_FAILED) {
        ESP_LOGI(CAN_TAG, "TX buffered: %" PRIu32, status.msgs_to_tx);
    }
    if (alerts_triggered & TWAI_ALERT_TX_SUCCESS) {
        ESP_LOGI(CAN_TAG, "Transmission successful");
    }
    if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL) {
        ESP_LOGI(CAN_TAG, "RX queue full");
    }

    return alerts_triggered;
}

void write_message(const twai_message_t& message)
{
    esp_err_t ret = twai_transmit(&message, portMAX_DELAY);
    // if (ret == ESP_OK) {
    //     ESP_LOGI(CAN_TAG, "Message queued for transmission");
    // } else {
    //     ESP_LOGI(CAN_TAG, "Transmission failed: %s", esp_err_to_name(ret));
    // }
}

twai_message_t read_message()
{
    twai_message_t message;
    while (twai_receive(&message, 0) == ESP_OK) {
        if (message.extd) {
            ESP_LOGI(CAN_TAG, "Extended format message");
        } else {
            ESP_LOGI(CAN_TAG, "Standard format message");
        }

        ESP_LOGI(CAN_TAG, "ID: 0x%08" PRIx32, message.identifier);
        if (!message.rtr) {
            ESP_LOGI(CAN_TAG, "Data:");
            for (uint8_t i = 0; i < message.data_length_code; i++) {
                ESP_LOGI(CAN_TAG, " [%d]: 0x%02x", i, message.data[i]);
            }
        }
    }
    return message;
}

} // namespace CAN