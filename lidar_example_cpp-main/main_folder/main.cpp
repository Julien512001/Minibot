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

  float maxDistance = 3.0;
  int filterNumber = 0;
  float angle_th = 1.0;
  float distance_th = 0.1;

  float Lsize = 1.85;
  float Ssize = 1.25;
  float dist_th = 0.1;

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
    fclose(fp3);

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
    
    int matrixCount = distCount +1;

    float** matrix = (float**) calloc(matrixCount, sizeof(float*));

    for (int i = 0; i < matrixCount; i++) {
      matrix[i] = (float*) calloc(matrixCount, sizeof(float));
    }


    ArrayToMatrix(distFilter, matrix, matrixCount);

    FILE* fp8;
    fp8 = fopen("DataM/LidarMatrix0.txt", "w");
    for (int i = 0; i < matrixCount; i ++) {
      for (int j = 0; j < matrixCount; j++) {
        fprintf(fp8,"%f, ", matrix[i][j]);
      }
      fprintf(fp8,"\n");
    }
    fclose(fp8);

    float distFilt_th = 0.1;
    float triangle_ref[] = {1.85,1.22,1.29};
    int myTriangle[3];

    makeTriangle(matrix, triangle_ref, myTriangle, distFilt_th, matrixCount);


    FILE* fp7;
    fp7 = fopen("DataM/LidarIndex0.txt", "w");
    for (int i = 0; i < 3; i ++) {
      fprintf(fp7, "myTriangle : %d\n", myTriangle[i]);
    }
    fclose(fp7);




    
    FILE* fp6;
    fp6 = fopen("DataM/LidarBalise0.txt", "w");
    
    for (int i = 0; i < 3; i++) {
      fprintf(fp6, "angle : %f, distance : %f\n", myClusterMean[myTriangle[i]].angle, myClusterMean[myTriangle[i]].distance);
    }

    fclose(fp6);








/*
    float triangle[] = {1.820820, 1.241578, 1.221634};
    clusterMean* newCluster = (clusterMean*) malloc(3*sizeof(clusterMean));
*/




/*
    findTriangle(triangle, distMatrix, newCluster, myClusterMean, matrixSize);

    FILE* fp6;
    fp6 = fopen("DataM/LidarBalise0.txt", "w");
    for (int i = 0; i < 3; i++) {
      fprintf(fp6,"angle : %f, distance : %f\n", newCluster[i].angle, newCluster[i].distance);
    }
    fclose(fp6);
  */
    

    free(myClusterMean);
    free(filteredData);
    free(distMatrix);
    free(distFilter);

    for (int i = 0; i < matrixCount; i++) {
        free(matrix[i]);
    }
    free(matrix);

    //free(newCluster);

    i++;
  }

	disconnectLidar(lidar);
  

  return 0;
}
