#include "dataManagement.h"

void initAll(){
    DM.i = 0;
    DTH.i = 0;
    DP.i = 0;
}

void clearAll(){
    initAll();
}

void saveAllData(){
    //To be define
}

void setDataMouv(double wx,double wy,double wz,double ax,double ay,double az,double mx,double my,double mz){

    DM.Dacc_x[DM.i] = ax;
    DM.Dacc_y[DM.i] = ay;
    DM.Dacc_z[DM.i] = az;

    DM.Dgyro_x[DM.i] = wx;
    DM.Dgyro_y[DM.i] = wy;
    DM.Dgyro_z[DM.i] = wz;

    DM.Dmagn_x[DM.i] = mx; 
    DM.Dmagn_y[DM.i] = my; 
    DM.Dmagn_z[DM.i] = mz; 
    DM.i++;
}

void setDataTempHumi(double temp, double humi){
    DTH.Dtemp[DTH.i] = temp;
    DTH.Dhumi[DTH.i] = humi;
    DTH.i++;
}

void setDataPressur(double pressur){
    DP.Dpressure[DP.i] = pressur;
    DP.i++;
}