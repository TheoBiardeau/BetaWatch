/**************************************************************
   Magnetometer, from BetaWatch library, is available for ESP32 platform to manage
   our connected watch.

   Licensed under University of Poitiers Connected Objects Master's degree by BetaWatch group.
   Author: Henintsoa Andrianarivony.
 **************************************************************/

#include "Magnetometer.h"

bool confcond = 1;

// - I2C functions to read/write regs into Magnetometer sensor
static int32_t i2c_master_read_Megnetometer(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_rd, uint16_t size)
{
    return i2c_master_read_slave(LIS2MDL_ADDR_7BITS, i2c_num, regaddr, data_rd, size);
}
static int32_t i2c_master_write_Megnetometer(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_wr, uint16_t size)
{
    return i2c_master_write_slave(LIS2MDL_ADDR_7BITS, i2c_num, regaddr, data_wr, size);
}

// - Initialize magnetometer
bool magnetometerInit()
{

    Driver_Magnetometer.write_reg = i2c_master_write_Megnetometer;
    Driver_Magnetometer.read_reg = i2c_master_read_Megnetometer;
    Driver_Magnetometer.handle = SENSOR_BUS;

    lis2mdl_device_id_get(&Driver_Magnetometer, &whoamI);

    if (whoamI == LIS2MDL_ID)
    {
        /* Restore default configuration */
        lis2mdl_reset_set(&Driver_Magnetometer, PROPERTY_ENABLE);
        do
        {
            lis2mdl_reset_get(&Driver_Magnetometer, &rst);
        } while (rst);

        /* Enable Block Data Update */
        lis2mdl_block_data_update_set(&Driver_Magnetometer, PROPERTY_ENABLE);
        /* Set Output Data Rate */
        lis2mdl_data_rate_set(&Driver_Magnetometer, LIS2MDL_ODR_10Hz);
        /* Set / Reset sensor mode */
        lis2mdl_set_rst_mode_set(&Driver_Magnetometer, LIS2MDL_SENS_OFF_CANC_EVERY_ODR);
        /* Enable temperature compensation */
        lis2mdl_offset_temp_comp_set(&Driver_Magnetometer, PROPERTY_ENABLE);
        /* Set device in continuous mode */
        lis2mdl_operating_mode_set(&Driver_Magnetometer, LIS2MDL_CONTINUOUS_MODE);
        /* Set low power mode */
        lis2mdl_power_mode_set(&Driver_Magnetometer, LIS2MDL_LOW_POWER);
        /* Set a LP bandwidth */
        lis2mdl_low_pass_bandwidth_set(&Driver_Magnetometer, LIS2MDL_ODR_DIV_4);
        
        ESP_LOGI(TAG_MAGNETOMETER, "Sensor is properly init.");

        return true;
    }
    else
    {
        ESP_LOGE(TAG_MAGNETOMETER, "Sensor is not found.");

        return false;
    }
}

// - Refresh magnetic values
T_dataMagnetique magnetometerCapture()
{
    if (confcond == 1)
    {
        confcond = 0;
        printf("begore init\n");
        magnetometerInit();
        printf("after init\n");
    }
    uint8_t reg;
    /* Read output only if new value is available */
    lis2mdl_mag_data_ready_get(&Driver_Magnetometer, &reg);

    if (reg)
    {
        /* Read magnetic field data */
        memset(data_raw_magnetic, 0x00, 3 * sizeof(int16_t));
        lis2mdl_magnetic_raw_get(&Driver_Magnetometer, data_raw_magnetic);
        Dmagne.Dmagn_x = lis2mdl_from_lsb_to_mgauss(data_raw_magnetic[0]);
        Dmagne.Dmagn_y = lis2mdl_from_lsb_to_mgauss(data_raw_magnetic[1]);
        Dmagne.Dmagn_z = lis2mdl_from_lsb_to_mgauss(data_raw_magnetic[2]);

        // displayMagnetic(); // DEBUG : uncomment the line to debug the sensor
    }
    return Dmagne;
}

// - Monitoring data via UART serial port
static void displayMagnetic()
{
    printf("====== MONITORING MAGNETOMETER ======\r\n");
    printf("Magnetic field [mG]: x=%4.2f  y=%4.2f  z=%4.2f\r\n", magnetic_mG[0], magnetic_mG[1], magnetic_mG[2]);
}