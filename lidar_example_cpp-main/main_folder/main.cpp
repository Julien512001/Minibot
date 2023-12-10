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
    interDistance(myClusterMean, distMatrix, clustNumber);

  
    FILE* fp4;
    fp4 = fopen("DataM/LidarDistM0.txt", "w");
    for (int i = 0; i < matrixSize; i++) {
      fprintf(fp4,"dist : %f, i : %d, j : %d\n", distMatrix[i].euclidian, distMatrix[i].i, distMatrix[i].j);
    }
    fclose(fp4);


    float triangle[] = {1.820820, 1.241578, 1.221634};
    clusterMean* newCluster = (clusterMean*) malloc(3*sizeof(clusterMean));

    findTriangle(triangle, distMatrix, newCluster, myClusterMean, matrixSize);

    FILE* fp5;
    fp5 = fopen("DataM/LidarBalise0.txt", "w");
    for (int i = 0; i < 3; i++) {
      fprintf(fp5,"angle : %f, distance : %f\n", newCluster[i].angle, newCluster[i].distance);
    }
    fclose(fp5);
    
    
    fclose(fp3);
    free(myClusterMean);
    free(filteredData);
    free(distMatrix);
    free(newCluster);
    i++;
  }

	disconnectLidar(lidar);
  

  return 0;
}
