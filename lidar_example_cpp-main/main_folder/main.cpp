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

  float maxDistance = 2.0;
  int filterNumber = 0;
  float angle_th = 5.0;
  float distance_th = 0.1;

  float Lsize = 1.85;
  float Ssize = 1.25;
  float dist_th = 0.05;

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


    myDistance* distMatrix = (myDistance*) malloc(matrixSize*sizeof(myDistance));
    printf("hello1\n");
    int distCount = interDistance(myClusterMean, distMatrix, clustNumber, Lsize, Ssize, dist_th);

  
    FILE* fp4;
    fp4 = fopen("DataM/LidarDistM0.txt", "w");
    for (int i = 0; i < matrixSize; i++) {
      fprintf(fp4,"dist : %f, i : %d, j : %d\n", distMatrix[i].euclidian, distMatrix[i].i, distMatrix[i].j);
    }
    fclose(fp4);

    printf("distCount %d\n", distCount);
    myDistance* distFilter = (myDistance*) malloc(distCount*sizeof(myDistance));
    distanceFilter(distFilter, distMatrix, matrixSize, Lsize, Ssize, dist_th);

    FILE* fp5;
    fp5 = fopen("DataM/LidarDistFiltM0.txt", "w");
    for (int i = 0; i < distCount; i++) {
      fprintf(fp5,"dist : %f, i : %d, j : %d\n", distFilter[i].euclidian, distFilter[i].i, distFilter[i].j);
    }
    fclose(fp5);


    float triangle[] = {1.820820, 1.241578, 1.221634};
    clusterMean* newCluster = (clusterMean*) malloc(3*sizeof(clusterMean));

/*
    findTriangle(triangle, distMatrix, newCluster, myClusterMean, matrixSize);

    FILE* fp6;
    fp6 = fopen("DataM/LidarBalise0.txt", "w");
    for (int i = 0; i < 3; i++) {
      fprintf(fp6,"angle : %f, distance : %f\n", newCluster[i].angle, newCluster[i].distance);
    }
    fclose(fp6);
  */
    
    fclose(fp3);
    free(myClusterMean);
    free(filteredData);
    free(distMatrix);
    free(distFilter);
    free(newCluster);

    i++;
  }

	disconnectLidar(lidar);
  

  return 0;
}
