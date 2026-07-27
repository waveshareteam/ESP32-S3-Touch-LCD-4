/*****************************************************************************
* | File      	:   PCF85063A.c
* | Author      :   Waveshare team
* | Function    :   PCF85063A driver
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2024-02-02
* | Info        :   Basic version
*
******************************************************************************/
#include "PCF85063A.h"

static uint8_t decToBcd(int val);
static int bcdToDec(uint8_t val);

static i2c_master_bus_handle_t s_i2c_bus;
static i2c_master_dev_handle_t s_rtc_device;

const unsigned char MonthStr[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov","Dec"};

/**
 * I2C
 **/
esp_err_t i2c_master_init(void)
{
    if (s_rtc_device != NULL) {
        return ESP_OK;
    }

    const i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_MASTER_NUM,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = 1,
    };
    esp_err_t ret = i2c_new_master_bus(&bus_config, &s_i2c_bus);
    if (ret != ESP_OK) {
        return ret;
    }

    const i2c_device_config_t device_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PCF85063A_ADDRESS,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    ret = i2c_master_bus_add_device(s_i2c_bus, &device_config, &s_rtc_device);
    if (ret != ESP_OK) {
        i2c_del_master_bus(s_i2c_bus);
        s_i2c_bus = NULL;
    }
    return ret;
}

i2c_master_bus_handle_t PCF85063A_Get_I2C_Bus(void)
{
    return s_i2c_bus;
}

