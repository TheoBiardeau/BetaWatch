#include "timer_bw.h"
#include "LPS_user.h"
#include "Accelero_et_Gyro.h"

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

    I2CSema = xSemaphoreCreateMutex();
    xSemaphoreGive(I2CSema);

    xTaskCreatePinnedToCore(setDataMouv, "setDataMouv", 10000, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(setDataPressur, "setDataPressur", 10000, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(setDataTempHumi, "setDataTempHumi", 10000, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(DataChoose, "DataChoose", 10000, NULL, 5, NULL, 0);
    printf("init data ok \n");
}

void saveAllData()
{
    // To be define but currently this function reset the queues
    xQueueReset(dataMouvement_Queue_Sd);
    xQueueReset(dataTempHumi_Queue_Sd);
    xQueueReset(dataPressur_Queue_Sd);
}

void setDataMouv()
{
    if (xSemaphoreTake(I2CSema, (TickType_t)portMAX_DELAY))
    {
        DM  = get_LSM6DSO();
        printf("%f\n",DM.Dacc_x);

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
        xSemaphoreGive(I2CSema);
    }
    vTaskDelete(NULL);
}

void setDataTempHumi()
{

    if (xSemaphoreTake(I2CSema, (TickType_t)portMAX_DELAY))
    {
        // DTH = getData

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
    vTaskDelete(NULL);
}

void setDataPressur()
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
    vTaskDelete(NULL);
}

void DataChoose()
{
    while (1)
    {
        xQueueReceive(s_timer_queue, &nb_occ_timer, portMAX_DELAY);

        xTaskCreatePinnedToCore(setDataMouv, "setDataMouv", 10000, NULL, 4, NULL, 1);

        if (nb_occ_timer % 10 == 0)
        {

            xTaskCreatePinnedToCore(setDataPressur, "setDataPressur", 10000, NULL, 4, NULL, 1);
        }
        if (nb_occ_timer % 100 == 0)
        {

            xTaskCreatePinnedToCore(setDataTempHumi, "setDataTempHumi", 10000, NULL, 4, NULL, 1);
        }
    }
}