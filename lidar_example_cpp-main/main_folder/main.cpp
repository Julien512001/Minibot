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
  FILE* fp;
  fp = fopen("Speed.txt", "a");

  sl_result res;
  while(true) {




    sl_lidar_response_measurement_node_hq_t nodes[8192];
    size_t nodeCount = sizeof(nodes)/sizeof(sl_lidar_response_measurement_node_hq_t);
    res = lidar->grabScanDataHq(nodes, nodeCount);
    
    if (SL_IS_OK(res)) {
      lidar->ascendScanData(nodes, nodeCount);
      for (int pos = 0; pos < (int)nodeCount ; ++pos) {
          printf("%s theta: %f Dist: %f Q: %d \n", 
                (nodes[pos].flag & SL_LIDAR_RESP_HQ_FLAG_SYNCBIT) ?"S ":"  ", 
                (nodes[pos].angle_z_q14 * 90.f) / (1<<14),
                nodes[pos].dist_mm_q2/1000.f/(1<<2),
                nodes[pos].quality >> SL_LIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);
          fprintf(fp,"%s theta: %f Dist: %f Q: %d \n", 
                (nodes[pos].flag & SL_LIDAR_RESP_HQ_FLAG_SYNCBIT) ?"S ":"  ", 
                (nodes[pos].angle_z_q14 * 90.f) / (1<<14),
                nodes[pos].dist_mm_q2/1000.f/(1<<2),
                nodes[pos].quality >> SL_LIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);
        }
      }
    }
  fclose(fp);

  
	disconnectLidar(lidar);

  return 0;
}
