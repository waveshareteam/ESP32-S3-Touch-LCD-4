/*  RTC - Simple example

    RTC simple example, how to initialize I2C, configure PCF85063A
    As well as reading and writing to the register of the sensor connected through I2C and alarm interrupt.
*/
#include "PCF85063A.h"
#include "custom_io_expander_ch32v003.h"
#include "esp_io_expander.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "RTC";

#define CH32_RTC_INT_MASK IO_EXPANDER_PIN_NUM_7

static esp_io_expander_handle_t s_ch32;

static datetime_t Set_Time = {
    .year = 2024,
    .month = 02,
    .day = 02,
    .dotw = 5,
    .hour = 9,
    .min = 0,
    .sec = 0};

static datetime_t Set_Alarm_Time = {
    .year = 2024,
    .month = 02,
    .day = 02,
    .dotw = 5,
    .hour = 9,
    .min = 0,
    .sec = 2};

char datetime_str[256];

static void rtc_interrupt_path_init(void)
{
    ESP_ERROR_CHECK(custom_io_expander_new_i2c_ch32v003(
        PCF85063A_Get_I2C_Bus(), CUSTOM_IO_EXPANDER_I2C_CH32V003_ADDRESS, &s_ch32));
    ESP_ERROR_CHECK(esp_io_expander_set_dir(s_ch32, CH32_RTC_INT_MASK, IO_EXPANDER_INPUT));
}

void app_main(void)
{
    datetime_t Now_time;
    //Initialize PCF85063A 初始化PCF85063A
    PCF85063A_Init();
    // RTC_INT is routed through CH32V003 EXIO7, not directly to an ESP32-S3 GPIO.
    rtc_interrupt_path_init();
    //set time 设置时间
    PCF85063A_Set_All(Set_Time);
    //set alarm 设置闹钟
    PCF85063A_Set_Alarm(Set_Alarm_Time);
    //Start alarm interrupt 启动闹钟中断
    PCF85063A_Enable_Alarm();

    while (1)
    {
        //reading access time 读取时间
        PCF85063A_Read_now(&Now_time);
        //Convert to string 转换成字符串
        datetime_to_str(datetime_str,Now_time);
        ESP_LOGI(TAG,"Now_time is %s ",datetime_str); 
        
        uint8_t ch32_rtc_int_reg = 0;
        ESP_ERROR_CHECK(custom_io_expander_get_int(s_ch32, &ch32_rtc_int_reg));
        const uint8_t rtc_status = PCF85063A_Get_Alarm_Flag();
        if ((rtc_status & RTC_CTRL_2_AF) != 0)
        {
            //Start the alarm again.Comment out the function if it only needs to run once 再次启动闹钟，如果只需要运行一次，需要注释这个函数
            PCF85063A_Enable_Alarm();
            ESP_LOGI(TAG,"The alarm clock goes off (CH32 RTC_INT register=%u).", ch32_rtc_int_reg);
        }
        //Delay 1 second 延时1秒
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
