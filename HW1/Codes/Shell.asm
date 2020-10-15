.data
seperator1:	.asciiz "===========================================================================\n"
information1: .asciiz "|======================= CSE 312 Operating Systems =======================|\n"
seperator2:	.asciiz "===========================================================================\n"
information2: .asciiz "Welcome 151044058@SpimShell\n\n"
information3: .asciiz "Runnable files\n"
information4: .asciiz "   -> ShowDivisibleNumbers.asm\n"
information5: .asciiz "   -> BinarySearch.asm\n"
information6: .asciiz "   -> LinearSearch.asm\n"
information7: .asciiz "   -> SelectionSort.asm\n\n"
information8: .asciiz "To terminate 151044058@SpimShell enter \"exit\"\n"  
shell_definiation: .asciiz "151044058@SpimShell$: "
good_bye_message: .asciiz "Terminating 151044058@SpimShell. Hava a nice day!\n"
terminate_shell: .asciiz "exit\n"
autor: .asciiz "Created by Ali Haydar KURBAN\n"

command1: .asciiz "ShowDivisibleNumbers.asm\n"
command2: .asciiz "BinarySearch.asm\n"
command3: .asciiz "LinearSearch.asm\n"
command4: .asciiz "SelectionSort.asm\n"

command1_without_new_line: .asciiz "ShowDivisibleNumbers.asm"
command2_without_new_line: .asciiz "BinarySearch.asm"
command3_without_new_line: .asciiz "LinearSearch.asm"
command4_without_new_line: .asciiz "SelectionSort.asm"


new_line: .asciiz "\n"
command_line_argument: .space 26
not_runnable_command: .asciiz "Command not found!!\n"

.text
.globl main
main:
	jal print_information # Calling print_information function
	
shell_loop: # It is an infinite loop to get command from the user
	la $a3, 0 # In each loop iteration, reinitialization of command_argument

	li, $v0, 4
	la $a0, shell_definiation # Printing shell_definiation
	syscall
	
	li $v0, 8 
	la $a0, command_line_argument # Getting command from the user
	la $a1, 26
	syscall
	
	la $a0, command_line_argument # Assing the command_line_argument to $a0
	la $a2, command1 # Assign the "ShowDivisibleNumbers.asm" to $a2
	jal string_compare # Calling string_compare function
	
	move $t3, $v0 # Moving result of the string_compare to $t3 
	beq $t3, $zero, command1_true # If command_line_argument = "ShowDivisibleNumbers.asm" 
	j command1_false # Otherwise
	
command1_true:	
	# ShowDivisibleNumbers.asm will run
	li $v0, 18 # Calling CREATE_PROCESS system call.
	la $a3, command1_without_new_line # Sending "ShowDivisibleNumbers.asm" to the system call
	syscall	

	j shell_loop # Getting back to the shell_loop
	
command1_false: # If command_line_argument != "ShowDivisibleNumbers.asm"
	la $a0, command_line_argument # Assing the command_line_argument to $a0
	la $a2, command2 # Assign the "BinarySearch.asm" to $a2
	jal string_compare # Calling string_compare function
	
	move $t3, $v0 # Moving result of the string_compare to $t3 
	beq $t3, $zero command2_true # If command_line_argument = "BinarySearch.asm" 
	j command2_false # Otherwise
	
command2_true:
	# BinarySearch.asm will run
	li $v0, 18 # Calling CREATE_PROCESS system call.
	la $a3, command2_without_new_line # Sending "BinarySearch.asm" to the system call
	syscall	

	j shell_loop # Getting back to the shell_loop
	
command2_false: # If command_line_argument != "BinarySearch.asm"
	la $a0, command_line_argument # Assing the command_line_argument to $a0
	la $a2, command3 # Assign the "LinearSearch.asm" to $a2
	jal string_compare # Calling string_compare function
	
	move $t3, $v0 # Moving result of the string_compare to $t3 
	beq $t3, $zero command3_true # If command_line_argument = "LinearSearch.asm"
	j command3_false # Otherwise
	
