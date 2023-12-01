#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sl_lidar.h"
#include "sl_lidar_driver.h"
#include "lidar.h"

using namespace sl;

int main(int argc, const char * argv[]){

  int res;
  ILidarDriver *lidar;
	lidar = connectLidar();


  sl_lidar_response_measurement_node_hq_t nodes[8192];
  size_t nodeCount = sizeof(nodes)/sizeof(sl_lidar_response_measurement_node_hq_t);
  res = lidar->grabScanDataHq(nodes, nodeCount);
  if (res != 0) {
    fprintf(stderr, "Failed to get data");
  }

  float angle_in_degrees   = nodes->angle_z_q14 * 90.f / (1 << 14);
  float distance_in_meters = nodes->dist_mm_q2 / 1000.f / (1 << 2);


  
	disconnectLidar(lidar);

    return 0;
}
