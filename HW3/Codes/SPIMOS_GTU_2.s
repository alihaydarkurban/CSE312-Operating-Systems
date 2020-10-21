.data
name_of_the_kernel: .asciiz "==============================This is SPIMOS_GTU_2==============================\n"
LinearSearch: .asciiz "LinearSearch.s"
BinarySearch: .asciiz "BinarySearch.s"
Collatz: .asciiz "Collatz.s"
Palindrome: .asciiz "Palindrome.s"
init: .asciiz "init"
invalid_program: .asciiz "Program is not found\n"

seperator: .asciiz " "
colon: .asciiz " : "
new_line: .asciiz "\n"

program_counter: .word 0 # ProgramCounter
not_start_yet: .word 2 # Not started the execution yet

# typedef struct                   // 204 bytes
# {  
#    int32 Registers[32];          // 128 bytes 
#    mem_addr ProgramCounter;      // 4 bytes
#    int ProcessID;                // 4 bytes
#    char ProcessName[64];         // 64 bytes
#    int ProcessState;             // 4 bytes (// -1 terminate, 0 ready, 1 running, 2 not_start_yet)

# }ProcessTable;

.text
.globl main
main:
###############################################################################################	
	li $t2, 1 # Counter

	li $v0, 4
	la $a0, name_of_the_kernel	# Printing the name of the kernel
	syscall

	addi $a3, $zero, 6	# There are 6 process
	li $v0, 18 # This syscall tell the system, how many process it has
	syscall

###############################################################################################	
# This is for saving the address of init
	li $v0, 9 			# Allocate memory
	li $a0, 204			# 204 bytes
	syscall
	move $s0, $v0 		# $s0 is for ProcessTable struct (for init)

	lw $t0, program_counter # The initial program_counter
	sw $t0, 128($s0) 		# Store the program_counter after the registers
	li $t0, 0 				# The ProcessId for init 
	sw $t0, 132($s0) 		# Store the ProcessId after the program_counter
	la $t0, init			# The beginning address of the init (ProcessName)
	sw $t0, 136($s0)		# Store the ProcessName after the ProcessId
	lw $t0, not_start_yet   # The ProcessState of the init (initially not_start_yet)
	sw $t0, 200($s0) 		# Store the ProcessState after the ProcessName

	move $a0, $s0
	li $v0, 19				# Taking the ProcessTable struct (for init) and saving the address of it with _fork function
	syscall

###############################################################################################	

	# This is for choosing random program and saving the address of it
	jal choose_random_program 

	li $v0, 20 # Start execution and it wait until all processes are terminated. 
	syscall

	j terminate_program	# Jump the terminate_program label to terminate the kernel 
	
choose_random_program:

	addi $a0, $zero, 4 # This 4 means that upper bound for random number. It means that random number can be 0,1,2,3.
	li $v0, 42 # Syscall for creating random number
	syscall
	
	move $t0, $v0	# Copy the random number to $t0
	
	beq $t0, $zero, program_0 # It means that random program is LinearSearch
	j not_program_0

###############################################################################################	
# This is for saving the address of LinearSearch
program_0:
	beq $t2, 6, end_choose_random_program

# ProcessTable struct for LinearSearch
	li $v0, 9 			# Allocate memory
	li $a0, 204			# 204 bytes
	syscall
	move $s0, $v0 		# $s0 is for ProcessTable struct (for LinearSearch)
	
	lw $t0, program_counter # The initial program_counter
	sw $t0, 128($s0) 		# Store the program_counter after the registers
	move $t0, $t2 			# The ProcessId for LinearSearch
	sw $t0, 132($s0) 		# Store the ProcessId after the program_counter
	la $t0, LinearSearch	# The beginning address of the LinearSearch (ProcessName)
	sw $t0, 136($s0)		# Store the ProcessName after the ProcessId
	lw $t0, not_start_yet   # The ProcessState of the LinearSearch (initially not_start_yet)
	sw $t0, 200($s0) 		# Store the ProcessState after the ProcessName

	move $a0, $s0
	li $v0, 19				# Taking the ProcessTable struct (for LinearSearch) and saving the address of it with _fork function
	syscall

	addi $t2, $t2, 1
	j program_0
###############################################################################################	

