#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pigpio.h>

// gcc -o Hex HexToBi.c -lpigpio -lrt -lpthread -lm

// SPI param
#define SPI_CHANNEL 1
#define SPI_SPEED   500000 // Vitesse de transmission en Hz

int spiHandle;

// Fonction toute faite 
int convertToDecimal(unsigned char *dataList, int dataSize) {
    int decimalValue = 0;

    // Gestion du bit de signe (MSB) du premier octet
    int sign = (dataList[0] & 0x80) ? -1 : 1;

    // Conversion de la liste en complément à deux en décimal
    for (int i = 0; i < dataSize; ++i) {
        decimalValue <<= 8; // Décalage de 8 bits vers la gauche
        decimalValue += dataList[i];
    }

    // Appliquer le signe
    return decimalValue * sign;
}

void readEncoder(int *current_speed) {

    char txData[] = {0x01,0x00, 0x00, 0x00, 0x00};
    char rxData[5];

    spiXfer(spiHandle, txData, rxData, sizeof(txData));

    int dataSize = sizeof(rxData) / sizeof(rxData[0]);
    *current_speed = convertToDecimal(rxData, dataSize);
    printf("%d\n", *current_speed);
}

int main(void) {

    if (gpioInitialise() < 0) {
        fprintf(stderr, "Échec de l'initialisation de pigpio.\n");
        exit(1);
    }

    spiHandle = spiOpen(SPI_CHANNEL, SPI_SPEED, 0);
    if (spiHandle < 0)
    {
        fprintf(stderr, "Impossible d'ouvrir le canal SPI\n");
        gpioTerminate();
        return 1;
    }

    int current_speed;
    for (int i = 0; i < 1000; i++) {
        readEncoder(&current_speed);
        sleep(1);
    }
    

    return 0;
}