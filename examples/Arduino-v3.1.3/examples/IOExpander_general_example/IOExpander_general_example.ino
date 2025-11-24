#include <Arduino.h>
#include <esp_io_expander.hpp>
#define EXAMPLE_CHIP_NAME TCA95XX_8BIT
#define EXAMPLE_I2C_SDA_PIN (15)
#define EXAMPLE_I2C_SCL_PIN (7)
#define EXAMPLE_I2C_ADDR (ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000)

#define _EXAMPLE_CHIP_CLASS(name, ...) esp_expander::name(__VA_ARGS__)
#define EXAMPLE_CHIP_CLASS(name, ...) _EXAMPLE_CHIP_CLASS(name, ##__VA_ARGS__)

esp_expander::Base *expander = nullptr;

void setup() {
  Serial.begin(115200);
  Serial.println("Test begin");

  expander = new EXAMPLE_CHIP_CLASS(
    EXAMPLE_CHIP_NAME, EXAMPLE_I2C_SCL_PIN, EXAMPLE_I2C_SDA_PIN, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000);
  expander->init();
  expander->begin();

  expander->printStatus();

  expander->multiPinMode(IO_EXPANDER_PIN_NUM_0 | IO_EXPANDER_PIN_NUM_1 | IO_EXPANDER_PIN_NUM_2 | IO_EXPANDER_PIN_NUM_3 | IO_EXPANDER_PIN_NUM_5, OUTPUT);

  expander->printStatus();

  // Touch reset
  expander->digitalWrite(0, LOW);
  delay(100);
  expander->digitalWrite(0, HIGH);

  // Backlight control
  expander->digitalWrite(1, LOW);
  delay(100);
  expander->digitalWrite(1, HIGH);
  // LCD reset
  expander->digitalWrite(2, LOW);
  delay(100);
  expander->digitalWrite(2, HIGH);
  // SD card interrupt, need to pull up when starting
  expander->digitalWrite(3, LOW);
  delay(100);
  expander->digitalWrite(3, LOW);

  // Buzzer control
  expander->digitalWrite(5, HIGH);
  delay(100);
  expander->digitalWrite(5, LOW);

  expander->printStatus();
}

void loop() {

}
