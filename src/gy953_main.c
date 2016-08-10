#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "gy953_main.h"
#include "gy953_com.h"
#include <math.h>
/**
 * three axis euler angles 
 * In this project, we only use angle_z to check robot direction
 * */

// Please attention for mutex

/* x is alpha angle, roll angle, value from -PI to PI */
volatile static float angle_x = 0.0;
/* y is gamma angle, pitch angle, value from -1/2PI to 1/2PI */
volatile static float angle_y = 0.0;
/* z is beta angle, yaw angle, value from -PI to PI */
volatile static float angle_z = 0.0;

volatile static float acc_x = 0.0;
volatile static float acc_y = 0.0;
volatile static float acc_z = 0.0;
static float accOffset_x = 0;
static float accOffset_y = 0;
static float accOffset_z = 0;

volatile static float gyr_x = 0.0;
volatile static float gyr_y = 0.0;
volatile static float gyr_z = 0.0;
static float gyrOffset_x = 0.0;
static float gyrOffset_y = 0.0;
static float gyrOffset_z = 0.0;

// command arguments
static unsigned char eulerCommand[3];
static unsigned char accCommand[3];
static unsigned char gyrCommand[3];
static unsigned char magCommand[3]; 

void *gy953Thread() {
    int fd;
    float result[3];

    const char *port = "/dev/ttymxc1";
    /* unsigned char eulerCommand[3]; */

    fd = gy953Init(port);
    if (-1 == fd) {
        printf("GY953 init fault.\n");
        pthread_exit((void*)1);
    }
    gy953ConstructCommand(EULERANGLE, eulerCommand);
    gy953ConstructCommand(ACCELEROMETER, accCommand);
    gy953ConstructCommand(GYROSCOPE, gyrCommand);
    gy953ConstructCommand(MAGNETOMETER, magCommand);

    printf("GY953 init down. port is %s, fd = %d\n", port, fd);

    while(1) {
        /* getGY953Result(fd, 1, eulerCommand, result); */
        /* angle_x = angleResult[0]; */
        /* angle_y = angleResult[1]; */
        /* angle_z = angleResult[2]; */
        getGY953Result(fd, 2, accCommand, result);
        acc_x = result[0] - accOffset_x;
        acc_y = result[1] - accOffset_y;
        acc_z = result[2] - accOffset_z;

        
        /* offsetCheckAcc(result[0], result[1], result[2], &accOffset_x, &accOffset_y, &accOffset_z); */
        /* getGY953Result(fd, gyrCommand, result); */
        /* gyr_x = result[0] - gyrOffset_x; */
        /* gyr_y = result[1] - gyrOffset_y; */
        /* gyr_z = result[2] - gyrOffset_z; */
        /* showEulerAngle(); */
        showAcc();
        /* showGyr(); */
        sleep(1);
        /* usleep(10000);   // 10ms */
    }
    gy953Close(fd);
    return (void *)0;
}

float getYawAngle(void) {
    return angle_z;
}

void showEulerAngle(void) {
    printf("angle x: %.2f ", angle_x);
    printf("angle y: %.2f ", angle_y);
    printf("angle z: %.2f\n", angle_z);
}

void showAcc(void) {
    printf("acceleration x: %.0f ", acc_x);
    printf("acceleration y: %.0f ", acc_y);
    printf("acceleration z: %.0f\n", acc_z);
}

void showGyr(void) {
    printf("gyroscope x = %.2f ", gyr_x);
    printf("gyroscope y = %.2f ", gyr_y);
    printf("gyroscope z = %.2f\n", gyr_z);
}

int offsetCheckAcc(float accx, float accy, float accz, float *aveAccx, float *aveAccy, float *aveAccz) {
    static unsigned int addn = 0;
    static float addx = 0;
    static float addy = 0;
    static float addz = 0;

    addn ++;
    addx = addx + accx;
    addy = addy + accy;
    addz = addz + accz;
    if (addn < 200)
        printf("%d%% gy953 accelerate offset check was running, keep sensor stable and waiting...\n", addn/2);
    else {
        *aveAccx = addx / addn;
        *aveAccy = addy / addn;
        *aveAccz = addz / addn;
        printf("offsetAccx = %.2f ", *aveAccx);
        printf("offsetAccy = %.2f ", *aveAccy);
        printf("offsetAccz = %.2f\n", *aveAccz);
        return 0;
    }
    return -1;
}


