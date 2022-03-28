#ifndef DATAMANAGEMENT
#define DATAMANAGEMENT

#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
/*******************************
 *           const             *
 *******************************/
#define SIZE_MVT 600
#define SIZE_TH 6
#define SIZE_PRESURE 60

/*******************************
 *       public struct         *
 *******************************/
typedef struct
{
    float Dacc_x;
    float Dacc_y;
    float Dacc_z;

    float Dgyro_x;
    float Dgyro_y;
    float Dgyro_z;

    uint8_t steps;

} T_dataMouvement;

typedef struct
{
    double Dmagn_x;
    double Dmagn_y;
    double Dmagn_z;

} T_dataMagnetique;

typedef struct
{
    double Dtemp;
    double Dhumi;
} T_dataTempHumi;

typedef struct
{
    double Dpressure;
} T_dataPressur;


/*******************************
 *       private struct         *
 *******************************/

static T_dataMouvement DM;
static T_dataPressur DP;
static T_dataTempHumi DTH;
static T_dataMagnetique DMA;
/*******************************
 *            Queue            *
 *******************************/

QueueHandle_t dataMouvement_Queue_Sd ; 
QueueHandle_t dataMouvement_Queue_Ble ; 
QueueHandle_t dataMouvement_Queue_Screen ;

QueueHandle_t dataMagn_Queue_Sd ; 
QueueHandle_t dataMagn_Queue_Ble ; 
QueueHandle_t dataMagn_Queue_Screen ;

QueueHandle_t dataTempHumi_Queue_Sd ; 
QueueHandle_t dataTempHumi_Queue_Ble ; 
QueueHandle_t dataTempHumi_Queue_Screen ; 

QueueHandle_t dataPressur_Queue_Sd ; 
QueueHandle_t dataPressur_Queue_Ble ; 
QueueHandle_t dataPressur_Queue_Screen ; 

/*******************************
 *       Prototype fonction    *
 *******************************/

void initQueuesSensors();

void setDataMouv();
void setDataTempHumi();
void setDataPressur();

void saveAllData();

void DataChoose();
/*******************************
 *       Private Variable   *
 *******************************/

static uint32_t nb_occ_timer;

/*******************************
 *       Semaphores   *
 *******************************/
xSemaphoreHandle I2CSema;

#endif