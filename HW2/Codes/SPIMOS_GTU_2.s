.data
name_of_kernel: .asciiz "==============================This is SPIMOS_GTU_2==============================\n"
LinearSearch: .asciiz "LinearSearch.s"
BinarySearch: .asciiz "BinarySearch.s"
Collatz: .asciiz "Collatz.s"
init: .asciiz "init"
invalid_program: .asciiz "Program is not found\n"

seperator: .asciiz " "
colon: .asciiz " : "
new_line: .asciiz "\n"

.text
.globl main
main:
###############################################################################################	
	li $t2, 1 # Counter

	li $v0, 4
	la $a0, name_of_kernel
	syscall

	addi $a3, $zero, 11
	li $v0, 19 # Process Table allocation
	syscall

	li $v0, 20
	la $a0, init # Putting init into the Process Table
	addi $a1, $zero, 0
	syscall
	
	# This is for choosing random program and putting it into the Process Table 
	jal choose_random_program 

	li $v0, 23 # Start execution and it wait until all processes are terminated. 
	syscall

	j terminate_program
	
choose_random_program:

	addi $a0, $zero, 3
	li $v0, 42 # Syscall for creating random number
	syscall
	
	move $t0, $v0
	
	beq $t0, $zero, program_0 # It means that random program is LinearSearch
	j not_program_0

# This is putting LinearSearch into the Process Table
program_0:
	beq $t2, 11, end_choose_random_program

	li $v0, 20
	la $a0, LinearSearch # Putting LinearSearch into the Process Table
	add $a1, $zero, $t2
	syscall

	addi $t2, $t2, 1
	j program_0

not_program_0:
	beq $t0, 1, program_1 # It means that random program is BinarySearch
	j not_program_1

# This is putting BinarySearch into the Process Table	
program_1:
	beq $t2, 11, end_choose_random_program

	li $v0, 20
	la $a0, BinarySearch # Putting BinarySearch into the Process Table
	add $a1, $zero, $t2
	syscall

	addi $t2, $t2, 1
	j program_1
	
not_program_1:
	beq $t0, 2, program_2 # It means that random program is Collatz
	j not_program_2

# This is putting Collatz into the Process Table
program_2:
	beq $t2, 11, end_choose_random_program

	li $v0, 20
	la $a0, Collatz # Putting Collatz into the Process Table
	add $a1, $zero, $t2
	syscall
	
	addi $t2, $t2, 1
	j program_2

not_program_2:
	li $v0, 4
	la $a0, invalid_program
	syscall
	j end_choose_random_program

end_choose_random_program:
	jr $ra
	
###############################################################################################
terminate_program:
	li $v0, 22 # PROCESS_EXIT syscall
	syscall
###############################################################################################