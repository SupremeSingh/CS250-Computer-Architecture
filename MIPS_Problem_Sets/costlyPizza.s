    .data
name_prompt:    .asciiz "Please enter pizza name:"                           # Prompts user to enter pizza name 
diam_prompt:    .asciiz "Please enter pizza diameter:"                       # Prompts user to enter pizza diameter
price_prompt:   .asciiz "Please enter pizza price:"                          # Prompts user to enter pizza price           
end:            .asciiz "DONE"                                               # Terminates console input 
line_break:     .asciiz "\n"                                                 # Causes line break 
tab:            .asciiz " "                                                  # Causes a tab shift
pi:             .float 3.14159265358979323846                                # Define value of pi
float_zero:     .float 0.0                                                   # Define 0 for comparison with float registers
radius_convert: .float 2.0                                                   # Use value to convert diameter to radius
    .text

main:
    lwc1 $f3, pi                                                             # Store value of pi in $f3
    lwc1 $f4, float_zero                                                     # Store value of float_zero in $f4
    lwc1 $f5, radius_convert                                                 # Store 2.0 in $f5   
input_loop:                                                                  # Loop to get user inputs   
  # Create space to read the 64-character pizza name 
    li $v0, 9
    li $a0, 64
    syscall
    move $t0, $v0                                                            # $t0 is the pointer to the structure
  # Prompt user to enter a name
    li $v0, 4
    la $a0, name_prompt   
    syscall
  # Read input
    li $v0, 8
    la $a0, 0($t0)                                                           # Store pointer to string in 0 + ($t0)
    li $a1, 64                                                               # Allocate 64 bytes of space to read the name   
    syscall
  # Copy the name to new register for further processing
    move $t1, $t0                                                            # Copy pointer to name to $t1
    lb $t2, line_break                                                       # Save a line break in $t2
  # Read every byte and check for initial conditions
readBytes:
    lb $t3, 0($t1)                                                           # Load a character of the name into $t3
    beqz $t3, checkDONE                                                      # On reachind end of the string, check if it is equal to DONE   
    beq $t3, $t2, trim                                                       # On reaching the line break, replace with string termination - $zero
    addi $t1, $t1, 1                                                         # Move onto next byte in the name   
    j readBytes                                                              # Jump to the top of the loop
  # Replace line break with $zero 
trim:
    sb $0, 0($t1)                                                            # Store a 0 in place of the line break in $t1 
  # Process string once all the characters have been read, no extra spaces left now
checkDONE:
    move $a0, $t0                                                            # Copy the pizza name to first argument register
    la $a1, end                                                              # Store "DONE" in second argument register       
    jal strncmp                                                              # Compare both the strings
    move $a0, $t4                                                            # Move pointer to item to first argument register, so it can be printed if input is DONE  
    beqz $v0, print                                                          # If the input is equal to "DONE", execute print  
  # Dynamically allocate memory for a new structure to add to list if name is not DONE
    li $v0, 9
    li $a0, 12                                                               # Allocate 12 bytes of space to read 3 attributes   
    syscall
    move $t5, $v0                                                            # $t5 is pointer to new structure
    sw $t0, 0($t5)                                                           # Store address of pizza name to beginning of the new , at 0 + ($t5)
  # Create a line break
    li $v0, 4
    la $a0, line_break
    syscall
  # Prompt user to enter a diameter 
    li $v0, 4                                                                  
    la $a0, diam_prompt
    syscall
  # Read Input
    li $v0, 6
    syscall
    mov.s $f1, $f0                                                           # Diameter of pizza is stored in $f1
  # Create a line break
    li $v0, 4
    la $a0, line_break
    syscall
  # Prompt user to enter a price
    li $v0, 4                                                                   
    la $a0, price_prompt
    syscall
  # Read Input
    li $v0, 6
    syscall                                                                  # Price is stored in $f0
  # Compute DPSI - Make 0 if the diameter is 0 and compute if not
    c.eq.s $f1, $f4                                                          # Check equality between diameter and 0.0 (Since diameter is float) 
    bc1t setZero                                                             # If diameter is 0, set the value of the dpsi to 0 
    div.s $f1, $f1, $f5                                                      # Save radius instead of diameter in $f1 
    mul.s $f1, $f1, $f1                                                      # $f1 ' = ($f1)^2, square value of radius       
    mul.s $f1, $f1, $f3                                                      # Store area of pizza in $f1 , m * r^2
    div.s $f2, $f0, $f1                                                      # Store dpsi in $f2
    swc1 $f2, 4($t5)                                                         # Store dpsi after name in structure, so at 4 + ($t5)
  # Create a pointer to the next structure
    sw $0, 8($t5)                                                            # Initialise pointer to null, so next is at 8 + ($t5)
    move $a0, $t4                                                            # Move pointer to item in list to first argument register
    move $a1, $t5                                                            # Copy pointer to structure to second argument register
    jal addToList                                                            # Add the structure to the list
    move $t4, $v0                                                            # Increment pointer to last node in list         
    j input_loop                                                             # Jump back to top and repeat the loop
  # Set the value of the dpsi of this node to 0 
  setZero:
    swc1 $f1, 4($t5)                                                         # Store dpsi as 0.0 after name in structure, so at 4 + ($t5)
  # Create a pointer to the next structure
    sw $0, 8($t5)                                                            # Initialise pointer to null, so next is at 8 + ($t5)
    move $a0, $t4                                                            # Move pointer to item in list to first argument register
    move $a1, $t5                                                            # Copy pointer to structure to second argument register
    jal addToList                                                            # Add the structure to the list
    move $t4, $v0                                                            # Increment pointer to last node in list 
    j input_loop                                                             # Jump back to top and repeat the loop
