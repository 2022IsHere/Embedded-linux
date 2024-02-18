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

static struct gpiod_line *pwm_line = NULL;
static int init_state = 0;
static int gpio_state = 0;

/*
 *@brief Init GPIO PIN, CHIP, etc for handling
 *
 */
int pwm_pulse_init() {

    char *chipname = "gpiochip0";
    unsigned int pwm_line_num = 23; // GPIO Pin #23
    struct gpiod_chip *chip;
    int pwm;

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

        int err_code = gpiod_line_set_value(pwm_line, 1);
        if (err_code < 0) {
            perror("Setting default value to gpio line failed!\n\n");
            goto release_line;
        }

        init_state = 1;
        gpio_state = 1;

    }

end:
    return init_state;
release_line:
    gpiod_line_release(pwm_line);
close_chip:
    gpiod_chip_close(chip);
    init_state = 0;
    gpio_state = 0;
    goto end;
}

/*
 *@brief Toggle GPIO PIN (turn on and off)
 *
 *@return 0 on success, -1 on failure
 */
int toggle_gpio_pin() {
    gpio_state = !gpio_state;

    if(gpio_state < 0) {
        perror("Problem retrieving gpio_state!\n");
        return -1;
    }

    int pwm = gpiod_line_set_value(pwm_line, gpio_state);
    if(pwm < 0) {
        perror("Problem toggleling GPIO line!\n");
        return -1;
    }
    return 0;
}
/*
 *@brief Toggle GPIO PIN (turn on)
 *
 *@return 0 on success, -1 on failure
 */
int turn_on_pwm() {
    int pwm = gpiod_line_set_value(pwm_line, 1);
    if(pwm < 0) {
        perror("Turning GPIO line to HIGH failed!\n");
        return -1;
    }
    return 0;
}

/*
 *@brief Toggle GPIO PIN (turn off)
 *
 *@return 0 on success, -1 on failure
 */
int turn_off_pwm() {
    if (init_state == 1) {
        int err_code = gpiod_line_set_value(pwm_line, 0);
        if (err_code < 0) {
            perror("Turning off PWM signal failed!\n\n");
            return -1;
        }
        return 0;
    } else {
        perror("GPIO not initialized!\n\n");
        return -1;
    }
}

/*
 *@brief Cleanup GPIO resources
 */
void cleanup_gpio() {
    if (init_state == 1) {
        gpiod_line_release(pwm_line);
        gpiod_chip_close(gpiod_line_get_chip(pwm_line));
        init_state = 0;
    }
}
