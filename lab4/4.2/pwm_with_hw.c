#include <stdio.h>
#include <stdlib.h>

#include "timers_init.h"

// 100 ms period in ns is 100 000 000.
// 50 ms duty cycle in ns is 50 000 000.

/* Return 0 with success and -1 with failure */
int setUP() {
    FILE *filename = NULL;
    filename = fopen("/boot/config.txt", "a");
    if (filename == NULL) {
        perror("Failing to open /boot/config.txt file!\n");
        return -1;
    }
    fprintf(filename, "dtoverlay=pwm-2chan\n");

    if (fclose(filename) != 0) {
        perror("Failing to close boot/config.txt file!\n");
        return -1;
    }
    
    fflush(filename);

    // access pin pwm0/pin18
    if (system("echo 0 > /sys/class/pwm/pwmchip0/export") != 0) {
        perror("Failed to execute command to enable pwm0!\n");
        return -1;
    }

    // access pin pwm0/pin19
    if (system("echo 1 > /sys/class/pwm/pwmchip0/export") != 0) {
        perror("Failed to execute command to enable pwm1!\n");
        return -1;
    }

    // create period and duty cycles
    if (system("echo 20000000 > /sys/class/pwm/pwmchip0/pwm0/period") != 0) {
        perror("Failed to set period for pin 18!\n");
        return -1;
    }

    if (system("echo 1500000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle") != 0) {
        perror("Failed to set duty cycle for pin 18!\n");
        return -1;
    }

    if (system("echo 1700000 > /sys/class/pwm/pwmchip0/pwm1/period") != 0) {
        perror("Failed to set period for pin 19!\n");
        return -1;
    }

    if (system("echo 1500000 > /sys/class/pwm/pwmchip0/pwm1/duty_cycle") != 0) {
        perror("Failed to set duty cycle for pin 19!\n");
        return -1;
    }

    if (system("echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable") != 0) {
        perror("Failed to enable pin 18!\n");
        return -1;
    }

    if (system("echo 1 > /sys/class/pwm/pwmchip0/pwm1/enable") != 0) {
        perror("Failed to enable pin 19!\n");
        return -1;
    }
    
    return 0;
}

int main() {
    /*int err_code = setUP();
    if (err_code != 0) {
        perror("Failed to set up PWM!\n");
        return -1;
    }
    */
    calculateSCurve();

    return 0;
}