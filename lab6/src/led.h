#ifndef LED_H
#define LED_H

#define DIMMER_LED_PIN 24

#define SYSTEM_LED_PIN 	23

#define LED_ON 1
#define LED_OFF 0

#define PWM_PERIOD_NS 20000000

#define MIN_DUTY_CYCLE_NS 500000
#define MAX_DUTY_CYCLE_NS 1950000

int map_encoder_value_to_duty_cycle(int encoder_value);

int set_pwm_duty_cycle(int duty_cycle);

#endif
