/* SD card and FAT filesystem example.
   This example uses the SDMMC peripheral to communicate with the SD card.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_check.h"
#include "esp_io_expander.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "custom_io_expander_ch32v003.h"


#define BOARD_I2C_SDA               GPIO_NUM_15
#define BOARD_I2C_SCL               GPIO_NUM_7
#define BOARD_I2C_NUM               I2C_NUM_0

#define BOARD_SD_D0                 GPIO_NUM_4
#define BOARD_SD_CMD                GPIO_NUM_1
#define BOARD_SD_CLK                GPIO_NUM_2

#define CH32_LCD_TOUCH_RST_MASK     (IO_EXPANDER_PIN_NUM_1 | IO_EXPANDER_PIN_NUM_3)
#define CH32_SYS_EN_MASK            IO_EXPANDER_PIN_NUM_5
#define CH32_BEE_EN_MASK            IO_EXPANDER_PIN_NUM_6
#define CH32_OUTPUT_MASK            (CH32_SYS_EN_MASK | CH32_BEE_EN_MASK | CH32_LCD_TOUCH_RST_MASK)
#define CH32_RTC_INT_MASK           IO_EXPANDER_PIN_NUM_7


#define EXAMPLE_MAX_CHAR_SIZE    64

static const char *TAG = "example";

#define MOUNT_POINT "/sdcard"

static esp_err_t s_example_write_file(const char *path, char *data)
{
    printf("Opening file %s\r\n", path);
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        printf("Failed to open file for writing\r\n");
        return ESP_FAIL;
    }
    fprintf(f, data);
    fclose(f);
    printf("File written\r\n");

    return ESP_OK;
}

static esp_err_t s_example_read_file(const char *path)
{
    printf("Reading file %s\r\n", path);
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        printf("Failed to open file for reading\r\n");
        return ESP_FAIL;
    }
    char line[EXAMPLE_MAX_CHAR_SIZE];
    fgets(line, sizeof(line), f);
    fclose(f);

    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    printf("Read from file: '%s'\r\n", line);

    return ESP_OK;
}

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

static esp_err_t board_io_expander_init(void)
{
    board_i2c_recover();

    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t i2c_bus_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = BOARD_I2C_SDA,
        .scl_io_num = BOARD_I2C_SCL,
        .i2c_port = BOARD_I2C_NUM,
    };
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&i2c_bus_conf, &i2c_bus), TAG, "I2C bus init failed");

    esp_io_expander_handle_t io_expander = NULL;
    esp_err_t ret = ESP_FAIL;
    for (int attempt = 0; attempt < 3; ++attempt) {
        ret = custom_io_expander_new_i2c_ch32v003(i2c_bus, CUSTOM_IO_EXPANDER_I2C_CH32V003_ADDRESS, &io_expander);
        if (ret == ESP_OK) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(20 + attempt * 20));
    }
    ESP_RETURN_ON_ERROR(ret, TAG, "CH32V003 init failed");

    ESP_RETURN_ON_ERROR(esp_io_expander_set_dir(io_expander, CH32_OUTPUT_MASK, IO_EXPANDER_OUTPUT),
                        TAG, "Set CH32V003 output direction failed");
    ESP_RETURN_ON_ERROR(esp_io_expander_set_dir(io_expander, CH32_RTC_INT_MASK, IO_EXPANDER_INPUT),
                        TAG, "Set CH32V003 input direction failed");
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(io_expander, CH32_OUTPUT_MASK, 0),
                        TAG, "Drive CH32V003 reset levels failed");
    vTaskDelay(pdMS_TO_TICKS(200));
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(io_expander, CH32_SYS_EN_MASK | CH32_LCD_TOUCH_RST_MASK, 1),
                        TAG, "Release CH32V003 reset levels failed");
    vTaskDelay(pdMS_TO_TICKS(200));

    return ESP_OK;
}

void app_main(void)
{
    esp_err_t ret;

    ESP_ERROR_CHECK(board_io_expander_init());

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    printf("Initializing SD card\r\n");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
    // Please check its source code and implement error recovery when developing
    // production applications.
    printf("Using SDMMC peripheral\r\n");

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = {
        .clk = BOARD_SD_CLK,
        .cmd = BOARD_SD_CMD,
        .d0 = BOARD_SD_D0,
        .d1 = GPIO_NUM_NC,
        .d2 = GPIO_NUM_NC,
        .d3 = GPIO_NUM_NC,
        .d4 = GPIO_NUM_NC,
        .d5 = GPIO_NUM_NC,
        .d6 = GPIO_NUM_NC,
        .d7 = GPIO_NUM_NC,
        .cd = SDMMC_SLOT_NO_CD,
        .wp = SDMMC_SLOT_NO_WP,
        .width = 1,
        .flags = 0,
    };

    printf("Mounting filesystem\r\n");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            printf("Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.\r\n");
        } else {
            printf("Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.\r\n", esp_err_to_name(ret));
        }
        return;
    }
    printf("Filesystem mounted\r\n");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    // Use POSIX and C standard library functions to work with files.

    // First create a file.
    const char *file_hello = MOUNT_POINT"/hello.txt";
    char data[EXAMPLE_MAX_CHAR_SIZE];
    snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Hello", card->cid.name);
    ret = s_example_write_file(file_hello, data);
    if (ret != ESP_OK) {
        return;
    }

    const char *file_foo = MOUNT_POINT"/foo.txt";

    // Check if destination file exists before renaming
    struct stat st;
    if (stat(file_foo, &st) == 0) {
        // Delete it if it exists
        unlink(file_foo);
    }

    // Rename original file
    printf("Renaming file %s to %sv", file_hello, file_foo);
    if (rename(file_hello, file_foo) != 0) {
        printf("Rename failed\r\n");
        return;
    }

    ret = s_example_read_file(file_foo);
    if (ret != ESP_OK) {
        return;
    }

    // Format FATFS
    ret = esp_vfs_fat_sdcard_format(mount_point, card);
    if (ret != ESP_OK) {
        printf("Failed to format FATFS (%s)", esp_err_to_name(ret));
        return;
    }

    if (stat(file_foo, &st) == 0) {
        printf("file still exists\r\n");
        return;
    } else {
        printf("file doesnt exist, format done\r\n");
    }

    const char *file_nihao = MOUNT_POINT"/nihao.txt";
    memset(data, 0, EXAMPLE_MAX_CHAR_SIZE);
    snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Nihao", card->cid.name);
    ret = s_example_write_file(file_nihao, data);
    if (ret != ESP_OK) {
        return;
    }

    //Open file for reading
    ret = s_example_read_file(file_nihao);
    if (ret != ESP_OK) {
        return;
    }

    // All done, unmount partition and disable the SDMMC peripheral
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    printf("Card unmounted\r\n");

    //deinitialize the bus after all devices are removed
    sdmmc_host_deinit();
}
