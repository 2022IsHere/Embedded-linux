#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif

// Define constants for LED timing
#define LED_ON_TIME 10
#define LED_OFF_TIME 90

// Function to update button presses to log file
void updateFile(int duration) {
	FILE *file = NULL;
	file = fopen("lab3.log","a");
	if (file == NULL) {
		printf("There was an issue opening the file...\n");
		exit(1);
	}
	fprintf(file, "%d\n", duration);
	fclose(file);
}

// Function to initialize the timer
void init_timer(struct timeval *timer) {
    gettimeofday(timer, NULL);
}
// Function to get the current time in milliseconds
int current_time_millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
// Function to check if the timer has elapsed
int is_timer_elapsed(struct timeval *timer, int duration_ms) {

	// check is timer even on to prevent faulty true returns
	if (timer->tv_sec == 0 && timer->tv_usec == 0) {
        return false;  // Timer not started
    }

	// run this only with adequate timer
    int current_time = current_time_millis();	// Get current time
    int elapsed_time = (current_time -
	(timer->tv_sec * 1000 + timer->tv_usec / 1000)); // subtruct current - when button was pressed
	return elapsed_time >= duration_ms; // compare subtructed value to led inverval
}
// Function to reset the timer
void reset_timer(struct timeval *timer) {
    timer->tv_sec = 0;
    timer->tv_usec = 0;
}

int main()
{



	char *chipname = "gpiochip0";
	unsigned int led_line_num = 23;	// GPIO Pin #23
	unsigned int button_line_num = 22; // GPIO Pin #22
	unsigned int led_state = 0;	// determines state of the led, 0 at default
	unsigned int button_state = 1;
	struct gpiod_chip *chip;
	struct gpiod_line *led_line, *button_line;
	int led;
	int btn;

	// initialize variables to clock button press
	struct timeval press_start = {0};
	struct timeval press_end = {0};
	int btn_duration_ms = 0;

	// initialize variables for led duty cycle
	struct timeval led_on_timer = {0};
	struct timeval led_off_timer = {0};
	int total_duration_ms = 0;

	// Setup chip
	chip = gpiod_chip_open_by_name(chipname);
	if (!chip) {
		perror("Open chip failed\n");
		goto end;
	}

	// Setup GPIO line for LED
	led_line = gpiod_chip_get_line(chip, led_line_num);
	if (!led_line) {
        perror("Get led line failed\n");
        goto close_chip;
    }

	// Setup GPIO line for button
	button_line = gpiod_chip_get_line(chip, button_line_num);
	if (!button_line) {
        perror("Get button line failed\n");
        goto release_button_line;
    }

	// Check GPIO PIN 22 for input configuration. By default input has internal resistor
	btn = gpiod_line_request_input(button_line, CONSUMER);
	if (btn < 0) {
		perror("Request button line as input failed\n");
		goto release_button_line;
	}

	// Check GPIO PIN 23 for output configuration
	led = gpiod_line_request_output(led_line, CONSUMER, 0);
	if (led < 0) {
		perror("Request led line as output failed\n");
		goto release_line;
	}
	int num = 0;
	printf("\nInitializing program....\n");
	sleep(3);
	printf("Start blinking now!\n\n");
	while(1) {
		num++;
		// Fetch button value from GPIO PIN 22
    	button_state = gpiod_line_get_value(button_line);

		// Refresh system
		usleep(1000);
		printf("Iteration %u-th\n", num);
		printf("Button state is %u\n", button_state);
		printf("Led state is %u\n\n", led_state);

		if (button_state == 0) {
			if (press_start.tv_sec == 0) {

				// Turn LED on		
				led_state = !button_state;
				led = gpiod_line_set_value(led_line, led_state);

				// capture the time led was turned on
				init_timer(&press_start);
				init_timer(&led_on_timer); // Initialize the LED ON timer
				reset_timer(&led_off_timer);  // Reset the LED OFF timer
				printf("Time since epoch when button is first pressed %ld\n", (press_start.tv_sec*1000+press_start.tv_usec/1000));
				printf("Time since epoch led_on_timer: %ld\n", (led_on_timer.tv_sec*1000+led_on_timer.tv_usec/1000));
				printf("Time since epoch led_off_timer: %ld\n\n", (led_off_timer.tv_sec*1000+led_off_timer.tv_usec/1000));
			}

			printf("led_on_timer has elapsed true or false: %d\n", is_timer_elapsed(&led_on_timer, LED_ON_TIME));
			printf("Elapsed led_on_time is: %ld\n\n", (current_time_millis() - (led_on_timer.tv_sec * 1000 + led_on_timer.tv_usec / 1000)));

			// Check if LED ON timer is up
			if(is_timer_elapsed(&led_on_timer, LED_ON_TIME)) {

				// Turn led off
				led_state = 0; 
				led = gpiod_line_set_value(led_line, led_state);

				printf("Led is turned off because led_on_timer has elapsed. Led is: %d\n\n", led_state);

				// reset LED ON timer
				reset_timer(&led_on_timer);
				init_timer(&led_off_timer);

				printf("led_on_timer is reset to: %ld\n",(led_on_timer.tv_sec * 1000 + led_on_timer.tv_usec/1000));
				printf("led_off_timer is initialised to: %ld\n\n", (led_off_timer.tv_sec*1000+led_off_timer.tv_usec/1000));
			}
			// Check if LED OFF timer us up
			if(is_timer_elapsed(&led_off_timer, LED_OFF_TIME)) {

				// Turn led on
				led_state = 1; 
				printf("led state %d\n", led_state);
				led = gpiod_line_set_value(led_line, 1);

				// reset LED OFF timer
				reset_timer(&led_off_timer);
				init_timer(&led_on_timer);

				printf("led_off_timer is  reset to: %lu\n",(led_off_timer.tv_sec*1000+led_off_timer.tv_usec/1000));
				printf("led_on_timer is initialised to: %lu\n\n",(led_on_timer.tv_sec*1000+led_on_timer.tv_usec/1000));
			}
		}

		// Button is released
		else {

			// Check moment button was released, calculate substruction, append to log
			if(press_start.tv_sec != 0) {
				init_timer(&press_end); 
				btn_duration_ms = (press_end.tv_sec - press_start.tv_sec) * 1000 +
                   (press_end.tv_usec - press_start.tv_usec) / 1000;
				updateFile(btn_duration_ms);

				// reset all timers
				reset_timer(&led_on_timer);
				reset_timer(&led_off_timer);
				reset_timer(&press_start);
				reset_timer(&press_end);

				// Turn LED off
				led_state = !button_state;
				led = gpiod_line_set_value(led_line, led_state);
			}
		} 
	}
release_button_line:
    gpiod_line_release(button_line);
release_line:
	gpiod_line_release(led_line);
close_chip:
	gpiod_chip_close(chip);
end:
	return 0;
}  