command3_true:
	# LinearSearch.asm will run
	li $v0, 18 # Calling CREATE_PROCESS system call.
	la $a3, command3_without_new_line  # Sending "LinearSearch.asm" to the system call
	syscall

	j shell_loop # Getting back to the shell_loop
	
command3_false: # If command_line_argument != "LinearSearch.asm"
	la $a0, command_line_argument # Assing the command_line_argument to $a0
	la $a2, command4 # Assign the "SelectionSort.asm" to $a2
	jal string_compare # Calling string_compare function
	
	move $t3, $v0 # Moving result of the string_compare to $t3 
	beq $t3, $zero command4_true # If command_line_argument = "SelectionSort.asm"
	j command4_false # Otherwise
	
command4_true:
	# SelectionSort.asm will run
	li $v0, 18 # Calling CREATE_PROCESS system call.
	la $a3, command4_without_new_line # Sending "SelectionSort.asm" to the system call
	syscall

	j shell_loop # Getting back to the shell_loop
	
command4_false: # If command_line_argument != "SelectionSort.asm"
	la $a0, command_line_argument # Assing the command_line_argument to $a0
	la $a2, terminate_shell # Assign the "exit" to $a2
	jal string_compare # Calling string_compare function
	
	move $t3, $v0 # Moving result of the string_compare to $t3 
	beq $t3, $zero terminate_shell_loop # If command_line_argument = "exit" terminate system
	j new_line_entered # Otherwise

new_line_entered:
	la $a0, command_line_argument # Assing the command_line_argument to $a0
	la $a2, new_line # Assign the "\n" to $a2
	jal string_compare # Calling string_compare function
	
	move $t3, $v0 # Moving result of the string_compare to $t3 
	beq $t3, $zero new_line_entered_true # If command_line_argument = "\n" terminate system
	j default # Otherwise
	
new_line_entered_true:
	j shell_loop # Getting back to the shell_loop
	
default: # Not runnable commands
	li $v0, 4
	la $a0, not_runnable_command 
	syscall
	
	j shell_loop
	
#print_information function that prints information about the shell	
print_information:
	li $v0, 4
	la $a0, seperator1
	syscall

	la $a0, information1
	syscall

	la $a0, seperator2
	syscall

	la $a0, information2
	syscall
	
	la $a0, information3
	syscall
	
	la $a0, information4
	syscall
	
	la $a0, information5
	syscall
	
	la $a0, information6
	syscall
	
	la $a0, information7
	syscall
	
	la $a0, information8
	syscall

	jr $ra
	
#string_compare function that compare 2 strings
#if Return value < 0 then it indicates $a0 is less than $a2.
#if Return value > 0 then it indicates $a2 is less than $a0.
#if Return value = 0 then it indicates $a1 is equal to $a2
string_compare:
	li $v0, 0
	
strcmp_loop:
	lbu $t0, ($a0) # Gettting fist content of $a0 
	lbu $t1, ($a2) # Gettting fist content of $a2
	add $a0, $a0, 1 # Incrementing index
	add $a2, $a2, 1 # Incrementing indexs
	beqz $t0, strcmp_loop_end # If item = null
	beqz $t1, strcmp_loop_end # If item = null
	bgt $t0, $t1, greater_than # If $t0 > $t1
	blt $t0, $t1, less_than # If $t0 < $t1
	beq $t0, $t1, strcmp_loop # If $t0 = $t1
	
greater_than:
	li $v0, 1 # Assign return value
	j strcmp_finish 
	
less_than:
	li $v0, -1 # Assign return value
	j strcmp_finish
	
equal:
	li $v0, 0 # Assign return value
	j strcmp_finish
	
strcmp_loop_end:
	beq $t0, $t1, equal
	beqz $t0, less_than
	beqz $t1, greater_than

strcmp_finish:
	jr $ra
	
terminate_shell_loop:
	li $v0, 4
	la $a0, good_bye_message # Printing good_by_message
	syscall

	la $a0, autor # Printing autor
	syscall

	
	li $v0, 10 # Terminate Shell.asm
	syscall