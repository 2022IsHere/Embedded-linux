#ifndef PWM_PULSE_H
#define PWM_PULSE_H

#include <gpiod.h>

#ifndef CONSUMER
#define CONSUMER "Consumer"
#endif

/* STEP 1 */
int pwm_pulse_init();
int toggle_gpio_pin();

/* STEP 2*/
int pwm_pulse_init();
int turn_on_pwm();
int turn_off_pwm();
void cleanup_gpio();

#endif /* PWM_PULSE_H */

