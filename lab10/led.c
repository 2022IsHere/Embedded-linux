#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>

#define LED_PIN 23

#define ON 1
#define OFF 0

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif

/*
@brief Read ruuvitag_sensor.log
*/
int read_temperature(const char *filename) {
    FILE *file;
    char buffer[1024];
    static int temperature;

    // Open the file for reading
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Read each line and extract the first character
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("First charachter %c and second %c character of line: \n", buffer[0], buffer[1]);
		
		// Initialize a temporary string to store the digits
    	char temporary_string[3];

		/// Check if the first character is a digit
		if (isdigit(buffer[0])) {
			temporary_string[0] = buffer[0];
			temporary_string[1] = '\0';  // Null-terminate the string

			// Check if the second character is also a digit
			if (isdigit(buffer[1])) {
				temporary_string[1] = buffer[1];
				temporary_string[2] = '\0';  // Null-terminate the string
			}

			// Convert the string to an integer
			temperature = atoi(temporary_string);
		}

    }

    // Close the file
    if (fclose(file) != 0) {
        printf("File closed successfully!\n");
    }

    return temperature;
}

/*
@brief Set up gpio
*/
int main(int argc, char **argv) {


	char *chipname = "gpiochip0";
	struct gpiod_chip *chip;
	struct gpiod_line *led_line;
	int system_status;
	
	chip = gpiod_chip_open_by_name(chipname);
	if (!chip) {
		perror("Open chip failed\n");
		system_status = -1;
		goto end;
	}
	
	led_line = gpiod_chip_get_line(chip, LED_PIN);
	if (!led_line) {
		perror("Get led line failed\n");
		system_status = -1;
		goto close_chip;
	}
	
	gpiod_line_request_output(led_line, CONSUMER, OFF);

	while (1) {
        const char *filename = "ruuvitag_sensor.log";
        int tmp = read_temperature(filename);
		printf("TMP value: %d\n", tmp);
        if (tmp >= 21) {
            gpiod_line_set_value(led_line, ON);
            printf("LED IS ON!\n");
        } else {
            gpiod_line_set_value(led_line, OFF);
            printf("LED IS OFF!\n");
        }

        // Sync reading tmp to sensor update time interval
        sleep(5);
    }
	
close_chip:
	gpiod_chip_close(chip);
end:
	return system_status;
}