#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include <string.h>  // Pour les fonctions de chaînes de caractères

//gcc -o UART UART.c -lpigpio


#define UART_TX_PIN 14
#define UART_RX_PIN 15
#define BAUD_RATE 9600
#define MAX_RESPONSE_SIZE 255  // Taille maximale de la réponse

int main() {
    if (gpioInitialise() < 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de pigpio\n");
        return 1;
    }

    if (gpioSetMode(UART_TX_PIN, PI_OUTPUT) < 0 || gpioSetMode(UART_RX_PIN, PI_INPUT) < 0) {
        fprintf(stderr, "Erreur lors de la configuration des broches UART\n");
        gpioTerminate();
        return 1;
    }

    if (gpioSetPullUpDown(UART_RX_PIN, PI_PUD_UP) < 0) {
        fprintf(stderr, "Erreur lors de la configuration de la résistance de tirage vers le haut sur la broche RX\n");
        gpioTerminate();
        return 1;
    }

    int uartHandle = serOpen("/dev/serial0", BAUD_RATE, 1);

    if (uartHandle < 0) {
        fprintf(stderr, "Erreur lors de l'ouverture du port UART\n");
        gpioTerminate();
        return 1;
    }

    // Instruction à envoyer
    char instruction[] = "Turn Led On";

    printf("Envoi de l'instruction: %s\n", instruction);

    //serWrite(uartHandle, instruction, sizeof(instruction)-1);
    int send;

    send = serWrite(uartHandle, instruction, sizeof(instruction));
    /*
    for (int i = 0; instruction[i] != '\0'; ++i) {
        serWriteByte(uartHandle, instruction[i]);
    }
    */

    // Attente de la fin de la transmission
    //serFlush(uartHandle);
    sleep(1);


        // Lecture de la réponse du périphérique
    char response[MAX_RESPONSE_SIZE];
    int read;
    read = serRead(uartHandle, response, sizeof(response));

    
    //printf("Response : %c", response);
    for (int i = 0; i < 255; i++) {
        printf("%c", response[i]);
    }

    serClose(uartHandle);
    gpioTerminate();

    return 0;
}