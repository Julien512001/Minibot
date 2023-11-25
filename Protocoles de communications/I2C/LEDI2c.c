#include <stdio.h>
#include <unistd.h>
#include <pigpio.h>

#define I2C_ADR 0x08


// gcc -o   LEDI2c LEDI2c.c -lpigpio
// sudo ./LEDI2c 

int I2C(){

    if (gpioInitialise() < 0) {
        fprintf(stderr, "Unable to initialize gpio\n");
        return 2;
    }

    int handle = i2cOpen(1, I2C_ADR, 0);

    if (handle < 0) {
        fprintf(stderr, "Unable to open I2C\n");
        return 2;
    }

    for(int i = 0; i < 500; i++){
        // Turn on the LED
        i2cWriteByte(handle, 0x01);
        int byteRead = i2cReadByte(handle);
        printf("Response = %d", byteRead);

        // Wait for a while
        time_sleep(0.1);

        // Turn off the LED
        i2cWriteByte(handle, 0x00);
        printf("Response = %d", byteRead);
        time_sleep(0.1);
    }

    
    i2cWriteByte(handle, 0x00);
    i2cClose(handle);
 
}
int main() {
    
    I2C();

    return 0;
}
