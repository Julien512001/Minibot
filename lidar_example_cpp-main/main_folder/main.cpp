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

  float maxDistance = 1.25;
  int filterNumber = 0;
  float angle_th = 5.0;
  float distance_th = 0.1;

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
    fp3 = fopen("DataM/LidarMeanM0.txt", "w");
    for (int i = 0; i < clustNumber; i++) {
      fprintf(fp3,"angle : %f, distance : %f\n", myClusterMean[i].angle, myClusterMean[i].distance);
    }


    int matrixSize = (clustNumber-1)*clustNumber/2;
    printf("matrixSize : %d\n", matrixSize);
    printf("hello0\n");


    float* distMatrix = (float*) malloc(matrixSize*sizeof(float));
    printf("hello1\n");
    interDistance(myClusterMean, distMatrix, clustNumber);

  
    FILE* fp4;
    fp4 = fopen("DataM/LidarDistM0.txt", "w");
    for (int i = 0; i < matrixSize; i++) {
      fprintf(fp4,"distance : %f\n", distMatrix[i]);
    }


    fclose(fp4);
    fclose(fp3);
    free(myClusterMean);
    free(filteredData);
    free(distMatrix);
    i++;
  }

	disconnectLidar(lidar);
  

  return 0;
}
