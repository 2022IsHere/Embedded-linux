#include <stdio.h>
#include <stdlib.h>

#define EXPORT_FILE "/sys/class/pwm/pwmchip0/export"
#define PERIOD_FILE "/sys/class/pwm/pwmchip0/pwm0/period"
#define DUTY_CYCLE_FILE "/sys/class/pwm/pwmchip0/pwm0/duty_cycle"
#define ENABLE_FILE "/sys/class/pwm/pwmchip0/pwm0/enable"

/*
@brief Function to set up channel 0 pwm for led
@desc Default period is 20 ms in ns, and duty cycle is 0.5 ms in ns
@detail Return 0 with success and -1 with failure 
*/
int setUpPwm() {
    FILE *exportFile = fopen(EXPORT_FILE, "w");
    if (!exportFile) {
        perror("Failed to open export file for writing!\n");
        return -1;
    }

    // Access pwm0/pin18
    if (fprintf(exportFile, "0") < 0) {
        perror("Failed to access pwm0 (pin 18)!\n");
        fclose(exportFile);
        return -1;
    }

    if (fclose(exportFile) != 0) {
        perror("Failed to close exportFile!\n");
        return -1;
    }

    FILE *periodFile = fopen(PERIOD_FILE, "w");
    if (!periodFile) {
        perror("Failed to open period file for writing!\n");
        return -1;
    }

    // Set the period to 20,000,000 ns (20 ms)
    if (fprintf(periodFile, "20000000") < 0) {
        perror("Failed to set period for pin 18!\n");
        fclose(periodFile);
        return -1;
    }

    if (fclose(periodFile) != 0) {
        perror("Failed to close periodFile!\n");
        return -1;
    }

    FILE *dutyCycleFile = fopen(DUTY_CYCLE_FILE, "w");
    if (!dutyCycleFile) {
        perror("Failed to open duty cycle file for writing!\n");
        return -1;
    }

    // Set the duty cycle to 500,000 ns (0.5 ms)
    if (fprintf(dutyCycleFile, "500000") < 0) {
        perror("Failed to set duty cycle for pin 18!\n");
        fclose(dutyCycleFile);
        return -1;
    }

    if (fclose(dutyCycleFile) != 0) {
        perror("Failed to close dutyCycleFile!\n");
        return -1;
    }

    FILE *enableFile = fopen(ENABLE_FILE, "w");
    if (!enableFile) {
        perror("Failed to open enable file for writing!\n");
        return -1;
    }

    // Enable pwm0 output
    if (fprintf(enableFile, "1") < 0) {
        perror("Failed to enable pin 18!\n");
        fclose(enableFile);
        return -1;
    }

    if (fclose(enableFile) != 0) {
        perror("Failed to close enableFile!\n");
        return -1;
    }

    return 0;
}
