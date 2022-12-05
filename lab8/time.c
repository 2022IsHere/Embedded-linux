// Could compile locally in raspi
// gcc time.c -o time.cgi
// and copy executable to /usr/lib/cgi-bin

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main (void) {
    time_t rawtime;
    struct tm * timeinfo;
    char textbuf[30];

        printf("Status: 200 OK\r\n");
	// encapsulate current time info to JSON
        printf("Content-type: application/json\r\n\r\n");
        printf("{ \"time\": ");
        {
          time ( &rawtime );
          ctime_r(&rawtime, textbuf);
          textbuf[strlen(textbuf)-1] = 0;
          printf ( "\"%s\"", textbuf);
        }
	printf("}\n");
}
