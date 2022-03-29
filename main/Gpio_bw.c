#include "Gpio_bw.h"
#include "dataManagement.h"
#include "driver/gpio.h"

void IRAM_ATTR GPIO_iterupt(void *arg)
{
    if (((uint32_t)arg == GPIO_BUTTON) ? 1 : 0)
    {

        if (chooseScreen < 8 && chooseScreen >= 0)
        {
            chooseScreen++;
        }
        else
        {
            chooseScreen = 0;
        }
        xQueueSendFromISR(dataMouvement_Queue_Screen, &DM, pdFALSE);
        xQueueSendFromISR(dataTempHumi_Queue_Screen, &DTH, pdFALSE);
        xQueueSendFromISR(dataPressur_Queue_Screen, &DTH, pdFALSE);
    }
}


void initGPIO()
{   
    chooseScreen = 0;
    gpio_reset_pin(GPIO_BUTTON);
    gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT);
    gpio_install_isr_service(GPIO_BUTTON);
    gpio_set_intr_type(GPIO_BUTTON, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(GPIO_BUTTON, GPIO_iterupt, (void *)GPIO_BUTTON);
}
