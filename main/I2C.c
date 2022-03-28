/**************************************************************
   I2C, from BetaWatch library, is available for ESP32 platform to manage
   our connected watch (adapted from ESPRESSIF example).

   Licensed under University of Poitiers Connected Objects Master's degree by BetaWatch group.
   Author: Henintsoa Andrianarivony.
 **************************************************************/

#include "I2C.h"

/**
 * @brief test code to read esp-i2c-slave like a memory device
 *        We need to fill the buffer of esp slave device, then master can read them out.
 *
 * _____________________________________________________________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 1 byte (register address) + ack | start | slave_addr + rd_bit + ack | read n bytes + nack | stop |
 * --------|---------------------------|---------------------------------------|-------|---------------------------|---------------------|------|
 *
 */
int32_t i2c_master_read_slave(uint8_t i2c_addr_sensor, uint8_t i2c_num, uint8_t regaddr, uint8_t *data_rd, uint16_t size)
{
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (i2c_addr_sensor << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regaddr, ACK_CHECK_EN);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (i2c_addr_sensor << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    if (size > 1) {
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
int32_t i2c_master_write_slave(uint8_t i2c_addr_sensor, uint8_t i2c_num, uint8_t regaddr, uint8_t *data_wr, uint16_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (i2c_addr_sensor << 1) | WRITE_BIT, ACK_CHECK_EN);
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
esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.clk_flags = 0;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE; // Pullup resistors are already present on X-NUCLEO-IKS01A3
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}