esp_err_t  DEV_I2C_Write_Byte(uint8_t addr, uint8_t reg, uint8_t Value)
{
    if (addr != PCF85063A_ADDRESS || s_rtc_device == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    uint8_t write_buf[2] = {reg, Value};
    return i2c_master_transmit(s_rtc_device, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS);
}

esp_err_t  DEV_I2C_Write_nByte(uint8_t addr, uint8_t *pData, uint32_t Len)
{
    if (addr != PCF85063A_ADDRESS || s_rtc_device == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return i2c_master_transmit(s_rtc_device, pData, Len, I2C_MASTER_TIMEOUT_MS);
}

esp_err_t DEV_I2C_Read_Byte(uint8_t addr, uint8_t reg, uint8_t *data)
{
    if (addr != PCF85063A_ADDRESS || s_rtc_device == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return i2c_master_transmit_receive(s_rtc_device, &reg, 1, data, 1, I2C_MASTER_TIMEOUT_MS);
}

esp_err_t DEV_I2C_Read_nByte(uint8_t addr, uint8_t reg, uint8_t *pData, uint32_t Len)
{
    if (addr != PCF85063A_ADDRESS || s_rtc_device == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return i2c_master_transmit_receive(s_rtc_device, &reg, 1, pData, Len, I2C_MASTER_TIMEOUT_MS);
}

/******************************************************************************
function:	PCF85063A initialized
parameter:
            
Info:Initiate Normal Mode, RTC Run, NO reset, No correction , 24hr format, Internal load capacitane 12.5pf
******************************************************************************/
void PCF85063A_Init()
{
	uint8_t Value = RTC_CTRL_1_DEFAULT|RTC_CTRL_1_CAP_SEL;
	ESP_ERROR_CHECK(i2c_master_init());
	ESP_ERROR_CHECK(DEV_I2C_Write_Byte(PCF85063A_ADDRESS, RTC_CTRL_1_ADDR, Value));
	// PCF85063A_Enable_Alarm();
	// uint8_t Value = 0;
	// ESP_ERROR_CHECK(DEV_I2C_Read_Byte(PCF85063A_ADDRESS,RTC_CTRL_1_ADDR,&Value));
	// printf("Value = 0x%x",Value);
}

/******************************************************************************
function:	软件复位PCF85063A
parameter:
Info:		
******************************************************************************/
void PCF85063A_Reset()
{
	uint8_t Value = RTC_CTRL_1_DEFAULT|RTC_CTRL_1_CAP_SEL|RTC_CTRL_1_SR;
	ESP_ERROR_CHECK(DEV_I2C_Write_Byte(PCF85063A_ADDRESS, RTC_CTRL_1_ADDR, Value));
}

/******************************************************************************
function:	设置RTC时间
parameter:
			time:时间结构体
Info:		
******************************************************************************/
void PCF85063A_Set_Time(datetime_t time)
{

	uint8_t buf[4] = {RTC_SECOND_ADDR,
					  decToBcd(time.sec),
					  decToBcd(time.min),
					  decToBcd(time.hour)};
	ESP_ERROR_CHECK(DEV_I2C_Write_nByte(PCF85063A_ADDRESS, buf, 4));
}

/******************************************************************************
function:	设置RTC日期
parameter:
			date:日期结构体
Info:		
******************************************************************************/
void PCF85063A_Set_Date(datetime_t date)
{
	uint8_t buf[5] = {RTC_DAY_ADDR,
					  decToBcd(date.day),
					  decToBcd(date.dotw),
					  decToBcd(date.month),
					  decToBcd(date.year - YEAR_OFFSET)};
	ESP_ERROR_CHECK(DEV_I2C_Write_nByte(PCF85063A_ADDRESS, buf, 5));
}

/******************************************************************************
function:	设置RTC时间跟日期
parameter:
			time:时间跟日期结构体
Info:		
******************************************************************************/
void PCF85063A_Set_All(datetime_t time)
{
	uint8_t buf[8] = {RTC_SECOND_ADDR,
					  decToBcd(time.sec),
					  decToBcd(time.min),
					  decToBcd(time.hour),
					  decToBcd(time.day),
					  decToBcd(time.dotw),
					  decToBcd(time.month),
					  decToBcd(time.year - YEAR_OFFSET)};
	ESP_ERROR_CHECK(DEV_I2C_Write_nByte(PCF85063A_ADDRESS, buf, 8));
}

/******************************************************************************
function:	读取当前RTC时间跟日期
parameter:
			time:时间跟日期结构体
Info:		
******************************************************************************/
void PCF85063A_Read_now(datetime_t *time)
{
	uint8_t bufss[7] = {0};
	ESP_ERROR_CHECK(DEV_I2C_Read_nByte(PCF85063A_ADDRESS, RTC_SECOND_ADDR, bufss, 7));
	time->sec = bcdToDec(bufss[0] & 0x7F);
	time->min = bcdToDec(bufss[1] & 0x7F);
	time->hour = bcdToDec(bufss[2] & 0x3F);
	time->day = bcdToDec(bufss[3] & 0x3F);
	time->dotw = bcdToDec(bufss[4] & 0x07);
	time->month = bcdToDec(bufss[5] & 0x1F);
	time->year = bcdToDec(bufss[6])+YEAR_OFFSET;
}

/******************************************************************************
function:	Enable Alarm and Clear Alarm flag
parameter:			
Info:		默认启动报警中断
******************************************************************************/
void PCF85063A_Enable_Alarm()
{
	uint8_t Value = RTC_CTRL_2_DEFAULT | RTC_CTRL_2_AIE;
	Value &= ~RTC_CTRL_2_AF;
	ESP_ERROR_CHECK(DEV_I2C_Write_Byte(PCF85063A_ADDRESS,RTC_CTRL_2_ADDR,Value));
}

/******************************************************************************
function:	Get Alarm flay
parameter:			
Info:		
******************************************************************************/
uint8_t PCF85063A_Get_Alarm_Flag()
{
	uint8_t Value = 0;
	ESP_ERROR_CHECK(DEV_I2C_Read_Byte(PCF85063A_ADDRESS,RTC_CTRL_2_ADDR,&Value));
	//printf("Value = 0x%x",Value);
	Value &= RTC_CTRL_2_AF | RTC_CTRL_2_AIE;
	return Value;
}

/******************************************************************************
function:	设置报警时间
parameter:			
Info:		
******************************************************************************/
void PCF85063A_Set_Alarm(datetime_t time)
{

	uint8_t buf[6] ={
		RTC_SECOND_ALARM,
		decToBcd(time.sec)&(~RTC_ALARM),
		decToBcd(time.min)&(~RTC_ALARM),
		decToBcd(time.hour)&(~RTC_ALARM),
		//decToBcd(time.day)&(~RTC_ALARM),
		//decToBcd(time.dotw)&(~RTC_ALARM)
		RTC_ALARM, 	//disalbe day
		RTC_ALARM	//disalbe weekday
	};
	ESP_ERROR_CHECK(DEV_I2C_Write_nByte(PCF85063A_ADDRESS, buf, 6));
}

/******************************************************************************
function:	读取设置的报警时间
parameter:			
Info:		
******************************************************************************/
void PCF85063A_Read_Alarm(datetime_t *time)
{
	uint8_t bufss[5] = {0};
	ESP_ERROR_CHECK(DEV_I2C_Read_nByte(PCF85063A_ADDRESS, RTC_SECOND_ALARM, bufss, sizeof(bufss)));
	time->sec = bcdToDec(bufss[0] & 0x7F);
	time->min = bcdToDec(bufss[1] & 0x7F);
	time->hour = bcdToDec(bufss[2] & 0x3F);
	time->day = bcdToDec(bufss[3] & 0x3F);
	time->dotw = bcdToDec(bufss[4] & 0x07);
}


/******************************************************************************
function:	Convert normal decimal numbers to binary coded decimal
parameter:			
Info:		
******************************************************************************/
static uint8_t decToBcd(int val)
{
	return (uint8_t)((val / 10 * 16) + (val % 10));
}

/******************************************************************************
function:	Convert binary coded decimal to normal decimal numbers
parameter:			
Info:		
******************************************************************************/
static int bcdToDec(uint8_t val)
{
	return (int)((val / 16 * 10) + (val % 16));
}

/******************************************************************************
function:	将时间转换成字符串
parameter:	
			datetime_str:存储转换后的字符串数据
			time:时间跟日期结构体
Info:		
******************************************************************************/
void datetime_to_str(char *datetime_str,datetime_t time)
{
	sprintf(datetime_str, " %d.%d.%d  %d %d:%d:%d ", time.year, time.month, 
			time.day, time.dotw, time.hour, time.min, time.sec);
} 