#include <stdbool.h>
#include <stdio.h>

/*******************************
 *       Public struct         *
 *******************************/ 
typedef struct
{
    double Dacc_x[5];
    double Dacc_y[5];
    double Dacc_z[5];

    double Dgyro_x[5];
    double Dgyro_y[5];
    double Dgyro_z[5];

    double Dmagn_x[5];
    double Dmagn_y[5];
    double Dmagn_z[5];

    uint16_t i;
} T_dataMouvement;

typedef struct 
{
    double Dtemp[5];
    double Dhumi[5];

    uint16_t i;
} T_dataTempHumi;

typedef struct 
{
    int32_t Dpressure[5];

    uint16_t i;
} T_dataPressur ;

/*******************************
 *       Prototype fonction    *
 *******************************/ 

void setDataMouv();
void setDataTempHumi();
void setDataPressur();
void saveAllData();