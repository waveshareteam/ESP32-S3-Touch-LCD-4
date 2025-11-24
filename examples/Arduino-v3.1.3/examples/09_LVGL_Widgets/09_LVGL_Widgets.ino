#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "lv_conf.h"
#include <demos/lv_demos.h>
#include "HWCDC.h"
#include "TouchDrvGT911.hpp"
#include <Wire.h>
#include <SPI.h>

TouchDrvGT911 GT911;
int16_t x[5], y[5];

float angleX = 1;
float angleY = 0;

bool rotation = false;

HWCDC USBSerial;
#define EXAMPLE_LVGL_TICK_PERIOD_MS 2

uint32_t screenWidth;
uint32_t screenHeight;

static lv_disp_draw_buf_t draw_buf;
// static lv_color_t buf[screenWidth * screenHeight / 10];

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

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf) {
  Serial.printf(buf);
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void example_increase_lvgl_tick(void *arg) {
  /* Tell LVGL how many milliseconds has elapsed */
  lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

static uint8_t count = 0;
void example_increase_reboot(void *arg) {
  count++;
  if (count == 30) {
    esp_restart();
  }
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
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
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touchX;
      data->point.y = touchY;

      USBSerial.print("Data x ");
      USBSerial.print(touchX);

      USBSerial.print("Data y ");
      USBSerial.println(touchY);

      // gfx->fillCircle(touchX, touchY, 5, BLUE);
    }
    USBSerial.println();
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void setup() {
  USBSerial.begin(115200); /* prepare for possible serial debug */

  Wire.begin(15, 7);

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
  GT911.setMaxTouchPoint(1);  // max is 5

  gfx->begin();

  screenWidth = gfx->width();
  screenHeight = gfx->height();

  lv_init();

  lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(screenWidth * screenHeight / 4 * sizeof(lv_color_t), MALLOC_CAP_DMA);

  lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(screenWidth * screenHeight / 4 * sizeof(lv_color_t), MALLOC_CAP_DMA);

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  USBSerial.println(LVGL_Arduino);
  USBSerial.println("I am LVGL_Arduino");



#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, screenWidth * screenHeight / 4);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.sw_rotate = 1;  // add for rotation
  // disp_drv.rotated = LV_DISP_ROT_90;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &example_increase_lvgl_tick,
    .name = "lvgl_tick"
  };

  const esp_timer_create_args_t reboot_timer_args = {
    .callback = &example_increase_reboot,
    .name = "reboot"
  };

  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);

  lv_demo_widgets();
  // lv_demo_benchmark();
  // lv_demo_keypad_encoder();
  // lv_demo_music();
  // lv_demo_stress();

  USBSerial.println("Setup done");
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}