# Helper Method 1: Compare 2 strings ; Arguments: String 1, String 2 ----------------------------------------------------------------------------
strncmp:
  # Allocate memory space on the stack
    addi $sp, $sp, -16                                                       # Allocate 16 bytes of space  
    sw $ra, 0($sp)                                                           # Save $ra
    sw $s0, 4($sp)                                                           # Save other registers
    sw $s1, 8($sp)
    sw $s2, 12($sp)
  # Copy arguments to local registers and save on the stack
    move $s0, $zero                                                          # Set $s0 to 0    
    move $s1, $a0                                                            # Set $s1 to value of $ao
    move $s2, $a1                                                            # Set $s2 to value of $a1
my_loop:
    lb $s3,0($s1)                                                            # Store first byte of $s1(first string) in $s3
    lb $s4,0($s2)                                                            # Store first byte of $s2(second string) in $s4                
    beqz $s3,go_low                                                          # If first string is finished, execute go_low
    beqz $s4,higher                                                          # If second string is finished, execute higher
    blt $s3,$s4,lower                                                        # If string is logically less, execute appt. code 
    bgt $s3,$s4,higher 
    addi $s1,$s1,1                                                           # Increment read character in $t3 and $t4
    addi $s2,$s2,1
    j my_loop
go_low:
    beqz $s4,equal                                                           # If second string is also finished, return 0
    j lower                                                                  # Else, first string < second string and return -1
  # If both the strings are equal, return 0
equal:
    li $v0,0
    lw $ra, 0($sp)
    lw $s0, 4($sp)
    lw $s1, 8($sp)
    lw $s2, 12($sp)
    addi $sp, $sp, 16
    jr $ra
lower:
    li $v0,-1                                                               # Second string < first string and return -1
    lw $ra, 0($sp)
    lw $s0, 4($sp)
    lw $s1, 8($sp)
    lw $s2, 12($sp)
    addi $sp, $sp, 16
    jr $ra
higher:
    li $v0,1                                                                # Second string > first string and return 1
    lw $ra, 0($sp)
    lw $s0, 4($sp)
    lw $s1, 8($sp)
    lw $s2, 12($sp)
    addi $sp, $sp, 16
    jr $ra

# Helper Method 2: Print ;  Arguments: Pointer to a node --------------------------------------------------------------------------------------
print:
  # Copy argument to local register
    move $s5, $a0                                                           # Store the first argument in $s5
    beqz $a0, end_program                                                   # If first argument is null, end the program     
  # Print pizza name
    li $v0, 4
    lw $s6, 0($s5)                                                          # Get address of the name from the structure 
    la $a0, 0($s6)                                                          # Store name of the structure in $a0
    syscall
  # Print a space
    la $a0, tab
    syscall
  # Print the dpsi
    li $v0, 2
    l.s $f12, 4($s5)
    syscall
  # Give line break
    li $v0, 4
    la $a0, line_break
    syscall
  # Move to next node
    lw $a0, 8($s5)
    j print                                                                 # Jump back to the loop

# Helper Method 3: Add new structure to linked list in sorted way ; Arguments: Pointers to current and next node -------------------------------
addToList:
  # Make space on stack and save registers
    addi $sp, $sp, -12                                                      # Create 12 bytes of space on the stack 
    sw $ra, 0($sp)                                                          # Save return address
    sw $s0, 4($sp)                                                          # Save $s0 to 4 + ($sp)
    sw $s1, 8($sp)                                                          # Save $s1 to 8 + ($sp)
  # Copy arguments into callee-saved registers
    move $s0, $a0                                                           # Copy current node into $s0
    move $s1, $a1                                                           # Copy new node into $s1
  # Add items to linked list
    beqz $s0, case1                                                         # If current node is null, make the new node the head of the linked list
    lwc1 $f7, 4($s0)                                                        # Store dpsi from first node in $f7
    lwc1 $f8, 4($s1)                                                        # Store dpsi from second node in $f8
    c.le.s $f8, $f7                                                         # If dpsi of the new node is more then the previous one, 
    bc1f case2                                                              # Move the current node up by one and put new node in its place
    c.eq.s $f7, $f8                                                         # If dpsi of the 2 nodes is not the same
    bc1f case3                                                              # Compare with next node and place items accordingly
    lw $a0, 0($s0)                                                          # Store name of the current node in first argument register
    lw $a1, 0($s1)                                                          # Store name of the new node in the second argument register
    jal strncmp                                                             # Compare both the strings
    bgtz $v0, case2                                                         # If the new name > old name, put new node after the current node
  # First base case for recursive code
case1: 
    move $v0, $s1                                                           # Make the second struct the first node in the list
    j return
  # Second base case for recursive code
case2:  
    sw $s0, 8($s1)                                                          # Put current node after new node
    move $v0, $s1                                                           # Place new node in current node's old position
    j return        
  # Recursive case
case3:      
    lw $a0, 8($s0)                                                          # Move the argument to the node after current node 
    move $a1, $s1                                                           # Copy new struct to second argument register
    jal addToList                                                           # Add new node to next node after current node (recursive)
    sw $v0, 8($s0)                                                          # Store returned node as next to old node 
    move $v0, $s0                                                           # Return initial node   
  # Return final result
return:
    lw $ra, 0($sp)                                                          # Reload the return address
    lw $s0, 4($sp)                                                          # Reload $s1
    lw $s1, 8($sp)                                                          # Reload $s2
    addi $sp, $sp, 12                                                       # Collapse the stack
    jr $ra 

  # Terminate the program
end_program:
    li $v0, 10
    syscall

