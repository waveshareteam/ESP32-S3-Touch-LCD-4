#include <Wire.h>
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "TouchDrvGT911.hpp"
#include <SPI.h>
#include "WS_CH32_IO.h"

#include "HWCDC.h"
HWCDC USBSerial;

TouchDrvGT911 GT911;
int16_t x[5], y[5];
uint8_t gt911_i2c_addr = 0;
bool gt911_available = false;

Arduino_DataBus *bus = new Arduino_SWSPI(
    GFX_NOT_DEFINED /* DC */, 42 /* CS */,
    2 /* SCK */, 1 /* MOSI */, GFX_NOT_DEFINED /* MISO */);

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    40 /* DE */, 39 /* VSYNC */, 38 /* HSYNC */, 41 /* PCLK */,
    46 /* R0 */, 3 /* R1 */, 8 /* R2 */, 18 /* R3 */, 17 /* R4 */,
    14 /* G0 */, 13 /* G1 */, 12 /* G2 */, 11 /* G3 */, 10 /* G4 */, 9 /* G5 */,
    5 /* B0 */, 45 /* B1 */, 48 /* B2 */, 47 /* B3 */, 21 /* B4 */,
    1 /* hsync_polarity */, 10 /* hsync_front_porch */, 8 /* hsync_pulse_width */, 50 /* hsync_back_porch */,
    1 /* vsync_polarity */, 10 /* vsync_front_porch */, 8 /* vsync_pulse_width */, 20 /* vsync_back_porch */);
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    480 /* width */, 480 /* height */, rgbpanel, 2 /* rotation */, true /* auto_flush */,
    bus, GFX_NOT_DEFINED /* RST */, st7701_type1_init_operations, sizeof(st7701_type1_init_operations));

void i2c_scan() {
  USBSerial.println("Scanning I2C bus...");
  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      USBSerial.print("I2C device found at address 0x");
      if (address < 16) {
        USBSerial.print("0");
      }
      USBSerial.println(address, HEX);
      nDevices++;
      
      if (address == GT911_SLAVE_ADDRESS_L || address == GT911_SLAVE_ADDRESS_H) {
        gt911_i2c_addr = address;
        USBSerial.print("Found GT911 candidate address: 0x");
        USBSerial.println(address, HEX);
      }
    } else if (error == 4) {
    }
  }

  if (nDevices == 0) {
    USBSerial.println("No I2C devices found");
  } else {
    USBSerial.println("I2C scan completed");
  }
}

bool init_gt911_with_probe(int sda_pin, int scl_pin) {
  Wire.begin(sda_pin, scl_pin);
  delay(100);
  
  i2c_scan();
  
  if (gt911_i2c_addr == 0) {
    USBSerial.println("GT911 not found in I2C scan");
    return false;
  }
  
  GT911.setPins(-1, -1);
  if (GT911.begin(Wire, gt911_i2c_addr, sda_pin, scl_pin)) {
    USBSerial.print("GT911 initialized successfully at address 0x");
    USBSerial.println(gt911_i2c_addr, HEX);
    return true;
  } else {
    USBSerial.print("Failed to initialize GT911 at address 0x");
    USBSerial.println(gt911_i2c_addr, HEX);
    return false;
  }
}

void setup() {
  USBSerial.begin(115200);

  if (!WS_CH32_IO::begin(Wire, WS_CH32_IO::DEFAULT_I2C_SDA, WS_CH32_IO::DEFAULT_I2C_SCL,
                         WS_CH32_IO::DEFAULT_I2C_FREQ, &USBSerial)) {
    USBSerial.println("CH32V003 IO expander init failed");
  }

  gfx->begin();
  gfx->fillScreen(WHITE);

  gt911_available = init_gt911_with_probe(15, 7);
  if (gt911_available) {
    GT911.setHomeButtonCallback([](void *user_data) {
      USBSerial.println("Home button pressed!");
    },
                                NULL);
    GT911.setMaxTouchPoint(1); // max is 5
  } else {
    USBSerial.println("GT911 not found; running in LCD-only mode for ESP32-S3-LCD-4.");
    gfx->setTextColor(BLACK);
    gfx->setTextSize(2);
    gfx->setCursor(48, 220);
    gfx->println("LCD-only mode");
    gfx->setCursor(48, 250);
    gfx->println("GT911 touch not found");
  }
}

void loop() {
  if (!gt911_available) {
    delay(100);
    return;
  }
  uint8_t touched = GT911.getPoint(x, y, GT911.getSupportTouchPoint());
  if (touched > 0) {
    USBSerial.print(millis());
    USBSerial.print("ms ");
    for (int i = 0; i < touched; ++i) {
      int16_t touchX = x[i];
      int16_t touchY = y[i];
      switch (gfx->getRotation()) {
        case 0:
          break;
        case 1:
          touchX = y[i];
          touchY = gfx->height() - x[i];
          break;
        case 2:
          touchX = gfx->width() - x[i];
          touchY = gfx->height() - y[i];
          break;
        case 3:
          touchX = gfx->width() - y[i];
          touchY = x[i];
          break;
      }
      USBSerial.print("X[");
      USBSerial.print(i);
      USBSerial.print("]:");
      USBSerial.print(x[i]);
      USBSerial.print(" ");
      USBSerial.print(" Y[");
      USBSerial.print(i);
      USBSerial.print("]:");
      USBSerial.print(y[i]);
      USBSerial.print(" ");

      gfx->fillCircle(touchX, touchY, 5, BLUE);
    }
    USBSerial.println();
  }
}
