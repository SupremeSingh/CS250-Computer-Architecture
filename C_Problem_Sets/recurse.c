#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper Method 1
int f_N(int num) {
    // Base Case
    if (num == 0) {
        return 0;
    } else if (num == 1) {
        return 1;
    } else {                    // Recursive case
        return f_N(num - 1) + 2*f_N(num - 2) + 3;
    }
}

int main(int argc, char *argv[]) {
    int index = argc - 1;                           // Find index at which last argument exists in argv
    int input = strtol(argv[index], NULL, 10);      // Get input by convertinf argv[index] to integer 
    printf("%d", f_N(input));
    return EXIT_SUCCESS;
}