#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Create main memory - 1D array of chars
char mainMemory[(1 << 24)];

// Helper Method 1 - Create a bit string of N 1s 
int oneString(int n) {
    return (1 << n) - 1;                                                                                 // Return N bit long string of 1s                                                                              
}

// Helper Method 2 - Find log base 2 of n
unsigned int logger(int n) {
    return ceil((double) log(n) / (double) log(2));
}

// Helper Method 3 - Returns first n bits of an address from the right [big-endian]
int lastN(int n, int address) {
    int ones = oneString(n);
    return address & ones;
}

// Create a struct for the memory block
typedef struct {
    int valid;                                                                                              // Tells if value in block is valid or not
    int tag;                                                                                                // Tag to identify block
    char data[513];                                                                                         // Data stored in a block
    int dirtyBit;                                                                                           // Tells if block is dirty or not
    int address;
} memoryBlock;

int main(int argc, char *argv[]) {

    // Open file and point to mFile
    FILE* mFile = fopen(argv[1], "r");

    // Read ABCs of cache
    int cacheSize = atoi(argv[2]) * 1024;                                                                  // Convert to integer and multiply by 2^10 since value is in KB
    int associativity = atoi(argv[3]);        
    int blockSize = atoi(argv[4]);

    // Compute basic parameters of the cache
    int numFrames = cacheSize / blockSize;
    int numSets = numFrames / associativity;
    int blockOffsetSize = logger(blockSize);
    int setIndexSize = logger(numSets);
    int tagIndexSize = 24 - setIndexSize - blockOffsetSize;

    memoryBlock cache[numSets][associativity];                                                             // 2D array of memoryBlocks representing the cache

    // Initialise all frames in cache as invalid and write dirtyBit = 0
    for(int i = 0; i < numSets; i++) {
        for(int j = 0; j < associativity; j++) {
            cache[i][j].valid = -1;                                      
            cache[i][j].dirtyBit = 0;
            cache[i][j].tag = -1;
            cache[i][j].address = 0;
            cache[i][j].data[512] = '\0';
        }
    }

    // Initialise all units in memory as 0 in hexadecimal
    for (int i = 0; i < (1 << 24); i ++) {
        mainMemory[i] = 0x00;
    }

    while(1) {                                                          

        char instruction[24];                              
        if (fscanf(mFile, "%s\n", instruction) == EOF) {                                                  // Read instruction, terminate program when all instructions have been read            
            break;
        }

        int address, accessSize;
        fscanf(mFile, "0x%x\n", &address);                                                                // Read the address in hexadecimal without considering 0x
        fscanf(mFile, "%d\n", &accessSize);                                                               // Read access size   

        // Break down address into constituents
        int temp = address;                                                                               // Store the address
        int block = lastN(blockOffsetSize, temp);                                                         // Block now has the offset of the address    
        temp = temp >> blockOffsetSize;                                                                   // temp address now has the everything in address other than offset
        int set = lastN(setIndexSize, temp);                                                              // Set has the set index of the address
        int tag = temp >> setIndexSize;                                                                   // Tag has the tag index

        int setPosition = -1;                                                                             // Tracks the block being considered in the set
        int hitStatus = -1;                                                                               // Tracks whether the block was found in set or not   

        if(strcmp(instruction, "store") == 0) {                                                           // In case operation is store
            for(int i = 0; i < associativity; i++) {                                                      // Go over each frame in the set       
                if(cache[set][i].tag == tag) {                                                            // If block is found in set 
                    printf("%s 0x%x hit\n", instruction, address);                                        // Print store 0x<Address in hex> as a hit        
                    hitStatus = 1;                                                                        // There will be a hit                    
                    setPosition = i;                                                                      // Mark the frame in which this block is currently stored
                    for (int j = setPosition; j > 0; j--){                                                // Reverse iterate over the set from the way in which the block was found
                        if (j == setPosition && cache[set][j].dirtyBit == 1) {
                            for (int k = 0 ; k < strlen(cache[set][j].data) ; k ++) {
                                mainMemory[cache[set][j].address + k] = cache[set][j].data[k];
                            }
                            cache[set][j].dirtyBit = 0;                                                   // Set the dirty bit back to 0  
                        }
                        cache[set][j] = cache[set][j-1];                                                  // Implement the LRU Policy
                    }
                    cache[set][0].tag = tag;                                                              // Initialise all parameters in the structure
                    cache[set][0].valid = 1;
                    cache[set][0].address = address;
                    cache[set][0].dirtyBit = 1;
                    for(int j = 0; j < accessSize; j++) {                                                 // Write data to memory; performs write-back
                        fscanf(mFile, "%02hhx", &cache[set][0].data[block + j]);         
                    }                
                }
            }
            if(hitStatus == -1) {                                                                         // If the block was not found in set 
                printf("%s 0x%x miss\n", instruction, address);                                           // Print store 0x<Address in hex> as a miss
                for (int j = (associativity - 1); j > 0; j--){                                            // Move each block back by 1 index, clearing way 0 and eliminating the least recently used block  
                    if (j == (associativity - 1) && cache[set][j].dirtyBit == 1) {                        // If this is the block being evicted and it is dirty   
                        for (int k = 0 ; k < strlen(cache[set][j].data) ; k ++) {                         // Write all the data stored in that block to main memory 
                            mainMemory[cache[set][j].address + k] = cache[set][j].data[k];                // Move data from cache to memory 
                        }
                        cache[set][j].dirtyBit = 0;                                                       // Set the dirty bit back to 0  
                    }
                    cache[set][j] = cache[set][j-1];        
                }
                cache[set][0].tag = tag;                                                                  // Load new block and initialise it
                cache[set][0].valid = 1;
                cache[set][0].dirtyBit = 1;
                cache[set][0].address = address;
                for(int j = 0; j < accessSize; j++) {                                                     // Write data to be stored in that cache after specified offset
                    fscanf(mFile, "%02hhx", &cache[set][0].data[block + j]);         
                }                
            }
        }

        if(strcmp(instruction, "load") == 0) {                                                            // In case operation is load
            for(int i = 0; i < associativity; i++) {
                if(cache[set][i].tag == tag) {                                                            // In case the block is in the set 
                    hitStatus = 1;                                                                        // Sign as hit       
                    setPosition = i;
                    char dataCopied[513];
                    dataCopied[512] = '\0';
                    strcpy(dataCopied, cache[set][i].data);                                               // Store value from block to be evicted in dataCopied      
                    for (int j = setPosition; j > 0; j--){
                        if (j == setPosition && cache[set][j].dirtyBit == 1) {
                            for (int k = 0 ; k < strlen(cache[set][j].data) ; k ++) {
                                mainMemory[cache[set][j].address + k] = cache[set][j].data[k];
                            }
                            cache[set][j].dirtyBit = 0;                                                   // Set the dirty bit back to 0  
                        }
                        cache[set][j] = cache[set][j-1];
                    }
                    cache[set][0].tag = tag;                                                             // Execute LRU   
                    cache[set][0].valid = 1;
                    cache[set][0].address = address;
                    strcpy(cache[set][0].data, dataCopied);    

                    printf("%s 0x%x hit ", instruction, address);                                        // Print load 0x<Address in hex> as hit      
                    for (int j = 0; j < accessSize; j++) {                                               // Print the value from memory   
                        printf("%02hhx", cache[set][0].data[block + j]);
                    }
                    printf("\n");                                                                        // Print line break
                }
            }
            if (hitStatus == -1) {                                                                       // If the block was not present in set
                for (int j = (associativity - 1); j > 0; j--){                                           // Move each block back by 1 index, clearing way 0   
                    if (j == (associativity - 1) && cache[set][j].dirtyBit == 1) {
                        for (int k = 0 ; k < strlen(cache[set][j].data) ; k ++) {
                            mainMemory[cache[set][j].address + k] = cache[set][j].data[k];  
                        }
                        cache[set][j].dirtyBit = 0;                                                      // Set the dirty bit back to 0  
                    }
                    cache[set][j] = cache[set][j-1];        
                }
                cache[set][0].tag = tag;                                                                 // Load new block to set
                cache[set][0].valid = 1;
                cache[set][0].address = address;
                for (int k = 0 ; k < accessSize ; k ++) {
                    cache[set][0].data[block + k] = mainMemory[address + k];
                }
                printf("%s 0x%x miss ", instruction, address);                                           // Print as miss
                for (int j = 0; j < accessSize; j++) {                                                   // Print the data loaded from cache
                    printf("%02hhx", cache[set][0].data[block + j]);
                }
                printf("\n");                                                                            // Print a line break
            }
        }
    }

    fclose(mFile);                                                                                      // Close file and terminate  
    return EXIT_SUCCESS;
}
