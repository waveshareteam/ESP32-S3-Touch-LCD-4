#include "twai.h"

static bool driver_installed = false; // Flag to check if the driver is installed

void setup() {
  // Start Serial 
  Serial.begin(115200); // Initialize serial communication at 115200 baud rate
  
  // TWAI configuration settings
  static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();                                                 // Set CAN bus speed to 500 kbps
  static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();                                              // Accept all incoming CAN messages
  static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL); // General configuration, set TX/RX GPIOs and mode
  
  // Initialize the CAN communication interface
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    driver_installed = true;
    Serial.println("TWAI driver installed successfully.");
  } else {
    Serial.println("Failed to install TWAI driver.");
    return;
  }

  // Start the TWAI driver
  if (twai_start() == ESP_OK) {
    Serial.println("TWAI driver started successfully.");
  } else {
    Serial.println("Failed to start TWAI driver.");
    return;
  }
}

void loop() {
  if (driver_installed) {
    twai_message_t message;
    message.identifier = 0x123; // CAN message ID
    message.extd = 0;           // Standard frame (not extended)
    message.data_length_code = 8; // Data length (8 bytes)
    message.data[0] = 0x01;     // Data bytes
    message.data[1] = 0x02;
    message.data[2] = 0x03;
    message.data[3] = 0x04;
    message.data[4] = 0x05;
    message.data[5] = 0x06;
    message.data[6] = 0x07;
    message.data[7] = 0x08;

    // Send the CAN message
    if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
      Serial.println("Message sent successfully.");
    } else {
      Serial.println("Failed to send message.");
    }
  }

  delay(1000); // Send a message every second
}