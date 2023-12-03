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
  FILE* fp;
  FILE* fp1;
  fp = fopen("Data/LidarDataTest.txt", "a");
  fp1 = fopen("Data/LidarDataCluster0.txt", "a");

	lidar = connectLidar();

  int i = 0;
  int cycle = 1;


  while (i < cycle) {
    myScanData(lidar, myData, maxDistance, fp);
    makeCluster(myData);
    i++;
  }

  for (int j = 0; j < (int) myData->size; j++) {
    fprintf(fp1, "angle : %f, distance : %f, cluster : %d\n", myData[j].angle, myData[j].distance, myData[j].clust);
  }
  


  
	disconnectLidar(lidar);

  fclose(fp);
  fclose(fp1);

  return 0;
}
