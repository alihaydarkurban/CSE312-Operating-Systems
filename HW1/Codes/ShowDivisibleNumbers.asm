.data
request: .asciiz "Enter 3 integers\n"
first_number: .asciiz "First number : "
second_number: .asciiz "Second number : "
third_number: .asciiz "Third number : "
divisible_numbers: .asciiz "Divisible numbers : "
no_divisible_number: .asciiz "There is not any divisible number!"
error_division_by_zero: .asciiz "Error!! Division by zero\n"
seperator: .asciiz " "
new_line: .asciiz "\n"

.text
.globl main
main:
###############################################################################################
# Definiation of registers
li $t0, 0 # first_number
li $t1, 0 # second_number
li $t2, 0 # third_number
li $t3, 0 # remaining
li $t4, 0 # flag for is there any divisible number
###############################################################################################
	
###############################################################################################	
# Dislaying the messages and initialization the registers of numbers
	li $v0, 4 # Printing request
	la $a0, request	
	syscall
	
	la $a0, first_number # Printing first_number
	syscall 
	
	# Initialization of the first_number
	li $v0, 5
	syscall
	move $t0, $v0
	###############
	
	li $v0, 4
	la $a0, second_number # Printing second_number
	syscall
	
	# Initialization of the second_number
	li $v0, 5
	syscall
	move $t1, $v0
	###############
	
	li $v0, 4
	la $a0, third_number # Printing third_number
	syscall
	
	# Initialization of the third_number
	li $v0, 5
	syscall
	move $t2, $v0

	beq $t2, $zero, division_by_zero_error

	
###############################################################################################

###############################################################################################
# Swapping if first_num > second_num
	ble $t0, $t1, not_bigger 
	move $t5, $t0
	move $t0, $t1
	move $t1, $t5
	
not_bigger:
###############################################################################################

###############################################################################################
#for (i = first_num + 1; i < second_num; ++i)
#{
#	if (i % third_number == 0)
#		printf("%d ", i);
#}
# Operations
	li $v0, 4
	la $a0, divisible_numbers # Printing divisible_numbers	
	syscall
	addi $t0, $t0, 1 # Incrementing first_number by 1
	# Divisible numbers are between first_number and second_number.
	# For example if first_number is 10 and second_number is 20
	# Then the divisable numbers can be 11, 12, ... , 18, 19 

my_loop:
	bge $t0, $t1, end_loop # first_number >= second_number
	rem $t3, $t0, $t2  # Modulo operation
	
	bne $t3, $zero, continue # Not divisible
	addi $t4, $t4, 1 # Edit flag for divisible numbers
	
	li $v0, 1 # Printing divisible number
	la $a0, ($t0) # Contents of $t0 
	syscall
	
	li $v0, 4
	la $a0, seperator # Printing seperator
	syscall
	
continue:
	addi $t0, $t0, 1 # Incrementing loop variable (first_number)
	j my_loop
	
end_loop:
	bne $t4, $zero, end_if # If $t4 = 0 there is not any divisible number
	li $v0, 4
	la $a0, no_divisible_number # Printing no_divisible_number
	syscall
end_if:
	li $v0, 4
	la $a0, new_line # Printing new_line
	syscall
	j terminate_program

###############################################################################################
division_by_zero_error:
	li $v0, 4
	la $a0, error_division_by_zero
	syscall

###############################################################################################
terminate_program:
# Terminating the program
li $v0, 10
syscall
###############################################################################################

