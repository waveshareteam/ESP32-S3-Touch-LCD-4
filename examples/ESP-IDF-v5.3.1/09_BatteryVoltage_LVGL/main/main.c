#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "bsp/esp-bsp.h"
#include "bsp/esp32_s3_touch_lcd_4.h"
#include "custom_io_expander_ch32v003.h"
#include "lvgl.h"

static const char *TAG = "battery_lvgl";

#define BATTERY_ADC_MAX         1023.0f
#define BATTERY_ADC_REF_VOLTAGE 3.3f
#define BATTERY_DIVIDER_RATIO   3.0f
#define BOARD_I2C_SDA           GPIO_NUM_15
#define BOARD_I2C_SCL           GPIO_NUM_7

#if LVGL_VERSION_MAJOR >= 9
#define LV_ACTIVE_SCREEN() lv_screen_active()
#else
#define LV_ACTIVE_SCREEN() lv_scr_act()
#endif

static lv_obj_t *battery_label;
static esp_io_expander_handle_t io_expander;

static void board_i2c_recover(void)
{
    gpio_config_t io_conf = {0};
    io_conf.pin_bit_mask = (1ULL << BOARD_I2C_SDA) | (1ULL << BOARD_I2C_SCL);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    esp_rom_delay_us(20);

    gpio_set_direction(BOARD_I2C_SCL, GPIO_MODE_OUTPUT_OD);
    gpio_set_pull_mode(BOARD_I2C_SCL, GPIO_PULLUP_ONLY);
    gpio_set_level(BOARD_I2C_SCL, 1);
    esp_rom_delay_us(10);

    for (int i = 0; i < 9 && gpio_get_level(BOARD_I2C_SDA) == 0; ++i) {
        gpio_set_level(BOARD_I2C_SCL, 0);
        esp_rom_delay_us(10);
        gpio_set_level(BOARD_I2C_SCL, 1);
        esp_rom_delay_us(10);
    }

    gpio_set_direction(BOARD_I2C_SDA, GPIO_MODE_OUTPUT_OD);
    gpio_set_pull_mode(BOARD_I2C_SDA, GPIO_PULLUP_ONLY);
    gpio_set_level(BOARD_I2C_SDA, 0);
    esp_rom_delay_us(10);
    gpio_set_level(BOARD_I2C_SCL, 1);
    esp_rom_delay_us(10);
    gpio_set_level(BOARD_I2C_SDA, 1);
    esp_rom_delay_us(10);

    gpio_set_direction(BOARD_I2C_SDA, GPIO_MODE_INPUT);
    gpio_set_direction(BOARD_I2C_SCL, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BOARD_I2C_SDA, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(BOARD_I2C_SCL, GPIO_PULLUP_ONLY);
    vTaskDelay(pdMS_TO_TICKS(20));
}

static float battery_voltage_from_raw(uint16_t raw)
{
    if (raw > 1023) {
        raw = 1023;
    }

    float adc_voltage = ((float)raw * BATTERY_ADC_REF_VOLTAGE) / BATTERY_ADC_MAX;
    return adc_voltage * BATTERY_DIVIDER_RATIO;
}

static esp_err_t read_battery_voltage(float *voltage, uint16_t *raw)
{
    ESP_RETURN_ON_FALSE(voltage != NULL, ESP_ERR_INVALID_ARG, TAG, "voltage is NULL");

    uint32_t total = 0;
    const int samples = 8;

    for (int i = 0; i < samples; ++i) {
        uint16_t sample = 0;
        ESP_RETURN_ON_ERROR(custom_io_expander_get_adc(io_expander, &sample), TAG, "Read CH32V003 ADC failed");
        total += sample;
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    uint16_t average = (uint16_t)((total + samples / 2) / samples);
    if (raw) {
        *raw = average;
    }
    *voltage = battery_voltage_from_raw(average);

    return ESP_OK;
}

static void battery_task(void *arg)
{
    while (1) {
        float voltage = 0.0f;
        uint16_t raw = 0;
        esp_err_t ret = read_battery_voltage(&voltage, &raw);

        if (bsp_display_lock(0)) {
            if (ret == ESP_OK) {
                lv_label_set_text_fmt(battery_label, "Battery\n%.2f V\nADC %u", voltage, raw);
            } else {
                lv_label_set_text_fmt(battery_label, "Battery\nread failed\n%s", esp_err_to_name(ret));
            }
            bsp_display_unlock();
        }

        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Battery: %.3f V, raw ADC: %u", voltage, raw);
        } else {
            ESP_LOGW(TAG, "Battery read failed: %s", esp_err_to_name(ret));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    board_i2c_recover();

    lv_display_t *disp = bsp_display_start();
    if (disp == NULL) {
        ESP_LOGE(TAG, "Display init failed");
        return;
    }

    io_expander = bsp_io_expander_init();
    if (io_expander == NULL) {
        ESP_LOGE(TAG, "CH32V003 IO expander init failed");
        return;
    }

    if (bsp_display_lock(0)) {
        lv_obj_t *screen = LV_ACTIVE_SCREEN();
        lv_obj_set_style_bg_color(screen, lv_color_hex(0x101418), LV_PART_MAIN);

        battery_label = lv_label_create(screen);
        lv_label_set_text(battery_label, "Battery\n--.-- V");
        lv_obj_set_style_text_color(battery_label, lv_color_hex(0xffffff), LV_PART_MAIN);
        lv_obj_set_style_text_align(battery_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_40, LV_PART_MAIN);
        lv_obj_align(battery_label, LV_ALIGN_CENTER, 0, 0);

        bsp_display_unlock();
    }

    xTaskCreate(battery_task, "battery_task", 4096, NULL, 5, NULL);
}
