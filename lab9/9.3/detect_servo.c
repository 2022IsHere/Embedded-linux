#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * @brief Function updates servo log
 */
void updateFile(int servo_pos) {
    FILE *file = fopen("/var/log/lighttpd/servo.log", "a");
    if (file == NULL) {
        printf("There was an issue opening the file...\n");
        exit(1);
    }

    // Update log
    fprintf(file, "Servo position: %d\n", servo_pos);

    fclose(file);
}

/*
 * @brief Function reads servo value from environment variables
 */
void detect_servo() {
    char *query_string = getenv("QUERY_STRING");
    if (query_string == NULL) {
        printf("No query string found.\n");
        return;
    }

    // Parse the query string to extract servoposition
    char *servoposition_str = strstr(query_string, "servoposition=");
    if (servoposition_str != NULL) {
        int servo_pos;
        if (sscanf(servoposition_str, "servoposition=%d", &servo_pos) == 1) {
            printf("Servo position: %d\n", servo_pos);
            updateFile(servo_pos);
        } else {
            printf("Invalid servo position value.\n");
        }
    } else {
        printf("No servo position found in query string.\n");
    }
}
