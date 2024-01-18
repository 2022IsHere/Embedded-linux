/* #include <stdio.h>
int main()
{
    FILE *f;
    /// open file, append mode, redirect stdout 
    f = freopen("lab2.log", "a", stdout);
    
    if (f == NULL) { 
    	printf("There was an issue opening the file...\n");
	return 1;
    }
    printf("Hello to stdout\n");
    for(int i=0;i<4;i++) {
        fprintf(f, "I'm logging %i ...\n",i);
    }
    
    // flush the output buffer to the file each run - Allows real-time tailing
    //fflush(stdout);

    // close the file
    fclose(f);
} */

/*
Lab2 - this replaced the demo code
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
	filePointer = freopen("lab2.log","a", stdout);

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
