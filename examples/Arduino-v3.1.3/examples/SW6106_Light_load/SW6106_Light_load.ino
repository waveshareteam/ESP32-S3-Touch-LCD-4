#include <Wire.h>
#include <Arduino.h>
#include <esp_io_expander.hpp>
#define EXAMPLE_CHIP_NAME TCA95XX_8BIT
#define EXAMPLE_I2C_SDA_PIN (15)
#define EXAMPLE_I2C_SCL_PIN (7)
#define EXAMPLE_I2C_ADDR (ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000)

#define _EXAMPLE_CHIP_CLASS(name, ...) esp_expander::name(__VA_ARGS__)
#define EXAMPLE_CHIP_CLASS(name, ...) _EXAMPLE_CHIP_CLASS(name, ##__VA_ARGS__)

esp_expander::Base *expander = nullptr;

#define DEVICE_ADDR 0x3C
#define REGISTER_ADDR 0x38
#define DATA_TO_WRITE 0x0A

void setup() {
  Wire.begin(15, 7);

  expander = new EXAMPLE_CHIP_CLASS(
    EXAMPLE_CHIP_NAME, I2C_NUM_0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000);
  expander->init();
  expander->begin();
  expander->pinMode(1, OUTPUT);
  expander->digitalWrite(1, LOW);

  Wire.beginTransmission(DEVICE_ADDR);
  Wire.write(REGISTER_ADDR);
  Wire.write(DATA_TO_WRITE);
  Wire.endTransmission();
  // Wire.end();
}

void loop() {
}
