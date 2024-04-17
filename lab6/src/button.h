#ifndef BUTTON_H
#define BUTTON_H

#include <gpiod.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h> 

#define BUTTON_PIN 	22

#define LOW 0
#define HIGH 1
#define BOUNCING 20

#define wait_for_sec 5
#define wait_for_ns 0

typedef struct {
	unsigned int line_num;
    struct timespec ts;
    struct gpiod_line *line;
} gpio_stuff_t;

extern int last_interrupt_time;

void is_falling_edge(struct gpiod_line_event *event, struct gpiod_line *lineButton);

void *gpio_function(void *args);

#endif