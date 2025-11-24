#include <Arduino_GFX_Library.h>
#include "HWCDC.h"

HWCDC USBSerial;
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

void setup(void)
{
  USBSerial.begin(115200);
  USBSerial.println("Arduino_GFX AsciiTable example");

  int numCols = 480 / 10;
  int numRows = 480 / 10;

  // Init Display
  if (!gfx->begin()) {
    USBSerial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

  gfx->setTextColor(GREEN);
  for (int x = 0; x < numRows; x++) {
    gfx->setCursor(12 + x * 10, 2);
    gfx->print(x, 60);
  }
  gfx->setTextColor(BLUE);
  for (int y = 0; y < numCols; y++) {
    gfx->setCursor(2, 12 + y * 10);
    gfx->print(y, 60);
  }

  char c = 0;
  for (int y = 0; y < numRows; y++) {
    for (int x = 0; x < numCols; x++) {
      gfx->drawChar(12 + x * 10, 12 + y * 10, c++, WHITE, BLACK);
    }
  }

  delay(5000);  // 5 seconds
}

void loop()
{
}