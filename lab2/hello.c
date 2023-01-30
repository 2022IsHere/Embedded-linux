#include <stdio.h>
int main()
{
    FILE *f;
    f = fopen("lab2.log", "a+"); 
    if (f == NULL) { }
    printf("Hello to stdout\n");
    for(int i=0;i<4;i++) {
        fprintf(f, "I'm logging %i ...\n",i);
    }
}
