/*
	****Two-way microcontroller****
	Idea is to change system between two states with a button.
	One state will saturate led brightness between 0 and 100% 
	and the other state will saturate servo motor between 0 and 100%.

	Constraint: Button is set to 0 when button line has no event detected for a set period of time, ts.
	Meaning that each button press will run alternative program path for a set period of time ts.
*/


#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h> 

#include "pwm_with_hw.h"
#include "timers_init.h"

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif


#define LOW 0
#define HIGH 1
#define BOUNCING 20

 int last_interrupt_time = -1; 

 /*
*@brief Function to change system state
*
*@detail Return -1 when unsuccessfull change, otherwise 0
*/
int system_state(int button_state, struct gpiod_line *lineLed) {

	if (button_state == LOW) {
		 gpiod_line_set_value(lineLed, HIGH);
		// Run servo
		run_servo();
		return LOW;
	} else {
		 gpiod_line_set_value(lineLed, LOW);
		// Run Led dimmer
	}
	return HIGH;
}

/*
*@ brief Function to save specific moment in time since epoch
*/
int interrupt_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

/* 
*@ brief Function to handle ISR
*/
void interrupt_service_rutine(struct gpiod_line *lineButton, struct gpiod_line *lineLed) {
    int current_interrupt_time = interrupt_time();
	printf("current time - previous time: %d ms\n", (current_interrupt_time-last_interrupt_time));
    if (current_interrupt_time - last_interrupt_time > BOUNCING || last_interrupt_time == -1) {

		gpiod_line_set_value(lineButton,LOW);
		printf("Button press is confirmed!\n");

		system_state(LOW, lineLed);

    }
    last_interrupt_time = current_interrupt_time;
}

/* 
*@brief Function to detect falling edge
*/
void is_falling_edge(struct gpiod_line_event *event, struct gpiod_line *lineButton, struct gpiod_line *lineLed) {

    if (event->event_type == 1) {

		// In case falling edge, run ISR to handle possible bouncing
        interrupt_service_rutine(lineButton, lineLed);
    }
}


// Let's define the pins here
#define BUTTON_PIN 	22
#define LED_PIN 23

typedef struct {
	unsigned int line_num;
	unsigned int line_num_led;
    struct timespec ts;
    struct gpiod_line *line;
	struct gpiod_line *lineLed;

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
			system_state(HIGH, actual_args->lineLed);

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
        is_falling_edge(&event, actual_args->line, actual_args->lineLed);
	

        i++;
    }
}


int main(int argc, char **argv)
{

	pthread_t gpio_thread;

	struct gpiod_chip *chip;
	struct gpiod_line *lineButton;	// the push button
	struct gpiod_line *lineLed;

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

	lineLed = gpiod_chip_get_line(chip, LED_PIN);
	gpiod_line_request_output(lineLed, "example1", 0);

	//pthread_create accepts only one argument to be passed to the function,
	//so let's create a struct and provide a pointer to it
	gpio_stuff_t gpio_stuff;

	gpio_stuff.line = lineButton;
    gpio_stuff.lineLed = lineLed;
    gpio_stuff.line_num = BUTTON_PIN;
    gpio_stuff.line_num_led = LED_PIN;  // Add LED pin information
    gpio_stuff.ts = ts;


	ret = pthread_create( &gpio_thread, NULL, gpio_function, &gpio_stuff);


	/* The eternal main loop */
	i = 0;

	printf("Entering main loop.\n");
	while (true) {

		//usleep(200e3); //200msecs

		i++;

	}

	ret = 0;

release_line: //generally.... don't use use "goto"s. Error handlers are the only justified place
	gpiod_line_release(lineButton);
	gpiod_line_release(lineLed);
close_chip:
	gpiod_chip_close(chip);

end:
	return ret;
}
