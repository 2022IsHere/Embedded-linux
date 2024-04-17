#include <stdio.h>
#include <unistd.h>

#define DIMMER_LED_PIN 24

#define SYSTEM_LED_PIN 	23

#define LED_ON 1
#define LED_OFF 0

#define PWM_PERIOD_NS 20000000  // 20 ms

#define MIN_DUTY_CYCLE_NS 500000   // 0.5 ms
#define MAX_DUTY_CYCLE_NS 1950000  // 1.95 ms

#define DUTY_CYCLE_FILE "/sys/class/pwm/pwmchip0/pwm0/duty_cycle"

/*
*@brief Function to map encoder value to duty cycle in nanoseconds
*desc Assign MIN and MAX values for Duty Cycle
*@detail check duty cycle and set to be within specifies range at all times
*/
int map_encoder_value_to_duty_cycle(int encoder_value) {

    // Calculate the step size for each encoder value (1.45 ms increment)
    int step_size = (MAX_DUTY_CYCLE_NS - MIN_DUTY_CYCLE_NS) / 100;

    // Map the encoder value to the duty cycle in nanoseconds
    int duty_ns = MIN_DUTY_CYCLE_NS + encoder_value * step_size;

    // Ensure duty cycle is within the defined range
    duty_ns = (duty_ns < MIN_DUTY_CYCLE_NS) ? MIN_DUTY_CYCLE_NS : duty_ns;
    duty_ns = (duty_ns > MAX_DUTY_CYCLE_NS) ? MAX_DUTY_CYCLE_NS : duty_ns;

    printf("Encoder value: %d, Duty cycle in ns: %d\n\n", encoder_value, duty_ns);

    return duty_ns;
}

/*
*@brief Function updates hardware duty cycle
*@detail return 0 on success and -1 on failure
*/
int set_pwm_duty_cycle(int duty_cycle) {
    FILE *dutyCycleFile = fopen(DUTY_CYCLE_FILE, "w");
    if (!dutyCycleFile) {
        perror("Failed to open duty cycle file for writing!\n");
        return -1;
    }

    // Set the duty cycle
    if (fprintf(dutyCycleFile, "%d", duty_cycle) < 0) {
        perror("Failed to set duty cycle!\n");
        fclose(dutyCycleFile);
        return -1;
    }

    if (fclose(dutyCycleFile) != 0) {
        perror("Failed to close dutyCycleFile!\n");
        return -1;
    }

    return 0;
}