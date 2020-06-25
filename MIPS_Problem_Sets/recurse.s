    .data 
get_input:  .asciiz "Please enter a number:"
line_break: .asciiz "\n"
    .text

# ----------------------------------------------------- C - equivalent -------------------------------------------------------------
# int func(int n) {
#     if (n == 0) {
#         return 0;
#     } else if (n == 1) {
#         return 1;
#     } else {
#         return func(n-1) + 2*func(n-2) + 3
#     }
# }

# ------------------------------------------------------ CALLING THE FUNCTION ----------------------------------------------------------

main:

# Prompt User
	li $v0, 4
	la $a0, get_input
	syscall

	la $a0, line_break
	syscall

# Accept User Input
	li $v0, 5
	syscall
	move $a0, $v0							# Number of iterations is stored in $a0


# Calling function
	jal func							    # Call the function with user input
	move $a0, $v0							# Move output to $a0

	li $v0, 1
	syscall

# Ending the program
    li $v0, 10
    syscall

# --------------------------------------------------- DECLARING THE FUNCTION --------------------------------------------------------
func:                                                   # n is stored in $a0, the argument
    bgt $a0, 1, func_recurse
    move $v0, $a0
    jr $ra
func_recurse: 
    addi $sp, $sp, -12                                  # Allocate memory in the stack frame
    sw $ra, 0($sp)                                      # Save return address at beginning of stack

    # Handling function call func(n-1)
    sw $a0, 4($sp)                                      # Save value of n in 4 + $sp
    addi $a0, $a0, -1                                   # Compute argument value
    jal func                                            # Call function
    lw $a0, 4($sp)                                      # Restore value of n in $a0 
    sw $v0, 8($sp)                                      # Save value of fib(n-1) to 8 + $sp

    # Handling function call func(n-2)
    addi $a0, $a0, -2                                   # Compute argument value        
    jal func                                            # call function, value of func(n-2) is stored in $v0

    lw $t0, 8($sp)                                      # Restore value of func(n-1) to register $t0

    mul $v0, $v0, 2                                     # fib(n-2)' = fib(n-2) * 2
    add $v0, $v0, $t0                                   # fib(n-2)" = fib(n-2)' + fib(n-1)
    addi $v0, $v0, 3                                    # ans = fib(n-2)" + 3

    # Deallocate frame and restore return address
    lw $ra, 0($sp)
    addi $sp, $sp, 12

    # Return values
    jr $ra

