#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

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
    fprintf(file, "[LED ON: %d-%02d-%02d %02d:%02d]\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday,
            date.tm_hour, date.tm_min);

    fclose(file);
}

int main()
{
	char *chipname = "gpiochip0";
	unsigned int led_line_num = 23;	// GPIO Pin #23
	unsigned int led_state = 0;	// determines state of the led, 0 at default
	struct gpiod_chip *chip;
	struct gpiod_line *led_line;
	int led;

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

	// Check GPIO PIN 23 for output configuration
	led = gpiod_line_request_output(led_line, CONSUMER, 0);
	if (led < 0) {
		perror("Request led line as output failed\n");
		goto release_line;
	}

	// Turn LED off
	led_state = 0;
	led = gpiod_line_set_value(led_line, led_state);

	// Update log file for debugging
	updateFile();

	printf("LED has been turned off! Jippii! URAAAAAS");

release_line:
	gpiod_line_release(led_line);
close_chip:
	gpiod_chip_close(chip);
end:
	return 0;
}  
