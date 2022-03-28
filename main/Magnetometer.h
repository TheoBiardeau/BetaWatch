/**************************************************************
   Magnetometer, from BetaWatch library, is available for ESP32 platform to manage
   our connected watch.

   Licensed under University of Poitiers Connected Objects Master's degree by BetaWatch group.
   Author: Henintsoa Andrianarivony.
 **************************************************************/
#ifndef Magneto
#define Magneto

/* -------------------- LIBRARIES -------------------- */
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "I2C.h"
#include "lis2mdl_reg.h"
#include "dataManagement.h"

/* ------------------- DECLARATIONS ------------------ */

static const char *TAG_MAGNETOMETER = "MAGNETOMETER";

#define LIS2MDL_ADDR_7BITS 0x1e

stmdev_ctx_t Driver_Magnetometer;

static int16_t data_raw_magnetic[3];
static int16_t data_raw_temperature;
static float magnetic_mG[3];
static float temperature_degC;
static uint8_t whoamI, rst;

T_dataMagnetique Dmagne;


// - I2C functions to read/write regs into Magnetometer sensor  
static int32_t i2c_master_read_Megnetometer(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_rd, uint16_t size);
static int32_t i2c_master_write_Megnetometer(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_wr, uint16_t size);

// - Initialize magnetometer
bool magnetometerInit();

// - Capture magnetic values
T_dataMagnetique magnetometerCapture();

// - Monitoring data via UART serial port
static void displayMagnetic();

#endif