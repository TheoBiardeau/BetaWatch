#include <stdbool.h>
#include <stdio.h>

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
    double Dacc_x[SIZE_MVT];
    double Dacc_y[SIZE_MVT];
    double Dacc_z[SIZE_MVT];

    double Dgyro_x[SIZE_MVT];
    double Dgyro_y[SIZE_MVT];
    double Dgyro_z[SIZE_MVT];

    double Dmagn_x[SIZE_MVT];
    double Dmagn_y[SIZE_MVT];
    double Dmagn_z[SIZE_MVT];

    uint16_t i;
} T_dataMouvement;

typedef struct
{
    double Dtemp[SIZE_TH];
    double Dhumi[SIZE_TH];

    uint16_t i;
} T_dataTempHumi;

typedef struct
{
    int32_t Dpressure[SIZE_PRESURE];

    uint16_t i;
} T_dataPressur;

T_dataMouvement DM;
T_dataPressur DP;
T_dataTempHumi DTH;

/*******************************
 *       Prototype fonction    *
 *******************************/

void initAll();

void clearAll();

void setDataMouv(double wx,double wy,double wz,double ax,double ay,double az,double mx,double my,double mz);
void setDataTempHumi(double temp, double humi);
void setDataPressur(double pressur);

void saveAllData();