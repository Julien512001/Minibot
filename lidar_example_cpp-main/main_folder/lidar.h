#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sl_lidar.h"
#include "sl_lidar_driver.h"
#include "rplidar.h"
#include "rplidar_driver.h"




#include <thread>

using namespace sl;

ILidarDriver* connectLidar();

void disconnectLidar(ILidarDriver* lidar);
