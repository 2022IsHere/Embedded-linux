#include <gpiod.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h> 

#define BUTTON_PIN 	22

#define LOW 0
#define HIGH 1
#define BOUNCING 20

typedef struct {
	unsigned int line_num;
    struct timespec ts;
    struct gpiod_line *line;
} gpio_stuff_t;

int last_interrupt_time = -1; 

/*
*@brief Function to save specific moment in time since epoch
*/
int interrupt_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

/*
*@brief Function to handle ISR
*/
void interrupt_service_rutine(struct gpiod_line *lineButton) {
    int current_interrupt_time = interrupt_time();
	printf("current time - previous time: %d ms\n", (current_interrupt_time-last_interrupt_time));
    if (current_interrupt_time - last_interrupt_time > BOUNCING || last_interrupt_time == -1) {

		gpiod_line_set_value(lineButton,LOW);
		printf("Button press is confirmed!\n");

    }
    last_interrupt_time = current_interrupt_time;
}

/*
*@brief Function to detect falling edge
*/
void is_falling_edge(struct gpiod_line_event *event, struct gpiod_line *lineButton) {

    if (event->event_type == 1) {

		// In case falling edge, run ISR to handle possible bouncing
        interrupt_service_rutine(lineButton);
    }
}

void *gpio_function(void *args) {
    int i, ret;
    struct gpiod_line_event event;

    gpio_stuff_t *actual_args = args;

    i = 1;

    printf("Let's go luteet, says gpio thread\n");

    while (true) {
        ret = gpiod_line_event_wait(actual_args->line, &actual_args->ts);

        if (ret < 0) {
            perror("Wait event notification failed\n");
            ret = -1;
            return (void *)(ret);
        } else if (ret == 0) {
            printf("Waiting period for event timed out!\n");
            fflush(stdout);
            continue;
        }

		// Read event on gpio line, 1 equals event, 0 equals none
		ret = gpiod_line_event_read(actual_args->line, &event);

		printf("Get event notification on line #%u %d times\n", actual_args->line_num, i);
		if (ret < 0) {
			perror("Read last event notification failed\n");
			ret = -1;
			return (void *)(ret);
		}

        // Check for falling edge and handle debouncing
        is_falling_edge(&event, actual_args->line);
	

        i++;
    }
}