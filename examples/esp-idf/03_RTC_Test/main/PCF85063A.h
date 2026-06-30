/*
 * pcf85063a.h
 *
 *  Created on: 13 Aug. 2021
 *      Author: Zildjhan
 */

#ifndef INC_PCF85063A_H_
#define INC_PCF85063A_H_

#include <stdio.h>
#include "esp_rom_sys.h"
#include "esp_log.h"
#include "argtable3/argtable3.h"
#include "driver/i2c.h"
#include "esp_console.h"
#include "driver/gpio.h"
#include "freertos/task.h"

#include <stdint.h>
#include <stdlib.h> //itoa()

/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t


/**
 * I2C config
 **/
#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define WRITE_BIT I2C_MASTER_WRITE  /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ    /*!< I2C master read */
#define ACK_CHECK_EN 0x1            /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0           /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                 /*!< I2C ack value */
#define NACK_VAL 0x1                /*!< I2C nack value */

//PCF85063A_ADDRESS
#define PCF85063A_ADDRESS   (0x51)
//
#define YEAR_OFFSET			(1970)
// registar overview - crtl & status reg
#define RTC_CTRL_1_ADDR     (0x00)
#define RTC_CTRL_2_ADDR     (0x01)
#define RTC_OFFSET_ADDR     (0x02)
#define RTC_RAM_by_ADDR     (0x03)
// registar overview - time & data reg
#define RTC_SECOND_ADDR		(0x04)
#define RTC_MINUTE_ADDR		(0x05)
#define RTC_HOUR_ADDR		(0x06)
#define RTC_DAY_ADDR		(0x07)
#define RTC_WDAY_ADDR		(0x08)
#define RTC_MONTH_ADDR		(0x09)
#define RTC_YEAR_ADDR		(0x0A)	// years 0-99; calculate real year = 1970 + RCC reg year
// registar overview - alarm reg
#define RTC_SECOND_ALARM	(0x0B)
#define RTC_MINUTE_ALARM	(0x0C)
#define RTC_HOUR_ALARM		(0x0D)
#define RTC_DAY_ALARM		(0x0E)
#define RTC_WDAY_ALARM		(0x0F)
// registar overview - timer reg
#define RTC_TIMER_VAL 	    (0x10)
#define RTC_TIMER_MODE	    (0x11)

//RTC_CTRL_1 registar
#define RTC_CTRL_1_EXT_TEST (0x80)
#define RTC_CTRL_1_STOP     (0x20) //0-RTC clock runs 1- RTC clock is stopped
#define RTC_CTRL_1_SR       (0X10) //0-no software reset 1-initiate software rese
#define RTC_CTRL_1_CIE      (0X04) //0-no correction interrupt generated 1-interrupt pulses are generated at every correction cycle
#define RTC_CTRL_1_12_24    (0X02) //0-24H 1-12H
#define RTC_CTRL_1_CAP_SEL  (0X01) //0-7PF 1-12.5PF

//RTC_CTRL_2 registar
#define RTC_CTRL_2_AIE      (0X80) //alarm interrupt 0-disalbe 1-enable
#define RTC_CTRL_2_AF       (0X40) //alarm flag  0-inactive/cleared 1-active/unchanged
#define RTC_CTRL_2_MI       (0X20) //minute interrupt 0-disalbe 1-enable
#define RTC_CTRL_2_HMI      (0X10) //half minute interrupt 
#define RTC_CTRL_2_TF       (0X08)

//
#define RTC_OFFSET_MODE     (0X80)

//
#define RTC_TIMER_MODE_TE   (0X04) //timer enable 0-disalbe 1-enable
#define RTC_TIMER_MODE_TIE  (0X02) //timer interrupt enable 0-disalbe 1-enable
#define RTC_TIMER_MODE_TI_TP    (0X01) //timer interrupt mode 0-interrupt follows timer flag 1-interrupt generates a pulse

// format 
#define RTC_ALARM 			(0x80)	// set AEN_x registers
#define RTC_CTRL_1_DEFAULT	(0x00)
#define RTC_CTRL_2_DEFAULT	(0x00)

#define RTC_TIMER_FLAG		(0x08)

typedef struct {
    UWORD year;
    UBYTE month;
    UBYTE day;
    UBYTE dotw;
    UBYTE hour;
    UBYTE min;
    UBYTE sec;
}datetime_t;

void DEV_GPIO_INT(int32_t Pin, gpio_isr_t isr_handler);

void PCF85063A_Init(void);
void PCF85063A_Reset(void);

void PCF85063A_Set_Time(datetime_t time);
void PCF85063A_Set_Date(datetime_t date);
void PCF85063A_Set_All(datetime_t time);

void PCF85063A_Read_now(datetime_t *time);


void PCF85063A_Enable_Alarm(void);
uint8_t PCF85063A_Get_Alarm_Flag();
void PCF85063A_Set_Alarm(datetime_t time);
void PCF85063A_Read_Alarm(datetime_t *time);

void datetime_to_str(char *datetime_str,datetime_t time);


#endif /* INC_PCF85063A_H_ */

// weekday format
// 0 - sunday
// 1 - monday
// 2 - tuesday
// 3 - wednesday
// 4 - thursday
// 5 - friday
// 6 - saturday