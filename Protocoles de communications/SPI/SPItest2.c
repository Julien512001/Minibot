#include <stdio.h>
#include <pigpio.h>

#define SPI_CHANNEL 1
#define SPI_SPEED   1000000 // Vitesse de transmission en Hz
#define SPI_FLAGS   0        // Flags pour le transfert SPI

int main()
{
    if (gpioInitialise() < 0)
    {
        fprintf(stderr, "Impossible d'initialiser pigpio\n");
        return 1;
    }

    int spiHandle = spiOpen(SPI_CHANNEL, SPI_SPEED, SPI_FLAGS);

    if (spiHandle < 0)
    {
        fprintf(stderr, "Impossible d'ouvrir le canal SPI\n");
        gpioTerminate();
        return 1;
    }
    // Données à envoyer
    char txData[] = {0x17, 0x00, 0x00, 0x00, 0x00};
    // Tampon pour stocker les données reçues
    char rxData[5];

    spiXfer(spiHandle, txData, rxData, sizeof(txData));

    // Affichage des données reçues
    printf("Données lues depuis le SPI: ");
    for (int i = 0; i < sizeof(rxData); ++i)
    {
        printf("0x%02X ", (unsigned char)rxData[i]);
    }
    printf("\n");

    // Fermeture du canal SPI
    spiClose(spiHandle);

    // Nettoyage de pigpio
    gpioTerminate();


    return 0;

}