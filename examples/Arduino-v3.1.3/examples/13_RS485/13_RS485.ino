#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

#define ECHO_TEST_TXD (44)
#define ECHO_TEST_RXD (43)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      UART_NUM_1
#define ECHO_UART_BAUD_RATE     115200
#define ECHO_TASK_STACK_SIZE    4096

static const char *TAG = "UART TEST";
#define BUF_SIZE (1024)

// 要发送的数据
const char *send_data = "Hello, UART!";

void echo_task(void *arg) {
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // 每100ms发送一次数据
        uart_write_bytes(ECHO_UART_PORT_NUM, send_data, strlen(send_data));
        ESP_LOGI(TAG, "Sent str: %s", send_data);

        int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        if (len) {
            data[len] = '\0';
            ESP_LOGI(TAG, "Recv str: %s", (char *) data);
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
}

void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}    