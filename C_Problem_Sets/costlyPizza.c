#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265358979323846                   // Define value for pi

// Declare struct
typedef struct pizza {
    char name[64];
    float diameter;
    float cost;
    float dpsi;                                     // Dollars per Square Inch
    struct pizza *next;                             // Pointer to the next node
} pizza_t;

// Helper Method 1 - Exchange values in 2 different nodes (needed to run bubble sort)
void exchange(pizza_t *p1, pizza_t *p2) {
    char name_val[64];
    strcpy(name_val, (*p1).name);
    float diam = (*p1).diameter;
    float cst = (*p1).cost;
    float rating = (*p1).dpsi;

    strcpy((*p1).name, (*p2).name);
    (*p1).cost = (*p2).cost;
    (*p1).diameter = (*p2).diameter;
    (*p1).dpsi = (*p2).dpsi;

    strcpy((*p2).name, name_val);
    (*p2).cost = cst;
    (*p2).diameter = diam;
    (*p2).dpsi = rating;
}

// Helper Method 2 - Bubble sort
void bubble_sort (pizza_t *pizza_array) {
    pizza_t *node1, *node2;                         // Declare 2 pointers to iterate over linkedlist
    for (node1 = pizza_array ; (*node1).next != NULL ; node1 = (*node1).next) {
        for (node2 = (*node1).next ; node2 != NULL ; node2 = (*node2).next) {
            if ((*node1).dpsi < (*node2).dpsi) {
                exchange(node1, node2);                
            } else if ((*node1).dpsi == (*node2).dpsi && strncmp((*node1).name, (*node2).name, 10) > 0) {
                exchange(node1, node2);
            }
        }
    }
}


int main(int argc, char *argv[]) {                                                                    



    // Declare 2 pointers for a linked list
    pizza_t *pizza_array = NULL;
    pizza_t *temp = NULL;


    // Analyse user input to get file name
    int index = argc - 1;


    // Open file and declare placeholder variables
    FILE *mfile = fopen(argv[index], "r");
    char string_input[64];
    strcpy(string_input, "start");
    float diam, cst, rating;


    // Check if file is NULL or not
    if (mfile == NULL) {
        printf("PIZZA FILE IS EMPTY\n");
    }

    while (1) {                                     // Run loop till string input is not DONE

        fscanf(mfile, "%s", string_input);                                              // Get user input
        
        if (strncmp(string_input, "DONE", 4) == 0) {
            break;
        }
        
        fscanf(mfile, "%f", &diam);
        fscanf(mfile, "%f", &cst);


        pizza_t *pizza_node = (pizza_t *) malloc(sizeof(pizza_t));                                  // Dynamically allocate memory for new node
         
        strcpy((*pizza_node).name,string_input);                                              // Initialising new pizza node    
        (*pizza_node).diameter = diam;
        (*pizza_node).cost = cst;

        if ((*pizza_node).diameter == 0.0) {                                                                    // check if diameter is 0
            (*pizza_node).dpsi = 0.0;

        } else {
            (*pizza_node).dpsi = 4.0*cst/(PI*diam*diam);

        }
        (*pizza_node).next = NULL;

        if (pizza_array == NULL) {                                                      // Adding new nodes to linked list
            pizza_array = pizza_node;
            temp = pizza_node;
        } else {
            (*temp).next = pizza_node;
            temp = (*temp).next;
        }
    }

    fclose(mfile);                                                                 // Close the file
    bubble_sort(pizza_array);                                                            // Sort array


    // Print the array and free malloced memory units
    while (pizza_array != NULL) {
        printf("%s %f\n", (*pizza_array).name, (*pizza_array).dpsi);
        pizza_t *temporary = pizza_array;
        pizza_array = (*pizza_array).next;
        free(temporary); 
    }                                                     


    return EXIT_SUCCESS;
}
                                       

