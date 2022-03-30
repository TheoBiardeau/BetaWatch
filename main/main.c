#include <stdio.h>
#include "lv.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "Gpio_bw.h"
#include "BLE.h"
#include "LPS_user.h"

#include "Clock.h"

void app_main(void)
{
    i2c_master_init();
    clockInit();
    clockSychronize();
    initQueuesSensors();
    initGPIO();
    launchLVGL(1);
    //Ble_launch();
    uint32_t test = 0;


}
