#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


/*******************************
 *           const             *
 *******************************/
#define SIZE_MVT 600
#define SIZE_TH 6
#define SIZE_PRESURE 60

/*******************************
 *       Public struct         *
 *******************************/
typedef struct
{
    double Dacc_x;
    double Dacc_y;
    double Dacc_z;

    double Dgyro_x;
    double Dgyro_y;
    double Dgyro_z;

    double Dmagn_x;
    double Dmagn_y;
    double Dmagn_z;
} T_dataMouvement;

typedef struct
{
    double Dtemp;
    double Dhumi;
} T_dataTempHumi;

typedef struct
{
    int32_t Dpressure;
} T_dataPressur;

T_dataMouvement DM;
T_dataPressur DP;
T_dataTempHumi DTH;

/*******************************
 *            Queue            *
 *******************************/

QueueHandle_t dataMouvement_Queue_Sd ; 
QueueHandle_t dataMouvement_Queue_Ble ; 
QueueHandle_t dataMouvement_Queue_Screen ;

QueueHandle_t dataTempHumi_Queue_Sd ; 
QueueHandle_t dataTempHumi_Queue_Ble ; 
QueueHandle_t dataTempHumi_Queue_Screen ; 

QueueHandle_t dataPressur_Queue_Sd ; 
QueueHandle_t dataPressur_Queue_Ble ; 
QueueHandle_t dataPressur_Queue_Screen ; 

/*******************************
 *       Prototype fonction    *
 *******************************/

void initAll();

void clearAll();

void setDataMouv(double wx,double wy,double wz,double ax,double ay,double az,double mx,double my,double mz);
void setDataTempHumi(double temp, double humi);
void setDataPressur(double pressur);

void saveAllData();