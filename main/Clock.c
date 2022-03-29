/**************************************************************
   Clock, from BetaWatch library, is available for ESP32 platform to manage
   our connected watch.

   Licensed under University of Poitiers Connected Objects Master's degree by BetaWatch group.
   Author: Henintsoa Andrianarivony.
 **************************************************************/

#include "Clock.h"

/*** FOR THE RTC ***/

// - I2C functions to read/write regs into RTC device  
static int32_t i2c_master_read_RTC(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_rd, uint16_t size)
{
    return i2c_master_read_slave(RV3029C2_ADDR_7BITS, i2c_num, regaddr, data_rd, size);
}
static int32_t i2c_master_write_RTC(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_wr, uint16_t size)
{
    return i2c_master_write_slave(RV3029C2_ADDR_7BITS, i2c_num, regaddr, data_wr, size);
}

/*** FOR THE WIFI ***/

// - Establish Wi-Fi connection for SNTP service
static void esp_wifi_start_station(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    tcpip_adapter_init();

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t sta_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );
    ESP_ERROR_CHECK(esp_wifi_connect() );
}


/*** FOR THE SNTP ***/

// - Initialize SNTP connection
static void sntpInit(void) {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}

// Synchronize time via SNTP
static void sntpSynch(void) {
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK( esp_event_loop_create_default() );

    /**
     * NTP server address could be aquired via DHCP,
     * see LWIP_DHCP_GET_NTP_SRV menuconfig option
     */
#ifdef LWIP_DHCP_GET_NTP_SRV
    sntp_servermode_dhcp(1);
#endif

    esp_wifi_start_station();

    sntpInit();

    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG_CLOCK, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);

    ESP_ERROR_CHECK(esp_wifi_stop());
}

/*** FOR CLOCK DRIVER ***/


// - Initialize clock
void clockInit(void) {

    rv3029Driver.write_reg = i2c_master_write_RTC;
    rv3029Driver.read_reg = i2c_master_read_RTC;
    rv3029Driver.handle = SENSOR_BUS;
}

// - Synchronize clock
void clockSychronize() {
    time_t now;
    struct tm timeinfo;

    // Use sntp to synchronize ESP internal clock
    sntpSynch();
    // Grab current time with a time_t type variable
    time(&now);
    // Fit current time in a struc tm type
    localtime_r(&now, &timeinfo);
    // Set current time into the targeted RTC
    int ret = rv3029c2_time_set(&rv3029Driver, &timeinfo);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG_CLOCK, "Device is properly synchronized.");
        //displayTime(); //DEBUG : uncomment the line to debug the sensor
    } else {
        ESP_LOGE(TAG_CLOCK, "Divice cannot be synchronized.");
    }
}

// - Get the timestamp of the current clock time 
struct tm clockGetTime() {
    struct tm tm_now;
    
    // Grab current time from the RTC
    rv3029c2_time_get(&rv3029Driver, &tm_now);
    //displayTime(); //DEBUG : uncomment the line to debug the sensor

    return tm_now;
}

// - Monitoring data via UART serial port
static void displayTime(){
    char strftime_buf[64];
    struct tm timeinfo;

    // Grab current time from the RTC
    rv3029c2_time_get(&rv3029Driver, &timeinfo);

    // Set timezone to France Standard Time and stringify time
    setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);
    tzset();
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    
    // Display time
    printf("====== MONITORING CLOCK ======\r\n");
    printf("The current date/time in France is: %s\r\n", strftime_buf);
}
