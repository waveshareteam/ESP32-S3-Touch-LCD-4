#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_io_expander.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "custom_io_expander_ch32v003.h"

static const char *TAG = "ch32_ioexpander";

#define BOARD_I2C_SDA GPIO_NUM_15
#define BOARD_I2C_SCL GPIO_NUM_7
#define BOARD_I2C_NUM I2C_NUM_0

#define CH32_I2C_ADDRESS CUSTOM_IO_EXPANDER_I2C_CH32V003_ADDRESS

#define CH32_TP_RST_MASK  IO_EXPANDER_PIN_NUM_1
#define CH32_LCD_RST_MASK IO_EXPANDER_PIN_NUM_3
#define CH32_SYS_EN_MASK  IO_EXPANDER_PIN_NUM_5
#define CH32_BEE_EN_MASK  IO_EXPANDER_PIN_NUM_6
#define CH32_RTC_INT_MASK IO_EXPANDER_PIN_NUM_7

#define CH32_OUTPUT_MASK (CH32_TP_RST_MASK | CH32_LCD_RST_MASK | CH32_SYS_EN_MASK | CH32_BEE_EN_MASK)

#define CH32_PWM_MAX              255
#define CH32_ADC_MAX              1023.0f
#define CH32_ADC_REF_VOLTAGE      3.3f
#define BATTERY_DIVIDER_RATIO     3.0f

static i2c_master_bus_handle_t s_i2c_bus;
static esp_io_expander_handle_t s_ch32;

static void board_i2c_recover(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BOARD_I2C_SDA) | (1ULL << BOARD_I2C_SCL),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
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

static esp_err_t board_i2c_init(void)
{
    if (s_i2c_bus) {
        return ESP_OK;
    }

    board_i2c_recover();

    const i2c_master_bus_config_t i2c_bus_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = BOARD_I2C_SDA,
        .scl_io_num = BOARD_I2C_SCL,
        .i2c_port = BOARD_I2C_NUM,
    };

    return i2c_new_master_bus(&i2c_bus_conf, &s_i2c_bus);
}

static esp_err_t ch32_init(void)
{
    ESP_RETURN_ON_ERROR(board_i2c_init(), TAG, "I2C bus init failed");

    esp_err_t ret = ESP_FAIL;
    for (int attempt = 0; attempt < 3; ++attempt) {
        ret = custom_io_expander_new_i2c_ch32v003(s_i2c_bus, CH32_I2C_ADDRESS, &s_ch32);
        if (ret == ESP_OK) {
            break;
        }
        ESP_LOGW(TAG, "CH32 create failed on attempt %d: %s", attempt + 1, esp_err_to_name(ret));
        vTaskDelay(pdMS_TO_TICKS(20 + attempt * 20));
    }
    ESP_RETURN_ON_ERROR(ret, TAG, "CH32V003 IO expander not found at 0x%02X", CH32_I2C_ADDRESS);

    ESP_RETURN_ON_ERROR(esp_io_expander_set_dir(s_ch32, CH32_OUTPUT_MASK, IO_EXPANDER_OUTPUT),
                        TAG, "Set CH32 output direction failed");
    ESP_RETURN_ON_ERROR(esp_io_expander_set_dir(s_ch32, CH32_RTC_INT_MASK, IO_EXPANDER_INPUT),
                        TAG, "Set CH32 RTC_INT direction failed");

    ESP_LOGI(TAG, "Hold LCD and touch in reset, disable SYS_EN/BEE_EN");
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(s_ch32, CH32_OUTPUT_MASK, 0),
                        TAG, "Drive CH32 outputs low failed");
    vTaskDelay(pdMS_TO_TICKS(200));

    ESP_LOGI(TAG, "Enable SYS_EN, release LCD_RST and TP_RST");
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(s_ch32, CH32_SYS_EN_MASK | CH32_LCD_RST_MASK | CH32_TP_RST_MASK, 1),
                        TAG, "Release CH32 outputs failed");
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(s_ch32, CH32_BEE_EN_MASK, 0),
                        TAG, "Disable buzzer failed");
    vTaskDelay(pdMS_TO_TICKS(200));

    return ESP_OK;
}

static float battery_voltage_from_raw(uint16_t raw)
{
    if (raw > 1023) {
        raw = 1023;
    }

    const float adc_voltage = ((float)raw * CH32_ADC_REF_VOLTAGE) / CH32_ADC_MAX;
    return adc_voltage * BATTERY_DIVIDER_RATIO;
}

