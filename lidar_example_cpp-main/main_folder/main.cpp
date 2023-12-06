#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sl_lidar.h"
#include "sl_lidar_driver.h"


#include "lidar.h"

using namespace sl;

int main(int argc, const char * argv[]){

  ILidarDriver *lidar;
  myGrabData myData[1000];
  float maxDistance = 1.25;
  int filterNumber = 0;



  FILE* fp;
  FILE* fp1;
  fp = fopen("Data/LidarDataTable0.txt", "a");
  fp1 = fopen("Data/LidarDataFiltered0.txt", "a");

	lidar = connectLidar();

  int i = 0;
  int cycle = 1;


  while (i < cycle) {
    filterNumber = myScanData(lidar, myData, maxDistance, fp);
    printf("filterNumber : %d\n", filterNumber);


    myGrabData* filteredData = (myGrabData*) malloc(filterNumber*sizeof(myGrabData));
    filter(myData, filteredData, filterNumber, maxDistance);
    //makeCluster(myData);
    i++;
  }
  
  for (int j = 0; j < (int) myData->size; j++) {
    fprintf(fp1, "angle : %f, distance : %f\n", myData[j].angle, myData[j].distance);
  }
  

  
	disconnectLidar(lidar);

  fclose(fp);
  fclose(fp1);

  return 0;
}
