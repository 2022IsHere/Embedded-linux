/*
 * main.c
 *
 *  Created on: 17 Sep 2019
 *      Author: Jarno Tuominen
 */


#include "timers_init.h"
#include <stdio.h> //Needed for fprintf
#include <unistd.h> //needed for sleep

int main(void)
{
	int err_code;
	err_code = timers_init();

	while(1) {
		sleep(1);
	}

	return err_code;
}
