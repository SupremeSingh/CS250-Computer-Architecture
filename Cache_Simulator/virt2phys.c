// Include Libraries 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Helper Method 0 - Returns string of N 1s
int oneString(int N) {
    return (1 << N) - 1;                                                                                 // Return N bit long string of 1s                                                                              
}

// Helper Method 1 - Find log base 2 of n
unsigned int logger(int n) {
    return ceil((double) log(n) / (double) log(2));
}

// Helper Method 2 - Compute the VPN
int VPNFinder(int hexInput, int offsetSize, int wordSize) {
    int mask = (oneString(wordSize) >> offsetSize) << offsetSize;
    hexInput = hexInput & mask;
    hexInput = hexInput >> offsetSize;
    return hexInput;
}

// Helper Method 3 - Compute the offset
void OffsetFinder(int ppn_string, int hexInput, int vpnSize, int wordSize) {
    int mask = (oneString(wordSize) >> vpnSize);
    hexInput = hexInput & mask;
    hexInput = hexInput + ppn_string;
    printf("%x", hexInput);
}

int main(int argc, char *argv[]) {

// Declare global variables
    int wordSize, pageSize;
    char *virtualAddress;

// Open relevant file 
    FILE *mFile = fopen(argv[1], "r");                      // Open file containing page table and read
    virtualAddress = argv[2];                               // Store virtual address as a string

// Read important characteristic of page table
    fscanf(mFile, "%d", &wordSize);
    fscanf(mFile, "%d", &pageSize);

// Compute the size of vpn and offset
int numVirtualPages = (1 << wordSize) / pageSize;
int vpnSize = logger(numVirtualPages);
int offsetSize = wordSize - vpnSize;
int vAddress = (int) strtol(virtualAddress, NULL, 16);  ;

// Index to find correct page entry in table
int vpn = VPNFinder(vAddress, offsetSize, wordSize);
int index = 0;
int ppn_string;

// Loop over the page table 
while (1) {
    fscanf(mFile, "%d", &ppn_string);
    if (index == vpn) {
        if (ppn_string == -1) {
            printf("PAGEFAULT\n");  
            return EXIT_SUCCESS; 
        } 
        break;
    }
    index +=1;
}

// Close the file
    // if (ppn_string != 0) { 
    //     printf("%x", ppn_string);
    // }
    ppn_string = ppn_string << offsetSize;
    OffsetFinder(ppn_string, vAddress, vpnSize, wordSize);
    printf("\n");
    fclose(mFile);
// Terminate Main
    return EXIT_SUCCESS;
}