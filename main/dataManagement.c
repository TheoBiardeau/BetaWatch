#include "timer_bw.h"
#include "dataManagement.h"

void initQueuesSensors()
{
    dataMouvement_Queue_Sd = xQueueCreate(SIZE_MVT, sizeof(DM));
    dataMouvement_Queue_Ble = xQueueCreate(1, sizeof(DM));
    dataMouvement_Queue_Screen = xQueueCreate(1, sizeof(DM));

    dataTempHumi_Queue_Sd = xQueueCreate(SIZE_TH, sizeof(DTH));
    dataTempHumi_Queue_Ble = xQueueCreate(1, sizeof(DTH));
    dataTempHumi_Queue_Screen = xQueueCreate(1, sizeof(DTH));

    dataPressur_Queue_Sd = xQueueCreate(SIZE_PRESURE, sizeof(DP));
    dataPressur_Queue_Ble = xQueueCreate(1, sizeof(DP));
    dataPressur_Queue_Screen = xQueueCreate(1, sizeof(DP));

    I2CSema = xSemaphoreCreateMutex();
    xSemaphoreGive(I2CSema);

    xTaskCreate(setDataMouv, "setDataMouv", 10000, NULL, 4, NULL);
    xTaskCreate(setDataPressur, "setDataPressur", 10000, NULL, 4, NULL);
    xTaskCreate(setDataTempHumi, "setDataTempHumi", 10000, NULL, 4, NULL);
    xTaskCreate(DataChoose, "DataChoose", 10000, NULL, 5, NULL);
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
        // DM = getDataMouvement

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
            printf("doesnt \n");
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

        // DP = getDataPressur;
        if (xQueueSend(dataPressur_Queue_Sd, (void *)&DTH, NULL) != pdPASS)
        {
            
        }

        if (xQueueSend(dataPressur_Queue_Ble, (void *)&DTH, NULL) != pdPASS)
        {
            /* Failed to post the message, even after 10 ticks. */
        }

        if (xQueueSend(dataPressur_Queue_Screen, (void *)&DTH, NULL) != pdPASS)
        {
            /* Failed to post the message, even after 10 ticks. */
        }
    }
    xSemaphoreGive(I2CSema);
    vTaskDelete(NULL);
}

void DataChoose()
{
    while (1)
    {
        xQueueReceive(s_timer_queue, &nb_occ_timer, portMAX_DELAY);

        xTaskCreate(setDataMouv, "setDataMouv", 10000, NULL, 4, NULL);

        if (nb_occ_timer % 10 == 0)
        {
            xTaskCreate(setDataPressur, "setDataPressur", 10000, NULL, 4, NULL);
        }
        if (nb_occ_timer % 100 == 0)
        {

            xTaskCreate(setDataTempHumi, "setDataTempHumi", 10000, NULL, 4, NULL);
        }
    }
}