# SW6106 Light load description
When you turn the backlight off using an application, it may happen that the device shuts down after a period of power on

* Write 0x0A to the 0x38 register of the 0x3C device to set the minimum limit of SW6106 light load detection
  * Arduino IDE
  ```C++
  #include <Wire.h>
  #define DEVICE_ADDR 0x3C
  #define REGISTER_ADDR 0x38
  #define DATA_TO_WRITE 0x0A
  
  void setup() {
  Wire.begin(15,7);
  Wire.beginTransmission(DEVICE_ADDR);
  Wire.write(REGISTER_ADDR);          
  Wire.write(DATA_TO_WRITE);
  Wire.end();
  }
  
  void loop() {
  
  }
  ```
  * ESP-IDF(You can use what you know about i2c low-level drivers)
  ```C
  #include "i2c_bus.h"

  i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 9,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = 8,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_bus_handle_t i2c0_bus = i2c_bus_create(I2C_NUM_0, &conf);
    i2c_bus_device_handle_t i2c0_device1 = i2c_bus_device_create(i2c0_bus, 0x3C, 0);
    uint8_t data = 0x0A;
    i2c_bus_write_bytes(i2c0_device1, 0x85, 1, &data);
  ```