#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sl_lidar.h"
#include "sl_lidar_driver.h"
#include "lidar.h"

using namespace sl;

int main(int argc, const char * argv[]){

  ILidarDriver *lidar;
	lidar = connectLidar();


  sl_lidar_response_measurement_node_hq_t local_buf[256];
  size_t count;

  //lidar->startScan(1,1,0,nullptr);
  lidar->grabScanDataHq(local_buf, count);

  

  for (int i = 0; i < 0; i++) {
    printf("%lf\n", (double) local_buf[i]);
  }

	disconnectLidar(lidar);

  return 0;
}
