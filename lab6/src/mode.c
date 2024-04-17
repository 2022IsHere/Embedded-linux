#include <gpiod.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h> 

#include <unistd.h> // remove when printfs

#define DIMMER_LED_MODE  1
#define SERVO_CONTROL_MODE 0

typedef struct {
    struct timespec wait;
    struct gpiod_line *line;
    volatile int *flag;
} button_monitor;

/*
*@brief Function to control the system mode 
*@desc Toggle mode between DIMMER_LED_MODE and SERVO_CONTROL_MODE
*/
void change_mode(int button_status, struct gpiod_line *system_mode_led_line) {

    // Cast system mode static to remember it between function calls
    static int current_mode = DIMMER_LED_MODE;

    // Check if button pressed
    if (!button_status) {

        // Change system mode
        if (current_mode == DIMMER_LED_MODE) {
            current_mode = SERVO_CONTROL_MODE;
            gpiod_line_set_value(system_mode_led_line, SERVO_CONTROL_MODE);
        } else {
            current_mode = DIMMER_LED_MODE;
            gpiod_line_set_value(system_mode_led_line, DIMMER_LED_MODE);
        }
    }
}

/*
*@brief Function to return a flag to control encoder
*@desc Return 1 until button press is detected and return 0
*@detail Function freezes encoder till system mode is changed
*/
void *button_monitor_function(button_monitor *button) {

    int temp = &button;

    printf("Memory address of the button struct: %p", temp);

    printf("\n\nAbout to detect button presses!\n\n");

    while(true) {

        if (gpiod_line_get_value(button->line) == 0) {
            button->flag = 0;
            usleep(200e3);

            printf("Flag value: %d\n\n", button->flag);
        }
    }
    return NULL;
}