static esp_err_t read_battery_voltage(float *voltage, uint16_t *raw)
{
    ESP_RETURN_ON_FALSE(voltage != NULL, ESP_ERR_INVALID_ARG, TAG, "voltage is NULL");

    uint32_t total = 0;
    const int samples = 8;

    for (int i = 0; i < samples; ++i) {
        uint16_t sample = 0;
        ESP_RETURN_ON_ERROR(custom_io_expander_get_adc(s_ch32, &sample), TAG, "Read CH32 ADC failed");
        total += sample;
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    const uint16_t average = (uint16_t)((total + samples / 2) / samples);
    if (raw) {
        *raw = average;
    }
    *voltage = battery_voltage_from_raw(average);

    return ESP_OK;
}

static esp_err_t ch32_print_status(void)
{
    uint32_t output_levels = 0;
    uint32_t rtc_int_level = 0;
    uint8_t rtc_int_reg = 0;
    uint16_t raw = 0;
    float voltage = 0.0f;

    ESP_RETURN_ON_ERROR(esp_io_expander_get_level(s_ch32, CH32_OUTPUT_MASK, &output_levels),
                        TAG, "Read CH32 output levels failed");
    ESP_RETURN_ON_ERROR(esp_io_expander_get_level(s_ch32, CH32_RTC_INT_MASK, &rtc_int_level),
                        TAG, "Read CH32 RTC_INT level failed");
    ESP_RETURN_ON_ERROR(custom_io_expander_get_int(s_ch32, &rtc_int_reg),
                        TAG, "Read CH32 RTC_INT register failed");
    ESP_RETURN_ON_ERROR(read_battery_voltage(&voltage, &raw),
                        TAG, "Read CH32 battery voltage failed");

    ESP_LOGI(TAG, "CH32 output mask=0x%02" PRIX32
                  " SYS_EN=%d LCD_RST=%d TP_RST=%d BEE_EN=%d RTC_INT pin=%d reg=%u",
             output_levels,
             (output_levels & CH32_SYS_EN_MASK) ? 1 : 0,
             (output_levels & CH32_LCD_RST_MASK) ? 1 : 0,
             (output_levels & CH32_TP_RST_MASK) ? 1 : 0,
             (output_levels & CH32_BEE_EN_MASK) ? 1 : 0,
             (rtc_int_level & CH32_RTC_INT_MASK) ? 1 : 0,
             rtc_int_reg);
    ESP_LOGI(TAG, "Battery ADC raw=%u, voltage=%.3f V", raw, voltage);

    esp_io_expander_print_state(s_ch32);

    return ESP_OK;
}

static esp_err_t ch32_set_backlight_percent(uint8_t percent)
{
    if (percent > 100) {
        percent = 100;
    }

    const uint8_t duty = (uint8_t)((percent * CH32_PWM_MAX + 50) / 100);
    ESP_LOGI(TAG, "Set LCD backlight to %u%% (PWM duty %u/255)", percent, duty);
    return custom_io_expander_set_pwm(s_ch32, duty);
}

static esp_err_t ch32_beep_once(uint32_t on_ms)
{
    ESP_LOGI(TAG, "Buzzer on for %" PRIu32 " ms", on_ms);
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(s_ch32, CH32_BEE_EN_MASK, 1), TAG, "Buzzer on failed");
    vTaskDelay(pdMS_TO_TICKS(on_ms));
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(s_ch32, CH32_BEE_EN_MASK, 0), TAG, "Buzzer off failed");
    return ESP_OK;
}

static esp_err_t ch32_reset_lcd_and_touch(void)
{
    ESP_LOGI(TAG, "Pulse LCD_RST and TP_RST low");
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(s_ch32, CH32_LCD_RST_MASK | CH32_TP_RST_MASK, 0),
                        TAG, "Assert reset failed");
    vTaskDelay(pdMS_TO_TICKS(120));
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(s_ch32, CH32_LCD_RST_MASK | CH32_TP_RST_MASK, 1),
                        TAG, "Release reset failed");
    vTaskDelay(pdMS_TO_TICKS(120));
    return ESP_OK;
}

void app_main(void)
{
    ESP_ERROR_CHECK(ch32_init());
    ESP_ERROR_CHECK(ch32_print_status());
    ESP_ERROR_CHECK(ch32_beep_once(80));
    ESP_ERROR_CHECK(ch32_reset_lcd_and_touch());

    const uint8_t brightness_steps[] = {10, 40, 80, 100, 60, 20, 100};
    for (size_t i = 0; i < sizeof(brightness_steps) / sizeof(brightness_steps[0]); ++i) {
        ESP_ERROR_CHECK(ch32_set_backlight_percent(brightness_steps[i]));
        vTaskDelay(pdMS_TO_TICKS(800));
    }

    while (1) {
        ESP_ERROR_CHECK(ch32_print_status());
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
