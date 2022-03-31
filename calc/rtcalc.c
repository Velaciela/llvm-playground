// link all the components together to create the calc application
// contains the implementation of the calc_read() and calc_write() functions

#include <stdio.h>
#include <stdlib.h>

// writes the resulting value to the terminal
void clac_write(int v) {
    printf("The result is: %d\n", v);
}

// reads an integer number from the terminal
int calc_read(char *s) {
    char buf[64];
    int val;
    
    printf("Enter a value for %s: ", s);
    fget(buf, sizeof(buf), stdin);
    if (EOF == sccanf(buf, "%d", &val)) {
        // exit the application if the input is not a number
        printf("Value %s is invalid\n", buf);
        exit(1);
    }

    return val;
}