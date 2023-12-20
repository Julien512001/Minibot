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
  float angle_th = 2.0;
  float distance_th = 0.1;

//  float Lsize = 1.85;
//  float Ssize = 1.25;
  float Lsize = 1.30;
  float Ssize = 0.97;
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
    fp3 = fopen("DataL/LidarMeanL0.txt", "w");
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
    fp4 = fopen("DataL/LidarDistL0.txt", "w");
    for (int i = 0; i < matrixSize; i++) {
      fprintf(fp4,"dist : %f, i : %d, j : %d\n", distMatrix[i].euclidian, distMatrix[i].i, distMatrix[i].j);
    }
    fclose(fp4);

    printf("distCount %d\n", distCount);
    myDistance* distFilter = (myDistance*) malloc(distCount*sizeof(myDistance));
    distanceFilter(distFilter, distMatrix, matrixSize, Lsize, Ssize, dist_th);

    FILE* fp5;
    fp5 = fopen("DataL/LidarDistFiltL0.txt", "w");
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
    fp8 = fopen("DataL/LidarMatrixL0.txt", "w");
    for (int i = 0; i < matrixCount; i ++) {
      for (int j = 0; j < matrixCount; j++) {
        fprintf(fp8,"%f, ", matrix[i][j]);
      }
      fprintf(fp8,"\n");
    }
    fclose(fp8);

    float distFilt_th = 0.05;
    //float triangle_ref[] = {1.83,1.25,1.25};
    float triangle_ref[] = {Lsize,Ssize,Ssize};
    myTriangle* triangle = (myTriangle*) malloc(matrixCount*sizeof(myTriangle));

    int triangleNumber;

    triangleNumber = makeTriangle(matrix, triangle_ref, triangle, distFilt_th, matrixCount);

    triangle = (myTriangle*) realloc(triangle, triangleNumber*sizeof(myTriangle));

    FILE* fp7;
    fp7 = fopen("DataL/LidarIndexL0.txt", "w");
    for (int i = 0; i < triangleNumber; i ++) {
      fprintf(fp7, "%d, %d, %d\n", triangle[i].i, triangle[i].j, triangle[i].k);
    }
    fclose(fp7);
    printf("triangleNumber : %d\n", triangleNumber);



    FILE* fp6;
    fp6 = fopen("DataL/LidarBaliseL0.txt", "w");
    

    fprintf(fp6, "angle : %f, distance : %f\n", myClusterMean[triangle[0].i].angle, myClusterMean[triangle[0].i].distance);
    fprintf(fp6, "angle : %f, distance : %f\n", myClusterMean[triangle[0].j].angle, myClusterMean[triangle[0].j].distance);
    fprintf(fp6, "angle : %f, distance : %f\n", myClusterMean[triangle[0].k].angle, myClusterMean[triangle[0].k].distance);
    fclose(fp6);

    FILE* fp9;
    fp9 = fopen("DataL/LidarBaliseL1.txt", "w");
    

    fprintf(fp9, "angle : %f, distance : %f\n", myClusterMean[triangle[1].i].angle, myClusterMean[triangle[1].i].distance);
    fprintf(fp9, "angle : %f, distance : %f\n", myClusterMean[triangle[1].j].angle, myClusterMean[triangle[1].j].distance);
    fprintf(fp9, "angle : %f, distance : %f\n", myClusterMean[triangle[1].k].angle, myClusterMean[triangle[1].k].distance);
    fclose(fp9);







/*
    float triangle[] = {1.820820, 1.241578, 1.221634};
    clusterMean* newCluster = (clusterMean*) malloc(3*sizeof(clusterMean));
*/




/*
    findTriangle(triangle, distMatrix, newCluster, myClusterMean, matrixSize);

    FILE* fp6;myTriangle
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
    free(triangle);

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
