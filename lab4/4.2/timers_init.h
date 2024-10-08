/*
 * timers_init.h
 *
 *  Created on: 17 Sep 2019
 *      Author: Jarno Tuominen
 */

#ifndef TIMERS_INIT_H_
#define TIMERS_INIT_H_

//function prototypes
int timers_init(void);
double scaleToNs(double percentage, double minNS, double maxNS);
int calculateSCurve();
void writePositionToPWM(int pwmNumber, double position);

#endif /* TIMERS_INIT_H_ */
