#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_memory_utils.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "lv_demos.h"

#define BOARD_I2C_SDA GPIO_NUM_15
#define BOARD_I2C_SCL GPIO_NUM_7

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

void app_main(void)
{

    board_i2c_recover();

    bsp_display_start();

    bsp_display_lock(0);

    // lv_demo_music();
    lv_demo_benchmark();
    // lv_demo_widgets();

    bsp_display_unlock();
}
