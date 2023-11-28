#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pigpio.h>

// gcc -o leftmotor LeftMotorControl.c -lpigpio -lrt -lpthread -lm

// SPI param
#define SPI_CHANNEL 1
#define SPI_SPEED   500000 // Vitesse de transmission en Hz

// Pin param
#define PWM_PIN_L 12    //PWM1 -> M1PWM
#define dL_PIN 5        //GPIO_5 -> M1DIR
#define ENCODER_PIN 17  // Remplacez ceci par le bon numéro de GPIO pour votre encodeur

// PID param
#define Kp 2.0
#define Ki 0.02
#define Kd 0

long prevT = 0;
float previous_error = 0;
float eintegral = 0;

int spiHandle;

int target_speed = 61;


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


// Lecture de la valeur de l'encodeur gauche
void readEncoder(int *current_speed) {

    char txData[] = {0x0F,0x00, 0x00, 0x00, 0x00};
    char rxData[5];

    spiXfer(spiHandle, txData, rxData, sizeof(txData));

    int dataSize = sizeof(rxData) / sizeof(rxData[0]);
    *current_speed = abs(convertToDecimal(rxData, dataSize));
    printf("%d\n", *current_speed);
}


// code comme en arduino
void controlSpeed() {
    int PWM_value_L;
    int current_speed;
    float error, integral, derivative;
    long currentTime, elapsedTime;

    readEncoder(&current_speed);

    error = target_speed - current_speed;

    float P = error;

    eintegral += error;
    float I = eintegral;

    derivative = (error - previous_error);
    float D = derivative;

    PWM_value_L = P * Kp + I * Ki + D * Kd;

    if (PWM_value_L < 0) PWM_value_L = 0;
    else if (PWM_value_L > 1000) PWM_value_L = 1000;

    gpioPWM(PWM_PIN_L, PWM_value_L);

    previous_error = error;
}

void initPWM() {
    gpioSetMode(PWM_PIN_L, PI_OUTPUT);
    gpioSetPWMfrequency(PWM_PIN_L, 25000); // Fréquence PWM : 25 kHz
    gpioSetPWMrange(PWM_PIN_L, 1000);      // Plage PWM : 0-1000 (pourcentage)
    usleep(10000);
}

void runMotors() {
    unsigned int start_time = gpioTick();
    unsigned int elapsed_time = 0;

    while (elapsed_time < 5000) {
        controlSpeed();

        elapsed_time = (gpioTick() - start_time) / 1000;
        sleep(0.05);
    }

    gpioPWM(PWM_PIN_L, 0);
}

int main() {
    
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

    initPWM();

    gpioSetMode(dL_PIN, PI_OUTPUT);

    gpioWrite(dL_PIN, 1);

    //scanf("Entrer une valeur de référence : %d\n", target_speed);

    runMotors();

    spiClose(spiHandle);
    gpioTerminate();

    return 0;
}