/*
Lab1 
Sebastian Sopola
*/
#include <stdio.h>
#include <time.h>

void printHello () {

	printf("hello ");

}

void timeStamp () {
	
	time_t currentTime = time(NULL);
	struct tm date = *localtime(&currentTime);

	printf("%d-%02d-%02d %02d:%02d\n", date.tm_year + 1900, date.tm_mon, date.tm_mday, date.tm_hour, date.tm_min);

}

int main () {

	// create a pointer for file
	FILE *filePointer = NULL;

	// open file, append mode, redirect stdout 
	filePointer = freopen("output.txt","a", stdout);

	// check was file opened correctly
	if (filePointer == NULL) {
		printf("There was an issue opening the file...\n");
		return 1;
	}

	printHello();
	timeStamp();

	// close the file
	fclose(filePointer);	

	return 0;
}