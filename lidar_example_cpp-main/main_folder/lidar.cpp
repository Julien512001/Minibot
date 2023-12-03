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

void myScanData(ILidarDriver* lidar, myGrabData* myData, float maxDistance, FILE* fp) {

  int rotationCount = 1;
  int i = 0;
  size_t nodeCount = 1000;
  myData->size = (int) nodeCount;
  
  sl_result res;
  while(i < rotationCount) {
    sl_lidar_response_measurement_node_hq_t nodes[nodeCount];
    //size_t nodeCount = sizeof(nodes)/sizeof(sl_lidar_response_measurement_node_hq_t);
    res = lidar->grabScanDataHq(nodes, nodeCount);
    i++;
    if (SL_IS_OK(res)) {
      lidar->ascendScanData(nodes, nodeCount);
      for (int pos = 0; pos < (int)nodeCount ; ++pos) {
          /*
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
          */
          myData[pos].angle =    (nodes[pos].angle_z_q14 * 90.f) / (1<<14);
          myData[pos].distance = (nodes[pos].dist_mm_q2/1000.f/ (1<<2) < maxDistance) ? nodes[pos].dist_mm_q2/1000.f/ (1<<2) : 0.0;

          //printf("angle : %f, distance : %f\n", data[pos].angle, data[pos].distance);

          fprintf(fp, "angle : %f, distance : %f\n", myData[pos].angle, myData[pos].distance);
        }
      }
    }
    


}

void makeCluster(myGrabData* myData) {

    float threshold_angle = 0.1;
    float threshold_distance = 1.0;
    int ClustNumber = 1;

    for (int i = 2; i < (int) myData->size; i++) {
        if (myData[i].distance == 0.0) {
            myData[i].clust = 0;
        }
        else if (myData[i].distance != 0.0) {
            
            myData[i].clust = ClustNumber;
            /*
            if ( (myData[i+1].angle - myData[i].angle) <  threshold_angle && (myData[i+1].distance - myData[i].distance) < threshold_distance ) {
              myData[i+1].clust = ClustNumber;
            }
            */


        }




    }
}