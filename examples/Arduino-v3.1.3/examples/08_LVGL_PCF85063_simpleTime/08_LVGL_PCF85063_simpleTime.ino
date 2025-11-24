#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "lv_conf.h"
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include "SensorPCF85063.hpp"
#include "HWCDC.h"

HWCDC USBSerial;
#define EXAMPLE_LVGL_TICK_PERIOD_MS 2

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[480 * 480 / 10];

lv_obj_t *label;  // Global label object
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


#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf) {
  USBSerial.printf(buf);
  USBSerial.flush();
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

void setup() {
  USBSerial.begin(115200); /* prepare for possible serial debug */
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

  gfx->begin();

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  USBSerial.println(LVGL_Arduino);
  USBSerial.println("I am LVGL_Arduino");

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, 480 * 480 / 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = 480;
  disp_drv.ver_res = 480;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);

  // lv_obj_t *label = lv_label_create(lv_scr_act());
  // lv_label_set_text(label, "Hello Ardino and LVGL!");
  // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

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

  label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Initializing...");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);

  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    RTC_DateTime datetime = rtc.getDateTime();
    USBSerial.printf(" Year :");
    USBSerial.print(datetime.year);
    USBSerial.printf(" Month:");
    USBSerial.print(datetime.month);
    USBSerial.printf(" Day :");
    USBSerial.print(datetime.day);
    USBSerial.printf(" Hour:");
    USBSerial.print(datetime.hour);
    USBSerial.printf(" Minute:");
    USBSerial.print(datetime.minute);
    USBSerial.printf(" Sec :");
    USBSerial.println(datetime.second);

    char buf[32];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d\n%02d-%02d-%04d",
             datetime.hour, datetime.minute, datetime.second,
             datetime.day, datetime.month, datetime.year);

    // Update label with current time
    lv_label_set_text(label, buf);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_40, LV_PART_MAIN);
  }
  delay(20);
}
