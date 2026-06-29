#pragma once

#include <Arduino.h>
#include <Wire.h>

namespace WS_CH32_IO {

static constexpr uint8_t I2C_ADDRESS = 0x24;
static constexpr uint8_t REG_DIRECTION = 0x02;
static constexpr uint8_t REG_OUTPUT = 0x03;
static constexpr uint8_t REG_INPUT = 0x04;
static constexpr uint8_t REG_PWM = 0x05;
static constexpr uint8_t REG_ADC = 0x06;
static constexpr uint8_t REG_RTC_INT = 0x07;

static constexpr uint8_t PIN_LCD_TOUCH_RST = (1 << 1);
static constexpr uint8_t PIN_LCD_RST = (1 << 3);
static constexpr uint8_t PIN_SYS_EN = (1 << 5);
static constexpr uint8_t PIN_BEE_EN = (1 << 6);
static constexpr uint8_t PIN_RTC_INT = (1 << 7);

static constexpr uint8_t DIR_DEFAULT = 0xff;
static constexpr uint8_t OUT_RESET = 0x00;
static constexpr uint8_t OUT_DISPLAY_ON = PIN_SYS_EN | PIN_LCD_RST | PIN_LCD_TOUCH_RST;

static constexpr int DEFAULT_SDA = 15;
static constexpr int DEFAULT_SCL = 7;
static constexpr uint32_t DEFAULT_I2C_FREQ = 400000;
static constexpr float ADC_REF_VOLTAGE = 3.3f;
static constexpr float BATTERY_DIVIDER = 3.0f;

void recoverI2CBus(int sda = DEFAULT_SDA, int scl = DEFAULT_SCL);
bool begin(TwoWire &wire = Wire, int sda = DEFAULT_SDA, int scl = DEFAULT_SCL,
           uint32_t frequency = DEFAULT_I2C_FREQ, Print *log = nullptr);
bool probe(TwoWire &wire = Wire, uint8_t address = I2C_ADDRESS);
bool writeRegister(TwoWire &wire, uint8_t reg, uint8_t value,
                   uint8_t address = I2C_ADDRESS, uint8_t retries = 5);
bool readRegister(TwoWire &wire, uint8_t reg, uint8_t *value,
                  uint8_t address = I2C_ADDRESS, uint8_t retries = 5);
bool readRegisterBytes(TwoWire &wire, uint8_t reg, uint8_t *data, size_t len,
                       uint8_t address = I2C_ADDRESS, uint8_t retries = 5);
bool initDisplayPower(TwoWire &wire = Wire, Print *log = nullptr);
bool setPwm(TwoWire &wire, uint8_t value);
bool readAdcRaw(TwoWire &wire, uint16_t *raw);
float batteryVoltageFromRaw(uint16_t raw, float adc_ref = ADC_REF_VOLTAGE,
                            float divider = BATTERY_DIVIDER);
bool readBatteryVoltage(TwoWire &wire, float *voltage, uint16_t *raw = nullptr,
                        uint8_t samples = 8);

} // namespace WS_CH32_IO
