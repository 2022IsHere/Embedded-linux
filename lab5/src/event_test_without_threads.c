/*
 * event_test_without_threads.c
 *
 *  Created on: Feb 14, 2022
 *      Author: jarno
 */

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif


// Let's define the pins here
#define BUTTON_PIN 	22
#define RED_PIN 	23
#define YELLOW_PIN	24
#define GREEN_PIN	25

int main(int argc, char **argv)
{



	struct gpiod_chip *chip;
	struct gpiod_line *lineButton;	// the push button
	struct gpiod_line *lineRed;    // Red LED
	struct gpiod_line *lineGreen;  // Green LED
	struct gpiod_line *lineYellow; // Yellow LED

	struct gpiod_line_event event; //this represents the button events

	char *chipname = "gpiochip0";


	int i, counter, ret;

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


	// Configure LED lines as outputs
	gpiod_line_request_output(lineRed, "example1", 0);
	gpiod_line_request_output(lineGreen, "example1", 0);
	gpiod_line_request_output(lineYellow, "example1", 0);



	
	counter = 0;
	while (true) {

		//blink some leds
		gpiod_line_set_value(lineRed, (counter & 1) != 0);
		gpiod_line_set_value(lineYellow, (counter & 2) != 0);
		gpiod_line_set_value(lineGreen, (counter & 4) != 0);

		counter++;


		//Then, let's wait for the events
		printf("gpiod_line_event_wait is blocking everything for a long time!...");
		fflush(stdout); //since we don't have \n at the end...   setbuf(stdout, NULL); would also do the trick
		ret = gpiod_line_event_wait(lineButton, &ts);
		printf("... now continuing\n");


		if (ret < 0) {
			perror("Wait event notification failed\n");
			ret = -1;
			goto release_line;
		} else if (ret == 0) {
			printf("Wait event notification on line #%u timeout\n", BUTTON_PIN);
			continue;
		}

		ret = gpiod_line_event_read(lineButton, &event);

		printf("Get event notification on line #%u %d times\n", BUTTON_PIN, i);
		if (ret < 0) {
			perror("Read last event notification failed\n");
			ret = -1;
			goto release_line;
		}

		usleep(200e3); //200msecs

		//Note: we get here only if there were events
		i++;


	}

	printf("All done\n");

	ret = 0;

release_line:
	gpiod_line_release(lineRed);
	gpiod_line_release(lineYellow);
	gpiod_line_release(lineGreen);
	gpiod_line_release(lineButton);

close_chip:
	gpiod_chip_close(chip);
end:
	return ret;
}

