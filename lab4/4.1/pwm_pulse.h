#ifndef PWM_PULSE_H
#define PWM_PULSE_H

#include <gpiod.h>

#ifndef CONSUMER
#define CONSUMER "Consumer"
#endif

int pwm_pulse_init();
int toggle_gpio_pin();

#endif /* PWM_PULSE_H */

