#include <Arduino.h>
#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "lv_conf.h"
#include <Wire.h>
#include <SPI.h>
#include <FS.h>
#include <SD_MMC.h>
#include <stdio.h>  // Optional if not using printf

#include "HWCDC.h"
HWCDC USBSerial;

#define EXAMPLE_LVGL_TICK_PERIOD_MS 2
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[480 * 480 / 10];
lv_obj_t *label;  // Global label object

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

String listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  USBSerial.println("Listing directory: " + String(dirname));

  String dirContent = "Listing directory: " + String(dirname) + "\n";

  File root = fs.open(dirname);
  if (!root) {
    USBSerial.println("Failed to open directory");
    return "Failed to open directory\n";
  }
  if (!root.isDirectory()) {
    USBSerial.println("Not a directory");
    return "Not a directory\n";
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      String dirName = "  DIR : " + String(file.name()) + "\n";
      USBSerial.print(dirName);
      dirContent += dirName;
      if (levels) {
        dirContent += listDir(fs, file.path(), levels - 1);
      }
    } else {
      String fileInfo = "  FILE: " + String(file.name()) + "  SIZE: " + String(file.size()) + "\n";
      USBSerial.print(fileInfo);
      dirContent += fileInfo;
    }
    file = root.openNextFile();
  }
  return dirContent;
}


void setup() {
  USBSerial.begin(115200);

  Wire.begin(15, 7);
  delay(3000);

  gfx->begin();

  lv_init();

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

  SD_MMC.setPins(2, 1, 4);

  lv_obj_t *sd_info_label = lv_label_create(lv_scr_act());
  lv_label_set_long_mode(sd_info_label, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(sd_info_label, 300);
  lv_obj_align(sd_info_label, LV_ALIGN_CENTER, 0, 0);

  if (!SD_MMC.begin("/sdcard", true)) {
    USBSerial.println("Card Mount Failed");
    lv_label_set_text(sd_info_label, "Card Mount Failed");
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    USBSerial.println("No SD_MMC card attached");
    lv_label_set_text(sd_info_label, "No SD_MMC card attached");
  }

  USBSerial.print("SD_MMC Card Type: ");
  if (cardType == CARD_MMC) {
    USBSerial.println("MMC");
  } else if (cardType == CARD_SD) {
    USBSerial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    USBSerial.println("SDHC");
  } else {
    USBSerial.println("UNKNOWN");
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  USBSerial.println("SD_MMC Card Size: " + String(cardSize) + "MB");

  char sd_info[256];
  snprintf(sd_info, sizeof(sd_info), "SD_MMC Card Type: %s\nSD_MMC Card Size: %lluMB\n",
           cardType == CARD_MMC ? "MMC" : cardType == CARD_SD   ? "SDSC"
                                        : cardType == CARD_SDHC ? "SDHC"
                                                                : "UNKNOWN",
           cardSize);

  String dirList = listDir(SD_MMC, "/", 0);
  strncat(sd_info, dirList.c_str(), sizeof(sd_info) - strlen(sd_info) - 1);

  lv_label_set_text(sd_info_label, sd_info);
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}
