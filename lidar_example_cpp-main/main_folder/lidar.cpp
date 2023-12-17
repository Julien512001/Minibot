#include "lidar.h"



ILidarDriver* connectLidar(){

  ILidarDriver* lidar;
  IChannel* _channel;
	lidar = *createLidarDriver();
  _channel = (*createSerialPortChannel("/dev/ttyUSB0", 115200));
    if (SL_IS_OK((lidar)->connect(_channel))){
    	printf("Connected\n");

    	lidar->setMotorSpeed();
      LidarScanMode scanMode;
      lidar->startScan(false, true, 0, &scanMode);
      return lidar;
    }
    printf("Connection failed\n");

    return NULL;
}

void disconnectLidar(ILidarDriver* lidar){
    lidar->stop();
    lidar->setMotorSpeed(0);
    delete lidar;
}

int myScanData(ILidarDriver* lidar, myGrabData* myData, float maxDistance, size_t nodeCount) {

  int rotationCount = 1;
  int i = 0;
  int filterNumber = 0;
  FILE* fp;
  fp = fopen("Data/LidarTest0.txt", "a");
  sl_result res;
  while(i < rotationCount) {
    sl_lidar_response_measurement_node_hq_t nodes[nodeCount];
    res = lidar->grabScanDataHq(nodes, nodeCount);
    i++;
    if (SL_IS_OK(res)) {
      lidar->ascendScanData(nodes, nodeCount);
      for (int pos = 0; pos < (int)nodeCount ; ++pos) {
          if ((nodes[pos].dist_mm_q2/1000.f/ (1<<2) < maxDistance) & ((nodes[pos].dist_mm_q2/1000.f/ (1<<2)) != 0.0)) filterNumber ++;
          myData[pos].angle =    (nodes[pos].angle_z_q14 * 90.f) / (1<<14);
          myData[pos].distance = nodes[pos].dist_mm_q2/1000.f/ (1<<2);
          fprintf(fp, "angle : %f, distance : %f\n", myData[pos].angle, myData[pos].distance);
        }
      }
  }
  return filterNumber;
}


void filter(myGrabData* myData, myGrabData* filteredData, int filterNumber, float maxDistance, size_t nodeCount) {
  FILE* fp1;
  fp1 = fopen("Data/LidarDataM0.txt", "w");
  int count = 0;
  for (int i = 0; i < (int)nodeCount; i++) {
    if((myData[i].distance < maxDistance) & (myData[i].distance != 0)) {
      
      filteredData[count].distance = myData[i].distance;
      filteredData[count].angle    = myData[i].angle;
      count ++;
      //printf("angle : %f, dist : %f\n", myData[i].angle, myData[i].distance);
      //fprintf(fp1, "angle : %f, distance : %f\n", myData[i].angle, myData[i].distance);
    }
    else {
      count = count;;
    }
  }
  fclose(fp1);
}

int makeCluster(myGrabData* data, int count, float angle_th, float distance_th){

  FILE* fp2;
  fp2 = fopen("DataM/LidarClustM0.txt", "w");
  int number = 1;
  int clusterCount = 0;
  data[0].clust = number;
  for (int i = 1; i < count; i++) {
    if (((data[i].angle - data[i-1].angle) < angle_th/2.0) & ((data[i].distance - data[i-1].distance) < distance_th/2.0)) {
      data[i].clust = number;
      clusterCount ++;
      //printf("angle : %f, dist : %f, clust : %d\n", data[i].angle, data[i].distance, data[i].clust);
      fprintf(fp2, "angle : %f, distance : %f\n", data[i].angle, data[i].distance);
    } else {
      clusterCount = 0;
      number ++;
    }
  }

  fclose(fp2);

  return number;

}

void meanCluster(myGrabData* data, clusterMean* myClusterMean, int filterNumber) {
  int countClust = 1;
  int count = 0;
  int counter = 0;
  float angle_sum = 0.0;
  float distance_sum = 0.0;

  while (count < filterNumber) {
    if (data[count].clust == countClust) {
      angle_sum += data[count].angle;
      distance_sum += data[count].distance;
      count++;
      counter++;
    } 
    else {
      myClusterMean[countClust-1].angle = angle_sum/(counter);
      myClusterMean[countClust-1].distance = distance_sum/(counter);
      counter = 0;
      countClust++;
      count++;
      angle_sum = 0;
      distance_sum = 0;
    }
    if (count == filterNumber-1) {
      myClusterMean[countClust-1].angle = angle_sum/(counter);
      myClusterMean[countClust-1].distance = distance_sum/(counter);
    }
  }
}

