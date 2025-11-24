#include <Wire.h>
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "TouchDrvGT911.hpp"
#include <SPI.h>

#include "HWCDC.h"
HWCDC USBSerial;

TouchDrvGT911 GT911;
int16_t x[5], y[5];

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


void setup() {
  USBSerial.begin(115200);

  GT911.setPins(-1, 16);
  if (!GT911.begin(Wire, GT911_SLAVE_ADDRESS_L, 15, 7)) {
    while (1) {
      USBSerial.println("Failed to find GT911 - check your wiring!");
      delay(1000);
    }
  }
  USBSerial.println("Init GT911 Sensor success!");

  GT911.setHomeButtonCallback([](void *user_data) {
    USBSerial.println("Home button pressed!");
  },
                              NULL);
  GT911.setMaxTouchPoint(1); // max is 5

  gfx->begin();
  gfx->fillScreen(WHITE);
}

void loop() {

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
