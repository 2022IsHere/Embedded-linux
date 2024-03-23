/* Show the CGI (Common Gateway Interface) environment variables */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Include code to read servo value from html page
#include "detect_servo.h"

/* Print a basic HTTP header. */

static void
print_http_header (const char * content_type)
{
    printf ("Content-Type: %s\n\n", content_type);
}

/* Handle errors by printing a plain text message. */

static void cgi_fail (const char * message)
{
    print_http_header ("text/plain");
    printf ("%s\n", message);
    exit (0);
}

/* The external variable "environ" contains the environment
   variables. */

extern char ** environ;

/* This structure will contain our environment variables. The
   nomenclature follows that of 'man getenv'. */

typedef struct
{
    char * name;
    char * value;
}
env_var;

/* Find the first equals sign ("=") in "s". Returns -1 if there is no
   equals sign in the string. */

static int find_equals (const char * s)
{
    int i = 0;
    while (1) {
	if (s[i] == '=') {
	    return i;
	}
	if (s[i] == '\0') {
	    return -1;
	}
	i++;
    }
}

/* Split an environment variable into the part before the equals sign
   and the part after the equals sign. Return the result in the
   env_var structure called "ev". Both are returned in allocated
   memory. */

static void
split_env_var (const char * variable, env_var * ev)
{
    int equals_pos;
    int env_var_len;

    equals_pos = find_equals (variable);
    if (equals_pos == -1) {
	cgi_fail ("could not parse environment");
    }
    env_var_len = strlen (variable);
    ev->name = strndup (variable, equals_pos);
    if (ev->name == 0) {
	cgi_fail ("out of memory");
    }
    ev->value =
	strndup (variable + equals_pos + 1, env_var_len - equals_pos - 1);
    if (ev->value == 0) {
	cgi_fail ("out of memory");
    }
}

/* Catch an environment variable overflow. */

#define _ENV_VAR_GIVE_UP 1000

/* Count the number of environment variables. */

static int
count_env_vars ()
{
    int i;
    for (i = 0; i < _ENV_VAR_GIVE_UP; i++) {
	if (environ[i] == 0) {
	    return i;
	}
    }
    cgi_fail ("too many environment variables");
    return 0;
}

static void get_environment (env_var * env_vars, int n_env_vars)
{
    int i = 0;
    for (i = 0; i < n_env_vars; i++) {
        split_env_var (environ[i], env_vars + i);
    }
}

int 
main ()
{
    int i;
    int n_env_vars;
    env_var * env_vars;



    n_env_vars = count_env_vars ();
    env_vars = calloc (n_env_vars, sizeof (env_var));
    if (! env_vars) {
        cgi_fail ("out of memory");
    }
    get_environment (env_vars, n_env_vars);
    print_http_header ("text/html");
    printf ("<h1>Environment variables</h1>\n");
    printf ("<table>\n");
    for (i = 0; i < n_env_vars; i++) {
	printf ("<tr><td>%d", i);
	printf ("<td>%s<td>%s",
		env_vars[i].name,
		env_vars[i].value);
    }
    printf ("</table>");

    // Read servo value
    detect_servo();

    free (env_vars);
    return 0;
}