not_program_0:
	beq $t0, 1, program_1 # It means that random program is BinarySearch
	j not_program_1

###############################################################################################	
# This is for saving the address of BinarySearch
program_1:
	beq $t2, 6, end_choose_random_program

# ProcessTable struct for BinarySearch
	li $v0, 9 			# Allocate memory
	li $a0, 204			# 204 bytes
	syscall
	move $s0, $v0 		# $s0 is for ProcessTable struct (for BinarySearch)
	
	lw $t0, program_counter # The initial program_counter
	sw $t0, 128($s0) 		# Store the program_counter after the registers
	move $t0, $t2 			# The ProcessId for BinarySearch
	sw $t0, 132($s0) 		# Store the ProcessId after the program_counter
	la $t0, BinarySearch	# The beginning address of the BinarySearch (ProcessName)
	sw $t0, 136($s0)		# Store the ProcessName after the ProcessId
	lw $t0, not_start_yet   # The ProcessState of the BinarySearch (initially not_start_yet)
	sw $t0, 200($s0) 		# Store the ProcessState after the ProcessName

	move $a0, $s0
	li $v0, 19				# Taking the ProcessTable struct (for BinarySearch) and saving the address of it with _fork function
	syscall

	addi $t2, $t2, 1
	j program_1
	
###############################################################################################	

not_program_1:
	beq $t0, 2, program_2 # It means that random program is Collatz
	j not_program_2

###############################################################################################	
# This is for saving the address of Collatz
program_2:
	beq $t2, 6, end_choose_random_program

# ProcessTable struct for Collatz
	li $v0, 9 			# Allocate memory
	li $a0, 204			# 204 bytes
	syscall
	move $s0, $v0 		# $s0 is for ProcessTable struct (for Collatz)

	lw $t0, program_counter # The initial program_counter
	sw $t0, 128($s0) 		# Store the program_counter after the registers
	move $t0, $t2			# The ProcessId for Collatz
	sw $t0, 132($s0) 		# Store the ProcessId after the program_counter
	la $t0, Collatz			# The beginning address of the Collatz (ProcessName)
	sw $t0, 136($s0)		# Store the ProcessName after the ProcessId
	lw $t0, not_start_yet   # The ProcessState of the Collatz (initially not_start_yet)
	sw $t0, 200($s0) 		# Store the ProcessState after the ProcessName

	move $a0, $s0
	li $v0, 19				# Taking the ProcessTable struct (for Collatz) and saving the address of it with _fork function
	syscall

	addi $t2, $t2, 1
	j program_2
###############################################################################################	

not_program_2:
	beq $t0, 3, program_3 # It means that random program is Palindrome
	j not_program_2

###############################################################################################	
# This is for saving the address of Palindrome
program_3:
	beq $t2, 6, end_choose_random_program

# ProcessTable struct for Palindrome
	li $v0, 9 			# Allocate memory
	li $a0, 204			# 204 bytes
	syscall
	move $s0, $v0 		# $s0 is for ProcessTable struct (for Palindrome)
	
	lw $t0, program_counter # The initial program_counter
	sw $t0, 128($s0) 		# Store the program_counter after the registers
	move $t0, $t2			# The ProcessId for Palindrome
	sw $t0, 132($s0) 		# Store the ProcessId after the program_counter
	la $t0, Palindrome		# The beginning address of the Palindrome (ProcessName)
	sw $t0, 136($s0)		# Store the ProcessName after the ProcessId
	lw $t0, not_start_yet   # The ProcessState of the Palindrome (initially not_start_yet)
	sw $t0, 200($s0) 		# Store the ProcessState after the ProcessName

	move $a0, $s0
	li $v0, 19				# Taking the ProcessTable struct (for Palindrome) and saving the address of it with _fork function
	syscall
	
	addi $t2, $t2, 1
	j program_3
###############################################################################################
# This label works when the random integer is not in the range
not_program_3:
	li $v0, 4
	la $a0, invalid_program	# Printing error message
	syscall
	j end_choose_random_program

end_choose_random_program: # Returnings back to the caller
	jr $ra
	
###############################################################################################
terminate_program:
	li $v0, 21 # PROCESS_EXIT syscall
	syscall
###############################################################################################