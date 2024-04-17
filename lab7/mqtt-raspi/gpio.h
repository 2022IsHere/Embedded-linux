#ifndef GPIO_HANDLER_H
#define GPIO_HANDLER_H

#include <stdio.h>
#include <pthread.h>
#include <gpiod.h>
#include <sys/time.h> 

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif

#define BUTTON_PIN 	22

#define LOW 0
#define HIGH 1
#define BOUNCING 20

#define WAIT_FOR_SEC 5
#define WAIT_FOR_NS 0

typedef struct {
	unsigned int line_num;
    struct timespec ts;
    struct gpiod_line *line;
} gpio_stuff_t;

extern int last_interrupt_time; 

int interrupt_time();
void interrupt_service_routine(struct gpiod_line *lineButton);
void is_falling_edge(struct gpiod_line_event *event, struct gpiod_line *lineButton);
void *gpio_function(void *args);

#endif /* GPIO_HANDLER_H */
