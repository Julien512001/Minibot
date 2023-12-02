#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sl_lidar.h"
#include "sl_lidar_driver.h"
#include "lidar.h"
#include "cluster.h"

using namespace sl;

int main(int argc, const char * argv[]){

  ILidarDriver *lidar;
  myGrabData myData[1000];
  float maxDistance = 1.25;
  FILE* fp;
  fp = fopen("Data/LidarDataTest.txt", "a");

	lidar = connectLidar();

  myScanData(lidar, myData, maxDistance, fp);


  printf("angle : %f, distance : %f\n", myData[0].angle, myData[0].distance);



  
	disconnectLidar(lidar);

  fclose(fp);

  return 0;
}
