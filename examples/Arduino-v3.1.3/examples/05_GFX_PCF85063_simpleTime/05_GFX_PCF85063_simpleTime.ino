#include <Arduino_GFX_Library.h>
#include "SensorPCF85063.hpp"
#include <Wire.h>

#include "HWCDC.h"
HWCDC USBSerial;

SensorPCF85063 rtc;
uint32_t lastMillis;

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

void setup(void) {
  USBSerial.begin(115200);

  if (!rtc.begin(Wire, PCF85063_SLAVE_ADDRESS, 15, 7)) {
    USBSerial.println("Failed to find PCF8563 - check your wiring!");
    while (1) {
      delay(1000);
    }
  }

  uint16_t year = 2025;
  uint8_t month = 2;
  uint8_t day = 18;
  uint8_t hour = 17;
  uint8_t minute = 15;
  uint8_t second = 30;

  rtc.setDateTime(year, month, day, hour, minute, second);

  if (!gfx->begin()) {
    USBSerial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(RGB565_WHITE);
}

void loop() {
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();

    RTC_DateTime datetime = rtc.getDateTime();

    char timeStr[32];
    snprintf(timeStr, sizeof(timeStr), "%04d-%02d-%02d %02d:%02d:%02d",
             datetime.year, datetime.month, datetime.day,
             datetime.hour, datetime.minute, datetime.second);

    gfx->fillScreen(RGB565_WHITE);

    gfx->setTextColor(RGB565_BLACK);
    gfx->setTextSize(4);

    // int16_t x = (gfx->width() - gfx->getTextWidth(timeStr)) / 2;
    // int16_t y = (gfx->height() - gfx->getTextHeight(timeStr)) / 2;

    gfx->setCursor(10, 200);
    gfx->print(timeStr);
  }
}
