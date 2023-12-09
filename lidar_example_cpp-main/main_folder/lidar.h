#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sl_lidar.h"
#include "sl_lidar_driver.h"
#include "rplidar.h"
#include "rplidar_driver.h"

typedef struct {
    float distance;
    float angle;
    int clust;
    int size;
} myGrabData;

typedef struct {
    float angle;
    float distance;
} clusterMean;


#include <thread>

using namespace sl;

ILidarDriver* connectLidar();
void disconnectLidar(ILidarDriver* lidar);
int myScanData(ILidarDriver* lidar, myGrabData* myData, float maxDistance, size_t nodeCount);
void filter(myGrabData* myData, myGrabData* filteredData, int filterNumber, float maxDistance, size_t nodeCount);
int makeCluster(myGrabData* data, int count, float angle_th, float distance_th);
void meanCluster(myGrabData* data, clusterMean* myClusterMean, int filterNumber);
float distance(clusterMean x1, clusterMean x2);
void interDistance(clusterMean* myClusterMean, float* distMatrix, int N);
