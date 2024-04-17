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
//#include "rotary_encoder.h"
#include "pwm.h"

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif

#include <sys/time.h> 

#define LOW 0
#define HIGH 1
#define BOUNCING 20
#define STATE_DIMMER_LED  0
#define STATE_SERVO_CONTROL 1

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

#define ENCODER_PIN_A 17
#define ENCODER_PIN_B 27
#define LED_PIN 24

typedef struct {
	unsigned int line_num;
    struct timespec ts;
    struct gpiod_line *line;
} gpio_stuff_t;

typedef struct {
    struct gpiod_line *lineA;
    struct gpiod_line *lineB;
	bool flag; // Flag is used to exit encoder function when needed
} EncoderThreadArgs;

#define MIN_ENCODER_VALUE 0
#define MAX_ENCODER_VALUE 100

// PWM period in nanoseconds
#define PWM_PERIOD_NS 20000000  // 20 ms

// Define the duty cycle range
#define MIN_DUTY_CYCLE_NS 500000   // 0.5 ms
#define MAX_DUTY_CYCLE_NS 1950000  // 1.95 ms

void *gpio_function(void *args) {
    int i, ret;
    struct gpiod_line_event event;

    gpio_stuff_t *actual_args = args;

    i = 1;

    printf("Let's go luteet, says gpio thread\n");

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

		printf("Get event notification on line #%u %d times\n", actual_args->line_num, i);
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

void *read_rotary_encoder(void *args) {
    EncoderThreadArgs *encoderThreadArgs = args;

    static int encoderPosCount = 0;
    static int pinAlast;
    int aVal;
    bool bCW;

	while(encoderThreadArgs->flag) {
		aVal = gpiod_line_get_value(encoderThreadArgs->lineA);

		if (aVal != pinAlast) { // Means the knob is rotating
			// If the knob is rotating, we need to determine the direction
			// We do that by reading pin B.
			if (gpiod_line_get_value(encoderThreadArgs->lineB) != aVal) { // Means pin A changed first - we're rotating clockwise
				encoderPosCount++;
				bCW = true;
			} else {
				bCW = false;
				encoderPosCount--;
			}

			// Apply limits to encoder (0 to 100)
			encoderPosCount = (encoderPosCount > MAX_ENCODER_VALUE) ? MAX_ENCODER_VALUE : encoderPosCount;
			encoderPosCount = (encoderPosCount < MIN_ENCODER_VALUE) ? MIN_ENCODER_VALUE : encoderPosCount;

			if (bCW) {
				printf("Clockwise turn!\n");
			} else {
				printf("Counter-clockwise turn!\n");
			}
			printf("\n\nEncoder position: %d\n\n", encoderPosCount);
		}
		pinAlast = aVal;

		// Add a delay to avoid high CPU usage
		usleep(10000); // Sleep for 10 milliseconds
	}
    return NULL; // Return NULL (or another value) as required by pthread_create
}


/*
*@brief Control switching between dimmer led (0) and servo control (1) modes
*/
void knob_mode_control(struct gpiod_line *lineRed,  struct gpiod_line *lineButton, int *currentState, struct gpiod_line *lineA, struct gpiod_line *lineB) {
	
	// ------------------- SYSTEM LED LOGIC ------------------------
	int newButtonState = gpiod_line_get_value(lineButton);
	if (newButtonState == LOW) {
		if (*currentState == STATE_DIMMER_LED) {
			*currentState = STATE_SERVO_CONTROL;
			gpiod_line_set_value(lineRed, HIGH);
		} else 
		{
			*currentState = STATE_DIMMER_LED;
			gpiod_line_set_value(lineRed, LOW);
		}
	}
	// --------------------MODE SELECTION LOGIC---------------------

	if (*currentState == STATE_DIMMER_LED) {

		printf("Controlling led!\n");

		// No need to call read_rotary_encoder here, as it's handled in a separate thread

        // adjustEncoderValue(encoderValue);

        // Adjust led brightness next

	} else 
	{
		printf("Controlling servo!\n");

	}
	// --------------------------------------------------------------
}

int main(int argc, char **argv)
{

	pthread_t gpio_thread;

	struct gpiod_chip *chip;
	struct gpiod_line *lineButton;	// System state switch
	struct gpiod_line *lineRed;     // System status Led
	struct gpiod_line *lineLed;		// Dimmer Led
	struct gpiod_line *lineA;		// Rotary encoder pin A
	struct gpiod_line *lineB;		// Rotary encoder pin B

	char *chipname = "gpiochip0";


	int i, ret;
	// int pinA, pinB;

	struct timespec ts = { 5, 0 }; //we will wait 5sec and then give up


	chip = gpiod_chip_open_by_name(chipname);
	if (!chip) {
		perror("Open chip failed\n");
		ret = -1;
		goto end;
	}

	lineButton = gpiod_chip_get_line(chip, BUTTON_PIN);
	if (!lineButton) {
		perror("Get line failed\n");
		ret = -1;
		goto close_chip;
	}

	ret = gpiod_line_request_rising_edge_events(lineButton, CONSUMER);
	if (ret < 0) {
		perror("Request event notification failed\n");
		ret = -1;
		goto release_line;
	}

	// Open more GPIO lines
	lineRed = gpiod_chip_get_line(chip, RED_PIN);
	lineLed = gpiod_chip_get_line(chip,LED_PIN);
	lineA = gpiod_chip_get_line(chip, ENCODER_PIN_A);
	lineB = gpiod_chip_get_line(chip, ENCODER_PIN_B);

	// Configure LED lines as outputs, default to logic LOW (off)
	gpiod_line_request_output(lineRed, "example1", 0);
	gpiod_line_request_output(lineLed, CONSUMER, 0);

	// Create struct for a button.
	// Assign button variables into struct to be accessed in button thread
	gpio_stuff_t gpio_stuff;

	gpio_stuff.line = lineButton;
	gpio_stuff.line_num = BUTTON_PIN;
	gpio_stuff.ts = ts;

	//ret = pthread_create( &gpio_thread, NULL, gpio_function, &gpio_stuff);

	
	// Create encoder thread
	pthread_t encoder_thread;

	// Create struct for a rotart encoder
	// Assign lineA and lineB to be read in encoder thread
	EncoderThreadArgs encoderStruct;
    encoderStruct.lineA = lineA;
    encoderStruct.lineB = lineB;
	encoderStruct.flag = true;

	ret = pthread_create(
	&encoder_thread,NULL,read_rotary_encoder,&encoderStruct);
	if (ret == 0) {
		perror("Successfully created encoder thread!\n");
	} else {
		perror("Error creating encoder thread!\n");
	}

	/* The eternal main loop */
	i = 0;

	// Call the setUpPwm function to enable HW PWM for LED and Servo

	int currentState = STATE_DIMMER_LED;

	printf("Entering main loop.\n");

	while(true) {
		knob_mode_control(lineRed, lineButton, &currentState, lineA, lineB);
		usleep(200e3);
	}

	ret = 0;

release_line: //generally.... don't use use "goto"s. Error handlers are the only justified place
	gpiod_line_release(lineRed);
	gpiod_line_release(lineButton);
	gpiod_line_release(lineLed);
	gpiod_line_release(lineA);
	gpiod_line_release(lineB);

close_chip:
	gpiod_chip_close(chip);

end:
	return ret;
}
