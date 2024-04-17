#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define MIN_ENCODER_VALUE 0
#define MAX_ENCODER_VALUE 100

// Function to set servo position based on encoder value
void set_servo_position(int encoder_value) {
    // Map the encoder value to the servo position (assuming 0-180 degree range)
    int servo_position = (encoder_value * 180) / MAX_ENCODER_VALUE;

    // Ensure servo position is within the valid range
    servo_position = (servo_position < 0) ? 0 : servo_position;
    servo_position = (servo_position > 180) ? 180 : servo_position;

    // Send the servo position command to your servo motor (implementation needed)
    // Example: set_servo_position_command(servo_position);
    printf("Servo position: %d degrees\n", servo_position);
}