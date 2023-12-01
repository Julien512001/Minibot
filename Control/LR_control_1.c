#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pigpio.h>

//This controller calculate the error of the speed and calculate a voltage between -12V and 12V

#define M_PI  3.14
#define TS 0.01
// gcc -o path LR_control_path.c -lpigpio -lrt -lpthread -lm

// SPI param
#define SPI_CHANNEL 1
#define SPI_SPEED   500000 // Vitesse de transmission en Hz

// Pin param
#define PWM_PIN_L 12    //PWM1 -> M1PWM
#define dL_PIN 5        //GPIO_5 -> M1DIR
#define PWM_PIN_R 13
#define dR_PIN 6
#define ENCODER_PIN 17  // Remplacez ceci par le bon numéro de GPIO pour votre encodeur

FILE* fp;

// PID param
#define Kp 0.39123
#define Ki 1.0038
#define Kd 0

long prevT = 0;
float previous_error_L = 0;
float eintegral_L = 0;

float previous_error_R = 0;
float eintegral_R = 0;

int spiHandle;

//int target_speed_L = -30;
//int target_speed_R = -30;

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
void readEncoder(float *current_speed_L, float *current_speed_R) {

    char txDataL[] = {0x0F,0x00, 0x00, 0x00, 0x00};
    char rxDataL[5];

    char txDataR[] = {0x01,0x00, 0x00, 0x00, 0x00};
    char rxDataR[5];

    spiXfer(spiHandle, txDataL, rxDataL, sizeof(txDataL));
    spiXfer(spiHandle, txDataR, rxDataR, sizeof(txDataR));

    int dataSizeL = sizeof(rxDataL) / sizeof(rxDataL[0]);
    int dataSizeR = sizeof(rxDataR) / sizeof(rxDataR[0]);
    int tick_L = (float) convertToDecimal(rxDataL, dataSizeL);
    int tick_R = (float) convertToDecimal(rxDataR, dataSizeR);
    float speed_L = tick_L/64.0 * 2.0*M_PI / TS;
    float speed_R = tick_R/64.0 * 2.0*M_PI / TS;
    *current_speed_L = speed_L;
    *current_speed_R = speed_R;
    fprintf(fp,"%f, %f\n", speed_L, speed_R);
    printf("%f, %d, %f, %d, ", speed_L, tick_L, speed_R, tick_R);
}

void Direction(float current_speed_L, float current_speed_right) {

    int dL;
    int dR;

    dL = (target_speed_L >= 0) ? 1:0; // Inversé car moteur gauche monté à l'envers
    dR = (target_speed_R >= 0) ? 0:1;

    gpioWrite(dL_PIN, dL);
    gpioWrite(dR_PIN, dR);
}

// code comme en arduino
void controlSpeed(float target_speed_L, float target_speed_R, float step) {
    int PWM_value_L;
    int PWM_value_R;
    float V_value_L;
    float V_value_R;

    float current_speed_L = 0.0;
    float current_speed_R = 0.0;

    float error_L, error_R, integral_L, integral_R, derivative_L, derivative_R;
    long currentTime, elapsedTime;

    readEncoder(&current_speed_L, &current_speed_R);

    error_L = target_speed_L - current_speed_L;
    error_R = target_speed_R - current_speed_R;
    printf("error_L = %f, error_R = %f, ", error_L, error_R);

    float P_L = error_L;
    float P_R = error_R;

    eintegral_L += error_L*step;
    float I_L = eintegral_L;

    eintegral_R += error_R*step;
    float I_R = eintegral_R;

    derivative_L = (error_L - previous_error_L)/step;
    float D_L = derivative_L;

    derivative_R = (error_R - previous_error_R)/step;
    float D_R = derivative_R;

    V_value_L = P_L * Kp + I_L * Ki + D_L * Kd;
    V_value_R = P_R * Kp + I_R * Ki + D_R * Kd;

    printf("%f, %f, ", V_value_L, V_value_R);

    if (V_value_L < -12) V_value_L = -12;
    else if (V_value_L > 12) V_value_L = 12;

    if (V_value_R < -12) V_value_R = -12;
    else if (V_value_R > 12) V_value_R = 12;

    Direction(V_value_L, V_value_R);

    printf("%f, %f\n", V_value_L, V_value_R);

    PWM_value_L = (int) abs(V_value_L)/12 * 1000;
    PWM_value_R = (int) abs(V_value_R)/12 * 1000;

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
    gpioSetMode(dL_PIN, PI_OUTPUT); 
    gpioSetMode(dR_PIN, PI_OUTPUT);
    usleep(10000);
}



void runMotors(int cycle, float step, float *target_speed_L, float *target_speed_R) {
    unsigned int start_time = gpioTick();
    unsigned int elapsed_time = 0;
    int i = 0;
    while (i < cycle) {

        controlSpeed(target_speed_L[i], target_speed_R[i], step);

        elapsed_time = (gpioTick() - start_time) / 1000;
        sleep(step);
        i++;
    }

    gpioPWM(PWM_PIN_L, 0);
    gpioPWM(PWM_PIN_R, 0);
}


int main() {
    
    if (gpioInitialise() < 0) {
        fprintf(stderr, "Échec de l'initialisation de pigpio.\n");
        exit(1);
    }
    int time = 30000;

    spiHandle = spiOpen(SPI_CHANNEL, SPI_SPEED, 0);
    if (spiHandle < 0)
    {
        fprintf(stderr, "Impossible d'ouvrir le canal SPI\n");
        gpioTerminate();
        return 1;
    }

    initPWM();

    fp = fopen("Speed.txt", "a");
    float *target_speed_L;
    float *target_speed_R;

    target_speed_L = malloc(time*sizeof(float));
    target_speed_R = malloc(time*sizeof(float));

    for (int i = 0; i < time; i++) {
        if (i < time/2) {
            target_speed_L[i] = 30.0;
            target_speed_R[i] = 30.0;
        } 
        else {
            target_speed_L[i] = -30.0;
            target_speed_R[i] = -30.0;
        }
    }


    float step = 0.05;

    runMotors(time, step, target_speed_L, target_speed_R);

    spiClose(spiHandle);
    gpioTerminate();
    fclose(fp);
    free(target_speed_L);
    free(target_speed_R);

    return 0;
}