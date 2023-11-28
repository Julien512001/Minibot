#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pigpio.h>

// gcc -o LR LR_control.c -lpigpio -lrt -lpthread -lm

// SPI param
#define SPI_CHANNEL 1
#define SPI_SPEED   500000 // Vitesse de transmission en Hz

// Pin param
#define PWM_PIN_L 12    //PWM1 -> M1PWM
#define dL_PIN 5        //GPIO_5 -> M1DIR
#define PWM_PIN_R 13
#define dR_PIN 6
#define ENCODER_PIN 17  // Remplacez ceci par le bon numéro de GPIO pour votre encodeur

// PID param
#define Kp 2.0
#define Ki 0.05
#define Kd 0

long prevT = 0;
float previous_error_L = 0;
float eintegral_L = 0;

float previous_error_R = 0;
float eintegral_R = 0;

int spiHandle;

int target_speed_L = 30;
int target_speed_R = 32;

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
void readEncoder(int *current_speed_L, int *current_speed_R) {

    char txDataL[] = {0x0F,0x00, 0x00, 0x00, 0x00};
    char rxDataL[5];

    char txDataR[] = {0x01,0x00, 0x00, 0x00, 0x00};
    char rxDataR[5];

    spiXfer(spiHandle, txDataL, rxDataL, sizeof(txDataL));
    spiXfer(spiHandle, txDataR, rxDataR, sizeof(txDataR));

    int dataSizeL = sizeof(rxDataL) / sizeof(rxDataL[0]);
    int dataSizeR = sizeof(rxDataR) / sizeof(rxDataR[0]);
    *current_speed_L = abs(convertToDecimal(rxDataL, dataSizeL));
    *current_speed_R = abs(convertToDecimal(rxDataR, dataSizeR));
    printf("%d, %d\n", *current_speed_L, *current_speed_R);
}


// code comme en arduino
void controlSpeed() {
    int PWM_value_L;
    int PWM_value_R;

    int current_speed_L;
    int current_speed_R;

    float error_L, error_R, integral_L, integral_R, derivative_L, derivative_R;
    long currentTime, elapsedTime;

    readEncoder(&current_speed_L, &current_speed_R);

    error_L = target_speed_L - current_speed_L;
    error_R = target_speed_R - current_speed_R;

    float P_L = error_L;
    float P_R = error_R;

    eintegral_L += error_L;
    float I_L = eintegral_L;

    eintegral_R += error_R;
    float I_R = eintegral_R;

    derivative_L = (error_L - previous_error_L);
    float D_L = derivative_L;

    derivative_R = (error_R - previous_error_R);
    float D_R = derivative_R;

    PWM_value_L = P_L * Kp + I_L * Ki + D_L * Kd;
    PWM_value_R = P_R * Kp + I_R * Ki + D_R * Kd;


    if (PWM_value_L < 0) PWM_value_L = 0;
    else if (PWM_value_L > 1000) PWM_value_L = 1000;

    if (PWM_value_R < 0) PWM_value_R = 0;
    else if (PWM_value_R > 1000) PWM_value_R = 1000;

    gpioPWM(PWM_PIN_L, PWM_value_L);
    gpioPWM(PWM_PIN_R, PWM_value_R);

    previous_error_L = error_L;
    previous_error_R = error_R;
}

void initPWM() {
    gpioSetMode(PWM_PIN_L, PI_OUTPUT);
    gpioSetPWMfrequency(PWM_PIN_L, 25000); // Fréquence PWM : 25 kHz
    gpioSetPWMrange(PWM_PIN_L, 1000);      // Plage PWM : 0-1000 (pourcentage)
    gpioSetMode(PWM_PIN_R, PI_OUTPUT);
    gpioSetPWMfrequency(PWM_PIN_R, 25000); // Fréquence PWM : 25 kHz
    gpioSetPWMrange(PWM_PIN_R, 1000);      // Plage PWM : 0-1000 (pourcentage)
    
    usleep(10000);
}

void runMotors(int cycle) {
    unsigned int start_time = gpioTick();
    unsigned int elapsed_time = 0;

    while (elapsed_time < cycle) {
        controlSpeed();

        elapsed_time = (gpioTick() - start_time) / 1000;
        sleep(0.05);
    }

    gpioPWM(PWM_PIN_L, 0);
    gpioPWM(PWM_PIN_R, 0);
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
    gpioSetMode(dR_PIN, PI_OUTPUT);
    
    gpioWrite(dL_PIN, 1);
    gpioWrite(dR_PIN, 0);

    //scanf("Entrer une valeur de référence : %d\n", target_speed);
    //target_speed_L = 60;
    //target_speed_R = 60;
    runMotors(5000);

    spiClose(spiHandle);
    gpioTerminate();

    return 0;
}