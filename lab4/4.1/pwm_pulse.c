/*
 * pwm_pulse.c
 *
 *  Created on: 5th Feb 2024
 *      Author: Sebastian Sopola
 */

// Necessary libraries for PWM signaling
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>




#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif


/*
 *@brief Init GPIO PIN, CHIP, etc for handling
 *
 */
int pwm_pulse_init() {

    static int init_state = 0;
    static int gpio_state = 1; // use static so that 
    static char *chipname = "gpiochip0";
    static unsigned int pwm_line_num = 23; // GPIO Pin #23
    static struct gpiod_chip *chip;
    static struct gpiod_line *pwm_line;
    static int pwm;
    int err_code;

    if (init_state == 0) {

        // Setup chip
        chip = gpiod_chip_open_by_name(chipname);
        if (!chip) {
            perror("Open chip failed\n");
            goto end;
        }

        // Setup GPIO line for pwm
        pwm_line = gpiod_chip_get_line(chip, pwm_line_num);
        if (!pwm_line) {
            perror("Get pwm line failed\n");
            goto close_chip;
        }

        // Check GPIO PIN 23 for output configuration
        pwm = gpiod_line_request_output(pwm_line, CONSUMER, 0);
        if (pwm < 0) {
            perror("Request pwm line as output failed\n");
            goto release_line;
        }

        err_code = gpiod_line_set_value(pwm_line, gpio_state);

        if (err_code < 0) {
            perror("Setting default value to gpio line failed!\n\n");
            return -1;
        }

        init_state = 1;
    }

    // Toggle gpio line
    if (init_state == 1) {
        gpio_state = !gpio_state;
        pwm = gpiod_line_set_value(pwm_line, gpio_state);
        if(pwm < 0){
            perror("Setting new value to gpio line failed!\n\n");
            return -1;
        }
        return 0;

    }
release_line:
        gpiod_line_release(pwm_line);
close_chip:
        gpiod_chip_close(chip);
end:
        return -1;
}
