#include "dataManagement.h"

void initAll()
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
    printf("init Queue ok \n");
}

void clearAll()
{
    initAll();
}

void saveAllData()
{
    // To be define
}

void setDataMouv(double wx, double wy, double wz, double ax, double ay, double az, double mx, double my, double mz)
{

    DM.Dacc_x = ax;
    DM.Dacc_y = ay;
    DM.Dacc_z = az;

    DM.Dgyro_x = wx;
    DM.Dgyro_y = wy;
    DM.Dgyro_z = wz;

    DM.Dmagn_x = mx;
    DM.Dmagn_y = my;
    DM.Dmagn_z = mz;

    if (xQueueSend(dataMouvement_Queue_Sd,(void *)&DM,NULL) != pdPASS)
    {
        /* Failed to post the message, even after 10 ticks. */
    }

    if (xQueueSend(dataMouvement_Queue_Ble,(void *)&DM,NULL) != pdPASS)
    {
        /* Failed to post the message, even after 10 ticks. */
    }

    if (xQueueSend(dataMouvement_Queue_Screen,(void *)&DM,NULL) != pdPASS)
    {
        /* Failed to post the message, even after 10 ticks. */
    }
}

void setDataTempHumi(double temp, double humi)
{
    DTH.Dtemp = temp;
    DTH.Dhumi = humi;

    if (xQueueSend(dataTempHumi_Queue_Sd,(void *)&DTH,NULL) != pdPASS)
    {
        /* Failed to post the message, even after 10 ticks. */
    }

    if (xQueueSend(dataTempHumi_Queue_Ble,(void *)&DTH,NULL) != pdPASS)
    {
        /* Failed to post the message, even after 10 ticks. */
    }

    if (xQueueSend(dataTempHumi_Queue_Screen,(void *)&DTH,NULL) != pdPASS)
    {
        /* Failed to post the message, even after 10 ticks. */
    }
}


void setDataPressur(double pressur)
{
    DP.Dpressure = pressur;
    if (xQueueSend(dataPressur_Queue_Sd,(void *)&DTH,NULL) != pdPASS)
    {
        /* Failed to post the message, even after 10 ticks. */
    }

    if (xQueueSend(dataPressur_Queue_Ble,(void *)&DTH,NULL) != pdPASS)
    {
        /* Failed to post the message, even after 10 ticks. */
    }

    if (xQueueSend(dataPressur_Queue_Screen,(void *)&DTH,NULL) != pdPASS)
    {
        /* Failed to post the message, even after 10 ticks. */
    }
}