/*test SPI*/
/* Pour exécuter : gcc -o main CAN.c -lpigpio -lrt -lpthread -lm */
#include <stdio.h>
#include <stdlib.h>
//#include "pigpio.h"
#include <unistd.h>
#include "CAN.h"



void ledcan_on(){  
    system("cansend can0 708#1eff40");
    sleep(1);
    return;
}
void ledcan_off(){  
    system("cansend can0 708#1eff00");
    sleep(1);
    return;
}

/*
void ledcan_read(){  
    //system("candump can0");
    //sleep(1);

    FILE *pipe = popen("candump can0", "r");
    if (pipe == NULL) {
        fprintf(stderr, "Failed to open pipe for command execution.\n");
        return 1;}
    char buf[256];
    // Read the output line by line and print it
    while (fgets(buf, 256, pipe) != NULL) {
        printf("Received CAN message: %s", buf);
    }
    pclose(pipe);

    return;
}
*/

int main(void){
    ledcan_on();
    sleep(5);
    ledcan_off();
    //ledcan_read();
    return 0;
}