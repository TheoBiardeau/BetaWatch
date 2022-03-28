/* i2c - STTS751 ESP 32
   Adapted from ESPRESSIF example
   Hervé BOEGLEN 02/2020

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "hts221_reg.h"
#include "HTS221_user.h"

static const char *TAG = "esp32_stts751";

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

#define DELAY_TIME_BETWEEN_ITEMS_MS 1000 /*!< delay time between different test items */

#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ CONFIG_I2C_MASTER_FREQUENCY        /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */

#define HTS221_ADDR_7BITS 0x5f
#define SENSOR_BUS I2C_MASTER_NUM
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will che,vck ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */

typedef struct
{
    float x0;
    float y0;
    float x1;
    float y1;
} lin_t;

float linear_interpolation(lin_t *lin, int16_t x)
{
    return ((lin->y1 - lin->y0) * x + ((lin->x1 * lin->y0) - (lin->x0 * lin->y1))) / (lin->x1 - lin->x0);
}

// Private variables
static uint8_t whoamI;
stmdev_ctx_t dev_ctx;

static int32_t data_raw_humidity;
static float humidity_perc;
static int32_t data_raw_temperature;
static float temperature_degC;

lin_t lin_hum;
lin_t lin_temp;

TaskHandle_t xHandle = NULL;

hts221_reg_t reg;

bool loopconf = 1;

/**
 * @brief test code to read esp-i2c-slave like a memory device
 *        We need to fill the buffer of esp slave device, then master can read them out.
 *
 * _____________________________________________________________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 1 byte (register address) + ack | start | slave_addr + rd_bit + ack | read n bytes + nack | stop |
 * --------|---------------------------|---------------------------------------|-------|---------------------------|---------------------|------|
 *
 */
static int32_t i2c_master_read_slave(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_rd, uint16_t size)
{
    if (size == 0)
    {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (HTS221_ADDR_7BITS << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regaddr, ACK_CHECK_EN);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (HTS221_ADDR_7BITS << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    if (size > 1)
    {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief Test code to write esp-i2c-slave
 *        Master device write data to slave memory like device,
 *        the data will be stored in slave buffer.
 *        We can read them out from slave buffer.
 *
 * __________________________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 1 byte (register address) + ack | write n bytes + ack  | stop |
 * --------|---------------------------|---------------------------------------|----------------------|------|
 *
 */
static int32_t i2c_master_write_slave(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_wr, uint16_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (HTS221_ADDR_7BITS << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regaddr, ACK_CHECK_EN);
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

T_dataTempHumi get_temphumi()
{

    while (loopconf)
    {
        dev_ctx.write_reg = i2c_master_write_slave;
        dev_ctx.read_reg = i2c_master_read_slave;
        dev_ctx.handle = SENSOR_BUS;
        ESP_LOGI(TAG, "WHOAMI TASK");

        whoamI = 0;
        hts221_device_id_get(&dev_ctx, &whoamI);

        if (whoamI != HTS221_ID)
        {
            ESP_LOGE(TAG, "WHOAMI failed : hts221\n");
            vTaskDelete(NULL);
        }

        /* Read humidity calibration coefficient */
        hts221_hum_adc_point_0_get(&dev_ctx, &lin_hum.x0);
        hts221_hum_rh_point_0_get(&dev_ctx, &lin_hum.y0);
        hts221_hum_adc_point_1_get(&dev_ctx, &lin_hum.x1);
        hts221_hum_rh_point_1_get(&dev_ctx, &lin_hum.y1);

        /* Read temperature calibration coefficient */
        hts221_temp_adc_point_0_get(&dev_ctx, &lin_temp.x0);
        hts221_temp_deg_point_0_get(&dev_ctx, &lin_temp.y0);
        hts221_temp_adc_point_1_get(&dev_ctx, &lin_temp.x1);
        hts221_temp_deg_point_1_get(&dev_ctx, &lin_temp.y1);

        /* Set Output Data Rate */
        hts221_data_rate_set(&dev_ctx, HTS221_ODR_1Hz);

        /* Device power on */
        hts221_power_on_set(&dev_ctx, PROPERTY_ENABLE);

        ESP_LOGI(TAG, "Whoami OK");
        loopconf = 0;
    }
    /* Read output only if new value is available */
    hts221_status_get(&dev_ctx, &reg.status_reg);

    if (reg.status_reg.t_da)
    {
        /* Read temperature data */
        memset(&data_raw_temperature, 0x00, sizeof(int16_t));
        hts221_temperature_raw_get(&dev_ctx, &data_raw_temperature);
        temperature_degC = linear_interpolation(&lin_temp, data_raw_temperature);

        DTHL.Dtemp = temperature_degC;
    }
    hts221_status_get(&dev_ctx, &reg.status_reg);

    if (reg.status_reg.h_da)
    {
        /* Read humidity data */
        memset(&data_raw_humidity, 0x00, sizeof(int16_t));
        hts221_humidity_raw_get(&dev_ctx, &data_raw_humidity);
        humidity_perc = linear_interpolation(&lin_hum, data_raw_humidity);

        if (humidity_perc < 0)
        {
            humidity_perc = 0;
        }

        if (humidity_perc > 100)
        {
            humidity_perc = 100;
        }

        DTHL.Dhumi = humidity_perc;
    }
    return DTHL;
}
