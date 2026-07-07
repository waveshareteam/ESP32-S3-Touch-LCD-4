#include <Arduino.h>
#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "HWCDC.h"
#include "WS_CH32_IO.h"

HWCDC USBSerial;

#define EXAMPLE_LVGL_TICK_PERIOD_MS 2

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[480 * 480 / 10];
static lv_obj_t *battery_label;
static uint32_t last_battery_ms;

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
void my_print(const char *buf)
{
    USBSerial.printf("%s", buf);
    USBSerial.flush();
}
#endif

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

    lv_disp_flush_ready(disp);
}

void example_increase_lvgl_tick(void *arg)
{
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

static void create_battery_ui()
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x101418), LV_PART_MAIN);

    battery_label = lv_label_create(lv_scr_act());
    lv_label_set_text(battery_label, "Battery\n--.-- V");
    lv_obj_set_style_text_color(battery_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_align(battery_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_40, LV_PART_MAIN);
    lv_obj_align(battery_label, LV_ALIGN_CENTER, 0, 0);
}

static void update_battery_label()
{
    float voltage = 0.0f;
    uint16_t raw = 0;
    char text[64];

    if (WS_CH32_IO::readBatteryVoltage(Wire, &voltage, &raw)) {
        snprintf(text, sizeof(text), "Battery\n%.2f V\nADC %u", voltage, raw);
        lv_label_set_text(battery_label, text);
        USBSerial.printf("Battery: %.3f V, raw ADC: %u\r\n", voltage, raw);
    } else {
        lv_label_set_text(battery_label, "Battery\nread failed");
        USBSerial.println("Battery ADC read failed");
    }
}

void setup()
{
    USBSerial.begin(115200);
    USBSerial.println("LVGL battery voltage example");

    if (!WS_CH32_IO::begin(Wire, WS_CH32_IO::DEFAULT_I2C_SDA, WS_CH32_IO::DEFAULT_I2C_SCL,
                           WS_CH32_IO::DEFAULT_I2C_FREQ, &USBSerial)) {
        USBSerial.println("CH32V003 init failed, continuing for display debug");
    }

    gfx->begin();
    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print);
#endif

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, 480 * 480 / 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 480;
    disp_drv.ver_res = 480;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };

    esp_timer_handle_t lvgl_tick_timer = NULL;
    esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);

    create_battery_ui();
    update_battery_label();
    last_battery_ms = millis();
}

void loop()
{
    lv_timer_handler();

    if (millis() - last_battery_ms >= 1000) {
        last_battery_ms = millis();
        update_battery_label();
    }

    delay(5);
}
