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


#include <thread>

using namespace sl;

ILidarDriver* connectLidar();
void disconnectLidar(ILidarDriver* lidar);
void myScanData(ILidarDriver* lidar, myGrabData* myData, float maxDistance, FILE* fp);
void makeCluster(myGrabData* myData);