#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "lsm6dso_reg.h"
#include "driver/i2c.h"

#include "Accelero_et_Gyro.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

//#include "./BLE/ble_init.h"
//#include "./BLE/gatt_server_profile.h"

#include "sdkconfig.h"

static const char *TAG = "esp32_lsm6do";

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

#define DELAY_TIME_BETWEEN_ITEMS_MS 1000 /*!< delay time between different test items */

#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ CONFIG_I2C_MASTER_FREQUENCY        /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */

#define LSM6DO_ADDR_7BITS 0x6b
#define SENSOR_BUS I2C_MASTER_NUM
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */

typedef union
{
    int16_t i16bit[3];
    uint8_t u8bit[6];
} axis3bit16_t;

static uint8_t whoamI, rst;
stmdev_ctx_t lsm6dsoDriver;
// static float angular_rate_mdps_LSM6DSOX[3];
// static float acceleration_mg_LSM6DSOX[3];
static axis3bit16_t data_raw_acceleration_LSM6DSOX;
static axis3bit16_t data_raw_angular_rate_LSM6DSOX;
TaskHandle_t xHandle_lsm6dso = NULL;
uint8_t reg_LSM6DSOX;
bool loopcond = 1;

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
    i2c_master_write_byte(cmd, (LSM6DO_ADDR_7BITS << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regaddr, ACK_CHECK_EN);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LSM6DO_ADDR_7BITS << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
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
    i2c_master_write_byte(cmd, (LSM6DO_ADDR_7BITS << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regaddr, ACK_CHECK_EN);
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE; // Pullup resistors are already present on X-NUCLEO-IKS01A3
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

T_dataMouvement get_LSM6DSO()
{
    int loop = 0;
    
    while (loopcond)
    {
        lsm6dsoDriver.write_reg = i2c_master_write_slave;
        lsm6dsoDriver.read_reg = i2c_master_read_slave;
        lsm6dsoDriver.handle = SENSOR_BUS;
        whoamI = 0;
        lsm6dso_device_id_get(&lsm6dsoDriver, &whoamI);
        /* Configure LSM6DSOX accelerometer */
        if (whoamI == LSM6DSO_ID)
        {
            /* Restore default configuration */
            lsm6dso_reset_set(&lsm6dsoDriver, PROPERTY_ENABLE);
            do
            {
                lsm6dso_reset_get(&lsm6dsoDriver, &rst);
            } while (rst);
            /* Disable I3C interface */
            lsm6dso_i3c_disable_set(&lsm6dsoDriver, LSM6DSO_I3C_DISABLE);
            /* Enable Block Data Update */
            lsm6dso_block_data_update_set(&lsm6dsoDriver, PROPERTY_ENABLE);
            /* Set Output Data Rate */
            lsm6dso_xl_data_rate_set(&lsm6dsoDriver, LSM6DSO_XL_ODR_6667Hz);
            lsm6dso_gy_data_rate_set(&lsm6dsoDriver, LSM6DSO_GY_ODR_6667Hz);
            /* Set full scale */
            lsm6dso_xl_full_scale_set(&lsm6dsoDriver, LSM6DSO_2g);
            lsm6dso_gy_full_scale_set(&lsm6dsoDriver, LSM6DSO_2000dps);
            /* Configure filtering chain */
            lsm6dso_xl_hp_path_on_out_set(&lsm6dsoDriver, LSM6DSO_LP_ODR_DIV_100);
            lsm6dso_xl_filter_lp2_set(&lsm6dsoDriver, PROPERTY_ENABLE);
            loopcond = 0;
        }
    }
    /* Check device ID for LSM6DSOX accelerometer */

    lsm6dso_xl_flag_data_ready_get(&lsm6dsoDriver, &reg_LSM6DSOX);

    if (reg_LSM6DSOX)
    {
        memset(data_raw_acceleration_LSM6DSOX.u8bit, 0x00, 3 * sizeof(int16_t));
        lsm6dso_acceleration_raw_get(&lsm6dsoDriver, data_raw_acceleration_LSM6DSOX.u8bit);
        DMT.Dacc_x = lsm6dso_from_fs2_to_mg(data_raw_acceleration_LSM6DSOX.i16bit[0]) / 1000;
        DMT.Dacc_y = lsm6dso_from_fs2_to_mg(data_raw_acceleration_LSM6DSOX.i16bit[1]) / 1000;
        DMT.Dacc_z = lsm6dso_from_fs2_to_mg(data_raw_acceleration_LSM6DSOX.i16bit[2]) / 1000;
    }

    lsm6dso_gy_flag_data_ready_get(&lsm6dsoDriver, &reg_LSM6DSOX);

    if (reg_LSM6DSOX)
    {
        memset(data_raw_angular_rate_LSM6DSOX.u8bit, 0x00, 3 * sizeof(int16_t));
        lsm6dso_angular_rate_raw_get(&lsm6dsoDriver, data_raw_angular_rate_LSM6DSOX.u8bit);
        DMT.Dgyro_x = lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate_LSM6DSOX.i16bit[0]) / 1000;
        DMT.Dgyro_y = lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate_LSM6DSOX.i16bit[1]) / 1000;
        DMT.Dgyro_z = lsm6dso_from_fs2000_to_mdps(data_raw_angular_rate_LSM6DSOX.i16bit[2]) / 1000;
    }

    return DMT;
}
