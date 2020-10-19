.data
name_of_kernel: .asciiz "==============================This is SPIMOS_GTU_1==============================\n"
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
	li $v0, 4
	la $a0, name_of_kernel
	syscall

	addi $a3, $zero, 4
	li $v0, 19 # Process Table allocation
	syscall

	li $v0, 20
	la $a0, init # Putting init into the Process Table
	addi $a1, $zero, 0
	syscall

	li $v0, 20
	la $a0, Collatz # Putting Collatz into the Process Table
	addi $a1, $zero, 1
	syscall

	li $v0, 20
	la $a0, LinearSearch # Putting LinearSearch into the Process Table
	addi $a1, $zero, 2
	syscall

	li $v0, 20
	la $a0, BinarySearch # Putting BinarySearch into the Process Table
	addi $a1, $zero, 3
	syscall

	li $v0, 23 # Start execution and it wait until all processes are terminated.  
	syscall

###############################################################################################
terminate_program:
	li $v0, 22 # PROCESS_EXIT syscall
	syscall
###############################################################################################