/**************************************************************
   I2C, from BetaWatch library, is available for ESP32 platform to manage
   our connected watch (adapted from ESPRESSIF example).

   Licensed under University of Poitiers Connected Objects Master's degree by BetaWatch group.
   Author: Henintsoa Andrianarivony.
 **************************************************************/

#ifndef __INC_I2C_H__
#define __INC_I2C_H__

/* -------------------- LIBRARIES -------------------- */
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"

/* ------------------- DECLARATIONS ------------------ */
//static const char *TAG_I2C = "I2C";

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

#define DELAY_TIME_BETWEEN_ITEMS_MS 1000 /*!< delay time between different test items */

#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ CONFIG_I2C_MASTER_FREQUENCY        /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */

#define SENSOR_BUS I2C_MASTER_NUM
#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */


// - Init I2C bus
esp_err_t i2c_master_init(void);
// - Read in a slave component
int32_t i2c_master_read_slave(uint8_t i2c_addr_sensor, uint8_t i2c_num, uint8_t regaddr, uint8_t *data_rd, uint16_t size);
// - Write in a slave component
int32_t i2c_master_write_slave(uint8_t i2c_addr_sensor, uint8_t i2c_num, uint8_t regaddr, uint8_t *data_wr, uint16_t size);

#endif