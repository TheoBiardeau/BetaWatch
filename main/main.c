#include <stdio.h>
#include "lv.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define GPIO_BUTTON 0

bool interupt_flag = 0;

void IRAM_ATTR tim_iterupt(void *arg)
{
    if (((uint32_t)arg == GPIO_BUTTON) ? 1 : 0)
    {
        if (xSemaphoreTakeFromISR(xGuiSemaphore, pdFALSE) == pdTRUE)
        {

            if( chooseScreen < 6 &&  chooseScreen >= 0)
            {
                chooseScreen ++;
            }
            else
            {
                chooseScreen = 0;
            }
            xSemaphoreGiveFromISR(xGuiSemaphore, pdFALSE);
        }
    }
   
}

void initGPIO()
{
    gpio_reset_pin(GPIO_BUTTON);
    gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT);
    gpio_install_isr_service(GPIO_BUTTON);
    gpio_set_intr_type(GPIO_BUTTON, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(GPIO_BUTTON, tim_iterupt, (void *)GPIO_BUTTON);
}

void app_main(void)
{
    initGPIO();
    launch(1);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100)); /* code */
        printf("%d \n",chooseScreen);
    }
}