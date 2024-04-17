#ifndef MODE_H
#define MODE_H

#include <gpiod.h>

#define DIMMER_LED_MODE  1
#define SERVO_CONTROL_MODE 0

#define wait_btn_sec 0
#define wait_btn_ns 25000000

typedef struct {
    struct timespec wait;
    struct gpiod_line *line;
    volatile int *flag;
} button_monitor;

void change_mode(int button_status, struct gpiod_line *system_mode_led_line);

void *button_monitor_function(void *args);

#endif