/*
 * timers_init.c
 *
 *  Created on: 17 Sep 2019
 *      Author: Jarno Tuominen
 */

#include "pwm_pulse.h"


/* Timer initialization routines and handlers */

#include "bits/types/timer_t.h"
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <string.h> //needed by memset

/* Let's create the global timer objects */
timer_t firstTimerID;
timer_t secondTimerID;
timer_t thirdTimerID;
timer_t fourthTimerID; // 20 ms timer
timer_t fifthTimerID; // Pulse stop timer


/**@brief Common handler for all the timers
 *
 * @details
 */
static void timerHandler( int sig, siginfo_t *si, void *uc )
{

    timer_t *tidp;
    FILE *fp;
    tidp = si->si_value.sival_ptr;
    fp=fopen("/home/pi/timers.log","a");
    if ( *tidp == firstTimerID ) {
    	fprintf (fp, "Timer 1 says hello!\n");
    } else if ( *tidp == secondTimerID ) {
    	fprintf (fp, "Timer 2 says hello!\n");
    } else if ( *tidp == thirdTimerID ) {
    	fprintf (fp, "Timer 3 says hello!\n");
    } else if ( *tidp == fourthTimerID ) {
        fprintf (fp, "Timer 4 says hello!\n");
        /* STEP 1 | Single function to init GPIO and toggle GPIO */
        //pwm_pulse_init();
        //toggle_gpio_pin();
        

        /* STEP 2 */
        pwm_pulse_init();
        turn_on_pwm();
        
    } 
    /* STEP 2 */
    else if ( *tidp == fifthTimerID ) {
        fprintf(fp, "Timer 5 says hello!\n");
        pwm_pulse_init();
        turn_off_pwm();
        cleanup_gpio();
    } 
    fclose(fp);
}


/**@brief Function for creating a timer
 *
 * @details
 */
static int makeTimer(timer_t *timerID, int expire_msec, float interval_msec )
{
    struct sigevent         te;
    struct itimerspec       its;
    struct sigaction        sa;
    int sigNo = SIGRTMIN;

    /* Set up signal handler. */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timerHandler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(sigNo, &sa, NULL) == -1) {
    	printf ("Failed to setup signal handling for timer\n");

        return(-1);
    }

    /* Set and enable alarm */
    te.sigev_notify = SIGEV_SIGNAL;
    te.sigev_signo = sigNo;
    te.sigev_value.sival_ptr = timerID;
    timer_create(CLOCK_REALTIME, &te, timerID);

    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = interval_msec * 1000000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = interval_msec * 1000000;

    timer_settime(*timerID, 0, &its, NULL);
    return(0);
}



/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. Also creates application timers.
 * @todo 	Revise error handling
 */
int timers_init(void)
{
	int err_code;

	//Create some timers
	err_code = makeTimer(&firstTimerID, 5, 5); //5ms
	if (err_code != 0) {
		return err_code;
	}
	err_code = makeTimer(&secondTimerID, 3, 3); //3ms
	if (err_code != 0) {
			return err_code;
	}
	err_code = makeTimer(&thirdTimerID, 15, 15); //15ms
	if (err_code != 0) {
			return err_code;
	}
    err_code = makeTimer(&fourthTimerID, 20, 20); //20ms
	if (err_code != 0) {
			return err_code;
	}
    /* STEP 2 */
    err_code = makeTimer(&fifthTimerID, 0, 1.5); // 1.5 ms PWM pulse length
    if (err_code != 0) {
        return err_code;
    }
    

	return 0;

}
