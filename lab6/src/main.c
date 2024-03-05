/*
	****Two-way microcontroller****
	The software part will use:
	A GPIO pin for led. 
	A GPIO pin for button.
	A GPIO pin for dimmer led. 
	A GPIO pin for servo motor. 

	First led will be 1 and 0 depending on the system state. 
	The state is determined based on button press. 
	Button Will be active low so button press equals to 0 and 1 to not pressed. 
	Led is on when button is pressed and vice versa.

	Next, Based on button, either servo motor is controlled with potentiemer or dimmer led. 
	When potentiemeter is turned, servo motor turns as well and the other state then controls
	the dimmer led in same manner. 
*/

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h> 

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif

#define LOW 0
#define HIGH 1
#define BOUNCING 20

int last_interrupt_time = -1; 

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
    if (current_interrupt_time - last_interrupt_time > BOUNCING || last_interrupt_time == -1) {

		gpiod_line_set_value(lineButton,LOW);
		printf("Button press is confirmed!\n");

    }
    last_interrupt_time = current_interrupt_time;
}

// Function to detect falling edge
void is_falling_edge(struct gpiod_line_event *event, struct gpiod_line *lineButton) {

    if (event->event_type == 1) {

		// In case falling edge, run ISR to handle possible bouncing
        interrupt_service_rutine(lineButton);
    }
}

// Let's define the pins here
#define BUTTON_PIN 	22
#define RED_PIN 	23

// pthread_create accepts only one argument to be passed to the function,
// so let's create a struct
typedef struct {
	unsigned int lineNum;
    struct timespec ts;
    struct gpiod_line *line;
} gpio_stuff_t;

void *gpio_function(void *args) {
    int i, ret;
    struct gpiod_line_event event;

    gpio_stuff_t *actual_args = args;

    i = 1;

    printf("GPIO thread here!\n");

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

		printf("Get event notification on line #%u %d times\n", actual_args->lineNum, i);
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

int main (int argc, char **argv) {

	pthread_t gpio_thread;

	struct gpiod_chip *chip;
	struct gpiod_line *lineButton;	// the push button
	struct gpiod_line *lineRed;    // Red LED

	char *chipname = "gpiochip0";

	int ret;
	int led;
	int btn;

	struct timespec ts = { 5, 0 }; // 5 s wait period

	chip = gpiod_chip_open_by_name(chipname);
	if (!chip) {
		perror("Open chip failed\n");
		ret = -1;
		goto end;
	}
	// Setup GPIO line for System Status i.e. LED
	lineRed = gpiod_chip_get_line(chip, RED_PIN);
	if (!lineRed) {
        perror("Get gpio line for red led failed\n");
        goto release_line;
    }
	// Setup GPIO line for System Status control i.e. BUTTON
	lineButton = gpiod_chip_get_line(chip, BUTTON_PIN);
	if (!lineButton) {
		perror("Get gpio line for button failed\n");
		goto release_line;
	}
	// Setup BUTTON_PIN as input
	btn = gpiod_line_request_input(lineButton, CONSUMER);
	if (btn < 0) {
		perror("Request button line as input failed\n");
		goto release_line;
	}
	// Setup RED_PIN as output
	led = gpiod_line_request_output(lineRed, CONSUMER, 0);
	if (led < 0) {
		perror("Request led line as output failed\n");
		goto release_line;
	}
	ret = gpiod_line_request_rising_edge_events(lineButton, CONSUMER);
	if (ret < 0) {
		perror("Request event notification failed\n");
		ret = -1;
		goto release_line;
	}

	gpio_stuff_t gpio_stuff;

	gpio_stuff.line = lineButton;
	gpio_stuff.lineNum = BUTTON_PIN;
	gpio_stuff.ts = ts;

	ret = pthread_create( &gpio_thread, NULL, gpio_function, &gpio_stuff);

	while(true) {
		
	}

	ret = 0;

release_line:
	gpiod_line_release(lineRed);
	gpiod_line_release(lineButton);
close_chip:
	gpiod_chip_close(chip);
end:
	return ret;
}
