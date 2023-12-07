#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sl_lidar.h"
#include "sl_lidar_driver.h"


#include "lidar.h"

using namespace sl;

int main(int argc, const char * argv[]){

  ILidarDriver *lidar;
  size_t nodeCount = 1000;
  myGrabData myData[nodeCount];

  float maxDistance = 1.2;
  int filterNumber = 0;
  float angle_th = 1.0;
  float distance_th = 0.01;

	lidar = connectLidar();

  int i = 0;
  int cycle = 1;


  while (i < cycle) {
    int clustNumber;
    filterNumber = myScanData(lidar, myData, maxDistance, nodeCount);
    printf("filterNumber : %d\n", filterNumber);

    myGrabData* filteredData = (myGrabData*) malloc(filterNumber*sizeof(myGrabData));
    filter(myData, filteredData, filterNumber, maxDistance, nodeCount);

    clustNumber = makeCluster(filteredData, filterNumber, angle_th, distance_th);
    printf("clustnumber : %d\n", clustNumber);

    
    clusterMean* myClusterMean = (clusterMean*) malloc(clustNumber*sizeof(clusterMean));
    meanCluster(filteredData, myClusterMean, filterNumber);

    FILE* fp3;
    fp3 = fopen("Data/LidarMean0.txt", "w");
    for (int i = 0; i < clustNumber; i++) {
      fprintf(fp3,"angle : %f, distance : %f\n", myClusterMean[i].angle, myClusterMean[i].distance);
    }


    fclose(fp3);
    free(myClusterMean);
    free(filteredData);
    i++;
  }

	disconnectLidar(lidar);
  

  return 0;
}
