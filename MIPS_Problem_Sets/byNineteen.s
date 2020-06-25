	.data							# Specify string values for user-interface		
get_input:	.asciiz	"Please enter an integer input:"
line_break:	.asciiz	"\n"
	.text

main: 								# Start main block

# Store value of 19 in $t9
li $t9, 19

# Prompt User
li $v0, 4
la $a0, get_input
syscall

la $a0, line_break
syscall

# Accept User Input
li $v0, 5
syscall
move $t0, $v0						# Number of iterations is stored in $t0

# Declare counter
li $t1, 1						# Counter is stored in $t1

# Run loop
loop:	bgt $t1, $t0, end_loop				# analog of for (int i = 1 ; i <= j ; i ++)
	
	mul $t2, $t9, $t1				# $t2 = 19 * i 
		
	li $v0, 1
	move $a0, $t2
	syscall

	li $v0, 4
	la $a0, line_break
	syscall	

	addi $t1, $t1, 1				# $t1 += 1
	j loop
end_loop:
	li $v0, 10
	syscall
