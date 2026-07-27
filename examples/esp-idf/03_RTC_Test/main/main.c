/* RTC - Simple example

   Initializes the PCF85063A over the board's shared I2C bus, configures an
   alarm, and observes the alarm path through the CH32V003 helper controller.
*/
#include <stdint.h>

#include "custom_io_expander_ch32v003.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_io_expander.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pcf85063a.h"
#include "sdkconfig.h"

static const char *TAG = "RTC";

#define RTC_I2C_PORT I2C_NUM_0
#define CH32_RTC_INT_MASK IO_EXPANDER_PIN_NUM_7

static i2c_master_bus_handle_t s_i2c_bus;
static pcf85063a_dev_t s_rtc;
static esp_io_expander_handle_t s_ch32;

static const pcf85063a_datetime_t s_set_time = {
    .year = 2024,
    .month = 2,
    .day = 2,
    .dotw = 5,
    .hour = 9,
    .min = 0,
    .sec = 0,
};

static const pcf85063a_datetime_t s_set_alarm_time = {
    .year = 2024,
    .month = 2,
    .day = 2,
    .dotw = 5,
    .hour = 9,
    .min = 0,
    .sec = 2,
};

static char s_datetime_str[256];

static void rtc_i2c_init(void)
{
    const i2c_master_bus_config_t bus_config = {
        .i2c_port = RTC_I2C_PORT,
        .sda_io_num = CONFIG_I2C_MASTER_SDA,
        .scl_io_num = CONFIG_I2C_MASTER_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = 1,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &s_i2c_bus));
}

static void rtc_interrupt_path_init(void)
{
    ESP_ERROR_CHECK(custom_io_expander_new_i2c_ch32v003(
        s_i2c_bus, CUSTOM_IO_EXPANDER_I2C_CH32V003_ADDRESS, &s_ch32));
    ESP_ERROR_CHECK(esp_io_expander_set_dir(s_ch32, CH32_RTC_INT_MASK, IO_EXPANDER_INPUT));
}

void app_main(void)
{
    pcf85063a_datetime_t now_time;

    rtc_i2c_init();
    ESP_ERROR_CHECK(pcf85063a_init(&s_rtc, s_i2c_bus, PCF85063A_ADDRESS));

    // RTC_INT is routed through CH32V003 EXIO7, not directly to an ESP32-S3 GPIO.
    rtc_interrupt_path_init();

    ESP_ERROR_CHECK(pcf85063a_set_time_date(&s_rtc, s_set_time));
    ESP_ERROR_CHECK(pcf85063a_set_alarm(&s_rtc, s_set_alarm_time));
    ESP_ERROR_CHECK(pcf85063a_enable_alarm(&s_rtc));

    while (1) {
        ESP_ERROR_CHECK(pcf85063a_get_time_date(&s_rtc, &now_time));
        pcf85063a_datetime_to_str(s_datetime_str, sizeof(s_datetime_str), now_time);
        ESP_LOGI(TAG, "Now_time is %s", s_datetime_str);

        uint8_t ch32_rtc_int_reg = 0;
        uint8_t rtc_status = 0;
        ESP_ERROR_CHECK(custom_io_expander_get_int(s_ch32, &ch32_rtc_int_reg));
        ESP_ERROR_CHECK(pcf85063a_get_alarm_flag(&s_rtc, &rtc_status));
        if ((rtc_status & PCF85063A_RTC_CTRL_2_AF) != 0) {
            // Comment this out when the alarm should run only once.
            ESP_ERROR_CHECK(pcf85063a_enable_alarm(&s_rtc));
            ESP_LOGI(TAG, "The alarm clock goes off (CH32 RTC_INT register=%u).", ch32_rtc_int_reg);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
