#include "WS_CH32_IO.h"

namespace WS_CH32_IO {

static void logLine(Print *log, const char *text)
{
    if (log) {
        log->println(text);
    }
}

void recoverI2CBus(int sda, int scl)
{
    Wire.end();
    delay(2);

#ifdef OUTPUT_OPEN_DRAIN
    const uint8_t open_drain_mode = OUTPUT_OPEN_DRAIN;
#else
    const uint8_t open_drain_mode = OUTPUT;
#endif

    pinMode(sda, INPUT_PULLUP);
    pinMode(scl, open_drain_mode);
    digitalWrite(scl, HIGH);
    delayMicroseconds(10);

    for (int i = 0; i < 9 && digitalRead(sda) == LOW; ++i) {
        digitalWrite(scl, LOW);
        delayMicroseconds(10);
        digitalWrite(scl, HIGH);
        delayMicroseconds(10);
    }

    pinMode(sda, open_drain_mode);
    digitalWrite(sda, LOW);
    delayMicroseconds(10);
    digitalWrite(scl, HIGH);
    delayMicroseconds(10);
    digitalWrite(sda, HIGH);
    delayMicroseconds(10);

    pinMode(sda, INPUT_PULLUP);
    pinMode(scl, INPUT_PULLUP);
    delay(2);
}

bool probe(TwoWire &wire, uint8_t address)
{
    wire.beginTransmission(address);
    return wire.endTransmission() == 0;
}

bool writeRegister(TwoWire &wire, uint8_t reg, uint8_t value,
                   uint8_t address, uint8_t retries)
{
    for (uint8_t attempt = 0; attempt < retries; ++attempt) {
        wire.beginTransmission(address);
        wire.write(reg);
        wire.write(value);
        if (wire.endTransmission() == 0) {
            return true;
        }
        delay(10 + attempt * 10);
    }
    return false;
}

bool readRegisterBytes(TwoWire &wire, uint8_t reg, uint8_t *data, size_t len,
                       uint8_t address, uint8_t retries)
{
    if (!data || len == 0) {
        return false;
    }

    for (uint8_t attempt = 0; attempt < retries; ++attempt) {
        wire.beginTransmission(address);
        wire.write(reg);
        if (wire.endTransmission(false) == 0) {
            size_t got = wire.requestFrom((int)address, (int)len);
            if (got == len) {
                for (size_t i = 0; i < len; ++i) {
                    data[i] = wire.read();
                }
                return true;
            }
        }
        delay(10 + attempt * 10);
    }
    return false;
}

bool readRegister(TwoWire &wire, uint8_t reg, uint8_t *value,
                  uint8_t address, uint8_t retries)
{
    return readRegisterBytes(wire, reg, value, 1, address, retries);
}

bool initDisplayPower(TwoWire &wire, Print *log)
{
    for (int attempt = 0; attempt < 3; ++attempt) {
        if (!probe(wire)) {
            delay(20);
            continue;
        }

        if (!writeRegister(wire, REG_DIRECTION, DIR_DEFAULT)) {
            delay(20);
            continue;
        }
        if (!writeRegister(wire, REG_OUTPUT, OUT_RESET)) {
            delay(20);
            continue;
        }

        delay(200);

        if (!writeRegister(wire, REG_DIRECTION, DIR_DEFAULT)) {
            delay(20);
            continue;
        }
        if (!writeRegister(wire, REG_OUTPUT, OUT_DISPLAY_ON)) {
            delay(20);
            continue;
        }

        delay(200);
        logLine(log, "CH32V003 IO expander initialized");
        return true;
    }

    logLine(log, "CH32V003 IO expander init failed");
    return false;
}

bool begin(TwoWire &wire, int sda, int scl, uint32_t frequency, Print *log)
{
    recoverI2CBus(sda, scl);
    wire.begin(sda, scl);
    wire.setClock(frequency);
    delay(20);
    return initDisplayPower(wire, log);
}

bool setPwm(TwoWire &wire, uint8_t value)
{
    return writeRegister(wire, REG_PWM, value);
}

bool readAdcRaw(TwoWire &wire, uint16_t *raw)
{
    if (!raw) {
        return false;
    }

    uint8_t data[2] = {0};
    if (!readRegisterBytes(wire, REG_ADC, data, sizeof(data))) {
        return false;
    }
    *raw = ((uint16_t)data[1] << 8) | data[0];
    return true;
}

float batteryVoltageFromRaw(uint16_t raw, float adc_ref, float divider)
{
    if (raw > 1023) {
        raw = 1023;
    }
    return ((float)raw * adc_ref / 1023.0f) * divider;
}

bool readBatteryVoltage(TwoWire &wire, float *voltage, uint16_t *raw, uint8_t samples)
{
    if (!voltage || samples == 0) {
        return false;
    }

    uint32_t total = 0;
    uint8_t valid = 0;
    for (uint8_t i = 0; i < samples; ++i) {
        uint16_t sample = 0;
        if (readAdcRaw(wire, &sample)) {
            total += sample;
            ++valid;
        }
        delay(5);
    }

    if (valid == 0) {
        return false;
    }

    uint16_t average = (uint16_t)((total + valid / 2) / valid);
    if (raw) {
        *raw = average;
    }
    *voltage = batteryVoltageFromRaw(average);
    return true;
}

} // namespace WS_CH32_IO
