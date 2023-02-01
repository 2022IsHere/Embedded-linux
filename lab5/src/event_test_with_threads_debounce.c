/*
 * event_test.c
 *
 *  Created on: Feb 14, 2022
 *      Author: jarno
 */


#if 0

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif


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


	i=0;

	printf("Let's go luteet, says gpio thread\n");


	while (true) {


		ret = gpiod_line_event_wait(actual_args->line, &actual_args->ts);

		if (ret < 0) {
			perror("Wait event notification failed\n");
			ret = -1;
			return (void *)(ret);
		} else if (ret == 0) {
			//printf("Wait event notification on line #%u timeout\n", actual_args->line_num);
			printf("*");
			fflush(stdout);
			continue;
		}

		ret = gpiod_line_event_read(actual_args->line, &event);

		//printf("event: %s timestamp: [%8ld.%09ld]\n", event.event_type, event.ts.tv_sec, event.ts.tv_nsec);
		printf("\nevent #%d time stamp: %8ld.%09ld secs\n",i,(long int)event.ts.tv_sec,(long int)event.ts.tv_nsec);
		if (ret < 0) {
			perror("Read last event notification failed\n");
			ret = -1;
			return (void *)(ret);
		}

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
#endif
