/**************************************************************
   Clock, from BetaWatch library, is available for ESP32 platform to manage
   our connected watch.

   Licensed under University of Poitiers Connected Objects Master's degree by BetaWatch group.
   Author: Henintsoa Andrianarivony.
 **************************************************************/

#ifndef __INC_CLOCK_H__
#define __INC_CLOCK_H__

/* -------------------- LIBRARIES -------------------- */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

// RTC
#include "driver/timer.h"
#include "soc/rtc.h"
#include <time.h>
#include <sys/time.h>
#include "rv3029c2_reg.h"
#include "I2C.h"

// WIFI & SNTP
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_sntp.h"
#include "esp_wifi.h"

#include "lwip/err.h"
#include "lwip/sys.h"


/* ------------------- DECLARATIONS ------------------ */


/* ===== VARIABLES ===== */

/*** FOR THE WIFI ***/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY
// FreeRTOS event group to signal when we are connected
static EventGroupHandle_t s_wifi_event_group;

/*** FOR THE RTC ***/
static const char *TAG_CLOCK = "CLOCK";
#define RV3029C2_ADDR_7BITS 0x56

stmdev_ctx_t rv3029Driver;


/* ===== FUNCTIONS ===== */

/*** FOR THE RTC ***/

// - I2C functions to read/write regs into RTC device  
static int32_t i2c_master_read_RTC(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_rd, uint16_t size);
static int32_t i2c_master_write_RTC(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_wr, uint16_t size);


/*** FOR THE WIFI ***/

// - Establish Wi-Fi connection for SNTP service
static void esp_wifi_start_station(void);


/*** FOR THE SNTP ***/

// - Initialize SNTP connection
static void sntpInit(void);
// Synchronize time via SNTP
static void sntpSynch(void);


/*** FOR CLOCK DRIVER ***/

// - Initialize clock
void clockInit(void);
// - Synchronize clock
void clockSychronize();
// - Get the timestamp of the current clock time 
time_t clockGetTime();
// - Monitoring data via UART serial port
static void displayTime();

#endif