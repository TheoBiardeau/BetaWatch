#include <stdio.h>
#include "lv.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "timer_bw.h"
#include "Gpio_bw.h"
#include "BLE.h"
#include "LPS_user.h"
void app_main(void)
{
    i2c_master_init();
    timer_initAll(TIMER_GROUP_0, TIMER_0, true, 0.1);
    initQueuesSensors();
    initGPIO();
    //Ble_launch();
    launchLVGL(1);
    uint32_t test = 0;
    while (1)
    {
        xQueueReceive(s_timer_queue, &test, portMAX_DELAY);
        printf("Number of alarm %d \n", test);
        printf("%d \n", chooseScreen);

    }
}
