/*
 * event_test.c
 *
 *  Created on: Feb 14, 2022
 *      Author: jarno
 */


#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif
/*
	****Debouncing software****
   Idea is to save the moment falling edge is detected. 
   Next time falling edge is detected, 
   compare previous and new moment. 
   In case result < 200 ms, 
   discard as bouncing of the button. 
   Otherwise, set button press to correct gpio line!
*/

#include <sys/time.h> 

#define LOW 0
#define HIGH 1

int last_interrupt_time = 0; 

// Function to save specific moment in time since epoch
int interrupt_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

// Function to handle ISR
void interrupt_service_rutine(struct gpiod_line *lineButton) {
    int current_interrupt_time = interrupt_time();
	printf("current time - previous time: %d ms\n", (current_interrupt_time-last_interrupt_time));
    if (current_interrupt_time - last_interrupt_time > 200) {

		gpiod_line_set_value(lineButton,LOW);
		printf("Button press is confirmed!\n");

		last_interrupt_time = 0;
    }
    last_interrupt_time = current_interrupt_time;
}

// Function to detect falling edge
void is_falling_edge(struct gpiod_line_event *event, struct gpiod_line *lineButton) {

	printf("Event type is: %d\n", event->event_type);
    if (event->event_type == 1) {

		printf("is_falling_edge returned true!\n");
		// In case falling edge, run ISR to handle possible bouncing
        interrupt_service_rutine(lineButton);
    }
}

// Let's define the pins here
#define BUTTON_PIN 	22
#define RED_PIN 	23
#define YELLOW_PIN	24
#define GREEN_PIN	25



typedef struct {
	unsigned int line_num;
    struct timespec ts;
    struct gpiod_line *line;
} gpio_stuff_t;

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


int main(int argc, char **argv)
{

	pthread_t gpio_thread;

	struct gpiod_chip *chip;
	struct gpiod_line *lineButton;	// the push button
	struct gpiod_line *lineRed;    // Red LED
	struct gpiod_line *lineGreen;  // Green LED
	struct gpiod_line *lineYellow; // Yellow LED

	char *chipname = "gpiochip0";


	int i, ret;

	struct timespec ts = { 5, 0 }; //we will wait 5sec and then give up


	chip = gpiod_chip_open_by_name(chipname);
	if (!chip) {
		perror("Open chip failed\n");
		ret = -1;
		goto end;
	}

	lineButton = gpiod_chip_get_line(chip, BUTTON_PIN);
	if (!lineButton) {
		perror("Get line failed\n");
		ret = -1;
		goto close_chip;
	}

	ret = gpiod_line_request_rising_edge_events(lineButton, CONSUMER);
	if (ret < 0) {
		perror("Request event notification failed\n");
		ret = -1;
		goto release_line;
	}

	// Open more GPIO lines, skip error checking...
	lineRed = gpiod_chip_get_line(chip, RED_PIN);
	lineYellow = gpiod_chip_get_line(chip, YELLOW_PIN);
	lineGreen = gpiod_chip_get_line(chip, GREEN_PIN);

	// Configure LED lines as outputs, default to logic LOW (off)
	gpiod_line_request_output(lineRed, "example1", 0);
	gpiod_line_request_output(lineGreen, "example1", 0);
	gpiod_line_request_output(lineYellow, "example1", 0);

	//pthread_create accepts only one argument to be passed to the function,
	//so let's create a struct and provide a pointer to it
	gpio_stuff_t gpio_stuff;

	gpio_stuff.line = lineButton;
	gpio_stuff.line_num = BUTTON_PIN;
	gpio_stuff.ts = ts;


	ret = pthread_create( &gpio_thread, NULL, gpio_function, &gpio_stuff);


	/* The eternal main loop */
	i = 0;

	printf("Entering main loop.\n");
	while (true) {

		//blink some leds. Note how we can pick "bits" from a counter i and make them toggle
		gpiod_line_set_value(lineRed, (i & 1) != 0);
		gpiod_line_set_value(lineYellow, (i & 2) != 0);
		gpiod_line_set_value(lineGreen, (i & 4) != 0);

		usleep(200e3); //200msecs

		i++;

	}

	ret = 0;

release_line: //generally.... don't use use "goto"s. Error handlers are the only justified place
	gpiod_line_release(lineRed);
	gpiod_line_release(lineYellow);
	gpiod_line_release(lineGreen);
	gpiod_line_release(lineButton);

close_chip:
	gpiod_chip_close(chip);

end:
	return ret;
}