float distance(clusterMean x1, clusterMean x2) {
  float x = (x2.distance*cos(M_PI/180 * x2.angle) - x1.distance*cos(M_PI/180 * x1.angle));
  float y = (x2.distance*sin(M_PI/180 * x2.angle) - x1.distance*sin(M_PI/180 * x1.angle));
  return isnan(sqrt(pow(x,2) + pow(y,2))) ? 0.0:sqrt(pow(x,2) + pow(y,2));
}


int interDistance(clusterMean* myClusterMean, myDistance* distMatrix, int N, float Lsize, float Ssize, float dist_th) {

  int i;
  int j;
  int k = 0;
  int distCount = 0;

  for (i = 0; i < N; i++) {
    for (j = i+1; j < N; j++) {
      distMatrix[k].euclidian = distance(myClusterMean[i], myClusterMean[j]);
      distMatrix[k].i = i;
      distMatrix[k].j = j;
      //printf("i+j-1 : %d\n", k);
      printf("dist : %f, i : %d, j : %d\n", distMatrix[k].euclidian, i, j);
      if (((distMatrix[k].euclidian < Lsize + dist_th) & (distMatrix[k].euclidian > Lsize - dist_th)) || ((distMatrix[k].euclidian < Ssize + dist_th) & (distMatrix[k].euclidian > Ssize - dist_th))) {
        distCount++;
      }
      k++;

    }
  }
  return distCount;
}


void distanceFilter(myDistance* new_dist, myDistance* old_dist, int size, float Lsize, float Ssize, float dist_th) {
  int count = 0;
  for (int i = 0; i < size; i++) {
    if (((old_dist[i].euclidian < Lsize + dist_th) & (old_dist[i].euclidian > Lsize - dist_th)) || ((old_dist[i].euclidian < Ssize + dist_th) & (old_dist[i].euclidian > Ssize - dist_th))) {
      new_dist[count] = old_dist[i];
      count ++;
    }
    else {
      count = count;
    }
  } 
  
}

void ArrayToMatrix(myDistance* array, float** matrix, int N) {
  for (int index = 0; index < N-1; index++) {
    matrix[array[index].i][array[index].j] = array[index].euclidian;
  }
}

int compare(float* a, float* b, float dist_th) {
  for (int i = 0; i < 3; i++) {
    if ((a[i] < b[i] - dist_th) || (a[i] > b[i] + dist_th)) {
      return 0;
    }
  }
  return 1;
}


int makeTriangle(float** matrix, float* triangle_ref, myTriangle* triangle, float dist_th, int size) {
  float buffer[3] = {0.0};
  float cons[3] = {0.0};
  int count = 0;
  for (int i = 0; i < size; i++) {
    for (int j = i+1; j < size; j++) {
      buffer[0] = matrix[i][j];
      for (int k = j+1; k < size; k++) {
        buffer[1] = matrix[i][k];
        buffer[2] = matrix[j][k];
        if (compare(buffer, triangle_ref, dist_th)) {
          triangle[count].i = i;
          triangle[count].j = j;
          triangle[count].k = k;
          count++;
        }
        cons[0] = triangle_ref[2];
        cons[1] = triangle_ref[0];
        cons[2] = triangle_ref[1];
        if (compare(buffer, cons, dist_th)) {
          triangle[count].i = i;
          triangle[count].j = j;
          triangle[count].k = k;
          count++;
        }
        cons[0] = triangle_ref[1];
        cons[1] = triangle_ref[2];
        cons[2] = triangle_ref[0];
        if (compare(buffer, cons, dist_th)) {
          triangle[count].i = i;
          triangle[count].j = j;
          triangle[count].k = k;
          count++;
        }
      }
    }
  }
  return count;
}