#ifndef CAN_HPP
#define CAN_HPP

#include "driver/twai.h"
#include <cstdint>

namespace CAN {

// GPIO pins used for CAN communication
constexpr gpio_num_t TX_GPIO_NUM = GPIO_NUM_6;
constexpr gpio_num_t RX_GPIO_NUM = GPIO_NUM_0;
constexpr const char* CAN_TAG = "TWAI Master";

// Transmission and polling intervals
constexpr uint32_t TRANSMIT_RATE_MS = 1000;
constexpr uint32_t POLLING_RATE_MS = 1000;

/**
 * @brief Initialize CAN interface
 * @param t_config Timing configuration
 * @param f_config Filter configuration
 * @param g_config General configuration
 * @return esp_err_t ESP_OK on success
 */
esp_err_t init(const twai_timing_config_t& t_config,
              const twai_filter_config_t& f_config,
              const twai_general_config_t& g_config);

/**
 * @brief Read and handle CAN alerts
 * @return uint32_t Triggered alerts bitmask
 */
uint32_t read_alerts();

/**
 * @brief Transmit CAN message
 * @param message Message to transmit
 */
void write_message(const twai_message_t& message);

/**
 * @brief Receive CAN message
 * @return twai_message_t Received message
 */
twai_message_t read_message();

} // namespace CAN

#endif // CAN_HPP