#include "LPS_user.h"
#include "Accelero_et_Gyro.h"
#include "Clock.h"
#include "Magnetometer.h"

void initQueuesSensors()
{

    dataMouvement_Queue_Sd = xQueueCreate(SIZE_MVT, sizeof(DM));
    dataMouvement_Queue_Ble = xQueueCreate(1, sizeof(DM));
    dataMouvement_Queue_Screen = xQueueCreate(1, sizeof(DM));

    dataMagn_Queue_Ble = xQueueCreate(SIZE_MVT, sizeof(DMA));
    dataMagn_Queue_Ble = xQueueCreate(1, sizeof(DMA));
    dataMagn_Queue_Screen = xQueueCreate(1, sizeof(DMA));

    dataTempHumi_Queue_Sd = xQueueCreate(SIZE_TH, sizeof(DTH));
    dataTempHumi_Queue_Ble = xQueueCreate(1, sizeof(DTH));
    dataTempHumi_Queue_Screen = xQueueCreate(1, sizeof(DTH));

    dataPressur_Queue_Sd = xQueueCreate(SIZE_PRESURE, sizeof(DP));
    dataPressur_Queue_Ble = xQueueCreate(1, sizeof(DP));
    dataPressur_Queue_Screen = xQueueCreate(1, sizeof(DP));

    dataTime_Queue_Sd = xQueueCreate(SIZE_PRESURE, sizeof(DP));
    dataTime_Queue_Ble = xQueueCreate(1, sizeof(DP));
    dataTime_Queue_Screen = xQueueCreate(1, sizeof(DP));

    I2CSema = xSemaphoreCreateMutex();
    xSemaphoreGive(I2CSema);

    xTaskCreatePinnedToCore(setDataPressure_Task, "setDataPressure_Task", 10000, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(setDataMouv_Task, "setDataMouv_Task", 10000, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(setDataTempHumi_Task, "setDataTempHumi_Task", 10000, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(getTimeOfClock_Task, "getTimeOfClock_Task", 10000, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(setDataMagn_Task, "setDataMagn_Task", 10000, NULL, 4, NULL, 1);

    printf("init data ok \n");
}

void saveAllData()
{
    // To be define but currently this function reset the queues
    xQueueReset(dataMouvement_Queue_Sd);
    xQueueReset(dataTempHumi_Queue_Sd);
    xQueueReset(dataPressur_Queue_Sd);
}

void setDataMouv_Task()
{
    while (1)
    {
        if (xSemaphoreTake(I2CSema, (TickType_t)portMAX_DELAY))
        {
            DM = get_LSM6DSO();
            printf("%f \n", DMA.Dmagn_x);

            if (xQueueSend(dataMouvement_Queue_Sd, (void *)&DM, NULL) != pdPASS)
            {
                /* Failed to post the message, even after 10 ticks. */
            }

            if (xQueueSend(dataMouvement_Queue_Ble, (void *)&DM, NULL) != pdPASS)
            {
                /* Failed to post the message, even after 10 ticks. */
            }

            if (xQueueSend(dataMouvement_Queue_Screen, (void *)&DM, NULL) != pdPASS)
            {
            }

            if (xQueueSend(dataMagn_Queue_Screen, (void *)&DMA, NULL) != pdPASS)
            {
            }
            xSemaphoreGive(I2CSema);
        }
        vTaskDelay(10);
    }
}

void setDataMagn_Task()
{
    while (1)

    {
        if (xSemaphoreTake(I2CSema, (TickType_t)portMAX_DELAY))
        {
            DMA = magnetometerCapture();

            xSemaphoreGive(I2CSema);
        }
        vTaskDelay(10);
    }
}

void setDataTempHumi_Task()
{
    while (1)

    {
        if (xSemaphoreTake(I2CSema, (TickType_t)portMAX_DELAY))
        {
            if (xQueueSend(dataTempHumi_Queue_Sd, (void *)&DTH, NULL) != pdPASS)
            {
                /* Failed to post the message, even after 10 ticks. */
            }

            if (xQueueSend(dataTempHumi_Queue_Ble, (void *)&DTH, NULL) != pdPASS)
            {
                /* Failed to post the message, even after 10 ticks. */
            }

            if (xQueueSend(dataTempHumi_Queue_Screen, (void *)&DTH, NULL) != pdPASS)
            {
                /* Failed to post the message, even after 10 ticks. */
            }
            xSemaphoreGive(I2CSema);
        }
        vTaskDelay(10000);
    }
}

void setDataPressure_Task()
{
    while (1)

    {
        if (xSemaphoreTake(I2CSema, (TickType_t)portMAX_DELAY))
        {
            DP = getPressure();
            if (xQueueSend(dataPressur_Queue_Sd, (void *)&DP, NULL) != pdPASS)
            {
            }

            if (xQueueSend(dataPressur_Queue_Ble, (void *)&DP, NULL) != pdPASS)
            {
            }

            if (xQueueSend(dataPressur_Queue_Screen, (void *)&DP, NULL) != pdPASS)
            {
            }
            xSemaphoreGive(I2CSema);
        }
        vTaskDelay(1000);
    }
}

void getTimeOfClock_Task()
{
    while (1)
    {
        if (xSemaphoreTake(I2CSema, (TickType_t)portMAX_DELAY))
        {
            tm_on_task = clockGetTime();
            if (xQueueSend(dataTime_Queue_Screen, (void *)&tm_on_task, NULL) != pdPASS)
            {
            }

            if (xQueueSend(dataTime_Queue_Screen, (void *)&tm_on_task, NULL) != pdPASS)
            {
            }

            if (xQueueSend(dataTime_Queue_Screen, (void *)&tm_on_task, NULL) != pdPASS)
            {
            }
            xSemaphoreGive(I2CSema);
        }
        vTaskDelay(1000);
    }
}
