#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>


#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif

void updateFile() {
    FILE *file = fopen("lab4.log", "a");
    if (file == NULL) {
        printf("There was an issue opening the file...\n");
        exit(1);
    }

    // Get timestamp
    time_t currentTime = time(NULL);
    struct tm date = *localtime(&currentTime);
    fprintf(file, "[MOTOR is turned ON: %d-%02d-%02d %02d:%02d]\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday,
            date.tm_hour, date.tm_min);

    fclose(file);
}

// Function to initialize the timer
void init_timer(struct timeval *timer) {
    gettimeofday(timer, NULL);
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

	// initialize variables for led duty cycle
	struct timeval led_on_timer = {0};

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


	// Fetch button value from GPIO PIN 22
    button_state = gpiod_line_get_value(button_line);

	// Check water level switch
	if (button_state == 0) {

		// Turn MOTOR on		
		led_state = !button_state;
		led = gpiod_line_set_value(led_line, led_state);

		init_timer(&press_start);

		// Log the time motor is turned on
		updateFile(); 

		// keep motor running for 10 minutes by pausing the program
		sleep(600);
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
