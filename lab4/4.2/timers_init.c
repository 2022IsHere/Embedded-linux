/*
 * timers_init.c
 *
 *  Created on: 17 Sep 2019
 *      Author: Jarno Tuominen
 */


/* Timer initialization routines and handlers */

#include "bits/types/timer_t.h"
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <string.h> //needed by memset

// Needed for setting step limit for 0% <-> 100% 
#define NUM_STEPS 100

// Global variables to track the current position and the S-Curve table index
int currentTableIndex = 0;
int currentDirection = 1; // 1 for forward, -1 for reverse


#define TABLESIZE 23
static double forwardSCurveTable[TABLESIZE]; 
static double reverseSCurveTable[TABLESIZE]; 
static int tableSize = TABLESIZE;

/* Let's create the global timer objects */
timer_t timerServiceID;

/*
*@brief Function to scale percentage to nanoseconds
*/
double scaleToNs(double percentage, double minNS, double maxNS) {
    return minNS + percentage * (maxNS - minNS) / 100.0;
}

/*
*@brief Calculate the S-curve table in nanoseconds with given percentages
*
*@details Calculates both forward and reverse S-Curve tables
*/
int calculateSCurve() {
    // Percentages for steps for S-curve table
    double percentages[] = {0.0, 0.4, 1.3, 3.0, 5.6, 9.3, 14.2, 20.3, 27.0, 34.3, 41.9, 49.7, 57.6, 65.2, 72.5, 79.2, 85.2, 90.3, 94.3, 97.0, 98.6, 99.6, 100.0};

    // Minimum and maximum pulse width in nanoseconds
    double minPW_NS = 1500000;
    double maxPW_NS = 2100000;

    // Calculate forward S-Curve table
    for (int index = 0; index < tableSize; index++) {
        forwardSCurveTable[index] = scaleToNs(percentages[index], minPW_NS, maxPW_NS);
    }

    printf("S-Curve table (in nanoseconds):\n");
    for (int index = 0; index < tableSize; index++) {
        printf("%.1f%%: %.0f ns\n", percentages[index], forwardSCurveTable[index]);
    }

    // Calculate reverse S-Curve table
    for (int index = 0; index < tableSize; index++) { 
        reverseSCurveTable[index] = scaleToNs(percentages[tableSize - 1 - index], minPW_NS, maxPW_NS);
    }

    printf("S-Curve table (in nanoseconds):\n");
    for (int index = 0; index < tableSize; index++) {
        printf("%.1f%%: %.0f ns\n", percentages[tableSize - 1 - index], reverseSCurveTable[index]);
    }

    return 0;
}

/*
*@brief Function to write PWM duty cycle with value from S-Curve table
*
*/
void writePositionToPWM(int pwmNumber, double position) {
    char pwmPath[50];
    snprintf(pwmPath, sizeof(pwmPath), "/sys/class/pwm/pwmchip0/pwm%d/duty_cycle", pwmNumber);

    FILE *filename = fopen(pwmPath, "w");
    if (filename == NULL) {
        perror("Failed to open PWM file for writing!\n");
        return;
    }

    fprintf(filename, "%.0f", position);

    if (fclose(filename) != 0) {
        perror("Failed to close PWM file!\n");
    }
    
}


/*
*@brief Common handler for all the timers
*
* @details
*/
static void timerHandler( int sig, siginfo_t *si, void *uc )
{

    timer_t *tidp;
    FILE *fp;
    tidp = si->si_value.sival_ptr;
    fp=fopen("/home/pi/timers.log","a");
    if ( *tidp == timerServiceID ) {
    	fprintf (fp, "Timer is triggered at another 100 ms!\n");
        
        // Determine which S-Curve table to use based on the current direction
        double *currentTable = (currentDirection == 1) ? forwardSCurveTable : reverseSCurveTable;

        // Call the function to write the position to PWM based on the current index
        writePositionToPWM(0, currentTable[currentTableIndex]);

        // Increment the index
        currentTableIndex++;

        // Check if we reached the end of the S-Curve table
        if (currentTableIndex >= tableSize)
        {
            currentTableIndex = 0;
            currentDirection = (currentDirection == 1) ? -1 : 1; // Toggle direction
        }
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

	//Create timers
	err_code = makeTimer(&timerServiceID, 0, 300); //100ms
	if (err_code != 0) {
		return err_code;
	}

	return 0;

}
