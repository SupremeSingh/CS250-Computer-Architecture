#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper Method 1
void print_nineteens(int num) {
    int count = 1;
    while (count <= num) {
        printf("%d\n", 19*count);
        count += 1;
    }
}

int main(int argc, char *argv[]) {
    int index = argc - 1;                           // Find index at which last argument exists in argv
    int input = strtol(argv[index], NULL, 10);      // Get input by convertinf argv[index] to integer
    print_nineteens(input); 
    return EXIT_SUCCESS;                            // Terminate program
}

