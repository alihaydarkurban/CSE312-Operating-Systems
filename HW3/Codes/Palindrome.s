# Checking the words with their lower-case terms. 
# For example "Anna" is checked like "anna" so that it is palindrome or 
# "EyEdipAdanadaPideYe" is checked like "eyedipadanadapideye" so that it is palindrome too.

# This code include command lines such as START CUSTOM CHOICE and END CUSTOM CHOICE
# If you delete the code that are range of these command lines, then the program work with case sensitive
.data
new_line: .asciiz "\n"
colon: .asciiz ": "
palindrome_true: .asciiz ": Palindrome"
palindrome_false: .asciiz ": Not Palindrome"
question: .asciiz "\nDo you want to continue (y/n)? "
request: .asciiz "Please enter the last word: "
bye: .asciiz "\nGoodbye....\n"
dictionary: 
.asciiz "ali", "haydar", "kurban", "medine", "aslan", "loveevol", "gtu", "operating", "systems", "homework3"
.asciiz "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "EyEdipAdanadaPideYe" 
.asciiz "word", "syscall", "true", "false", "loop", "move", "palindrome", "ege", "jump", "check"
.asciiz "computer", "programing", "software", "hardware", "mips", "assembly", "C", "C++", "java", "python"
.asciiz	"anna", "January", "February", "March", "April", "May", "June", "July", "August", "September"
.asciiz "Monday", "Tuesday", "Wednesday", "Thursay", "Friday", "Saturday", "Sunday", "Gunday", "refer", "why"
.asciiz "fiat", "bmw", "mercedes", "rotor", "nissan", "renault", "opel", "tesla", "skoda", "hundai"
.asciiz "apple", "samsung", "huawei", "LG", "stats", "vestel", "blackbary", "xiaomi", "casper", "nokia"
.asciiz "Adana", "madam", "Afyonkarahisar", "Aksaray", "Amasya", "Ankara", "Antalya", "Ardahan", "Van", "Siirt"
.asciiz "whatsapp", "instagram", "kayak", "twitter", "netflix", "spotify", "youtube", "google", "github", "udemy"

size: .word 100
yes: .asciiz "y"
y_n_input: .space 4
user_input: .space 64

.text
.globl main
main:
	la $s0, dictionary 	# Base address of dictionary
	lw $s1, size	   	# Size of words inside the dictionary	
	li $t2, 1		# Counter of words

###############################################################################################																							
# This is main loop for traversing the words of dictionary
loop:
	jal my_strlen	# Calling my_strlen function ($st0 will be used)
	move $t0, $v0	# $t0 is length of the word 
	
	# Getting back the address of word to the head
	sub $s0, $s0, $t0 
	sub $s0, $s0, 1
	
	# Printing counter
	li $v0, 1
	la $a0, ($t2)
	syscall

	# Printing ":"
	li $v0, 4
	la $a0, colon
	syscall

	# Printing the current word
	li $v0, 4
	move $a0, $s0	# $s0 is begining address of the word to print
	move $a1, $s0 	# Function argument for is_palindrome FORWARD
	move $a2, $s0   # Function argument for is_palindrome BACKWARD
	syscall
	
	# Calculating the address of next word
	add $s0, $s0, $t0
	add $s0, $s0, 1
	 
	beq $t0, 1, print_palindrome_true	# If the length of the word is 1 that means that it is palindrome
	
	jal is_palindrome 	# Calling is_palindrome function ($a1, $a2 and $t0 will be used)
	
	move $t5, $v0	# Return value of is_palindrome function 
	beq $t5, $zero print_palindorme_false 	# The current word is not palindrome
	bne $t5, $zero print_palindrome_true	# The current word is palindrome


print_palindorme_false:	# Printing information about the not palindrome
	li $v0, 4
	la $a0, palindrome_false
	syscall
	j continue
		
print_palindrome_true:	# Printing information about the palindrome
	li $v0, 4
	la $a0, palindrome_true
	syscall
	j continue
	
continue: # The other operations of label of "loop"
	addi $t2, $t2, 1	# Incrementing the counter
	bgt $t2, $s1, end_loop	# Checking that the counter reaches the higest value for it
	
	# Printing "\n"
	li $v0, 4
	la $a0, new_line
	syscall 
	j loop	# Jump the loop
	
end_loop: # When the counter reaches the higest value for it
	
	# Printing information
	li $v0, 4
	la $a0, question
	syscall
	
	# Getting (y/n) from user 
	li $v0, 8
	la $a0, y_n_input
	la $a1, 4
	la $a2, yes
	syscall

	lbu $t5, 0($a0) # Gettting fist content of $a0 
	lbu $t6, 0($a2) # Gettting fist content of $a2

	beq $t5, $t6, one_more_operation # If the character is equal to 'y' than goes one_more_operation
	j terminate_program # Jump the terminate_program label to terminate the program
###############################################################################################																						


###############################################################################################																							
# This part is for extra palindrome string check
one_more_operation:
	# Printing information
	li $v0, 4
	la $a0, request
	syscall
	
	# Getting string from user
	li $v0, 8
	la $a0, user_input
	move $a3, $a0
	la $a1, 64
	syscall
	
	# Printing counter
	li $v0, 1
	la $a0, ($t2)
	syscall

	# Printing ":"
	li $v0, 4
	la $a0, colon
	syscall
	
	move $s0, $a3
	jal my_strlen	# Calling my_strlen function ($st0 will be used)
	move $t0, $v0	# $t0 is length of the word 
	
	# Getting back the address of word to the head
	sub $s0, $s0, $t0
	sub $s0, $s0, 1
	
	sub $t0, $t0, 1 # This is for ignoring '\n' character of got string from the user
	
	li $s5, 0

print_loop: # This loop is printign the user_input without '\n' character
	beq $s5, $t0, continue2		
	li $v0, 11
	lbu $a0, ($a3)
	syscall
	add $a3, $a3, 1
	addi $s5, $s5, 1
	j print_loop # Jump the print_loop

continue2: # The other operations of label of "one_more_operation"
	beq $t0, 1, print_palindrome_true_last # If the length of the word is 1 that means that it is palindrome
			
	move $a1, $s0 	# Function argument for is_palindrome FORWARD
	move $a2, $s0   # Function argument for is_palindrome BACKWARD

	jal is_palindrome # Calling is_palindrome function ($a1, $a2 and $t0 will be used)
	move $t5, $v0 # Return value of is_palindrome function 
	
	beq $t5, $zero print_palindorme_false_last # The user_input is not palindrome
	bne $t5, $zero print_palindrome_true_last  # The user_input is palindrome

print_palindorme_false_last: # Printing information about the not palindrome
	li $v0, 4
	la $a0, palindrome_false
	syscall
	j terminate_program # Jump the terminate_program label to terminate the program
		
print_palindrome_true_last: # Printing information about the palindrome
	li $v0, 4
	la $a0, palindrome_true
	syscall
	j terminate_program # Jump the terminate_program label to terminate the program
###############################################################################################																						


###############################################################################################
# This is a function that calculate the length of the string 																				
my_strlen:
	li $t0, 0	# This is length counter of the strign
strlen_loop:
	lbu $t1, ($s0) 	# Gettting fist content of $s0 
	add $s0, $s0, 1 # Incrementing index
	beqz $t1, end_strlen_loop # If item = null then end of the function
	add $t0, $t0, 1 # Incrementing the length counter
	j strlen_loop	# Jump the strlen_loop
		
end_strlen_loop:
	move $v0, $t0	# Return the length of the string with $v0
	jr $ra			# Return the caller
###############################################################################################		


###############################################################################################
# This is a function that chech the string is palindrome or not
# If the string is palindrome then it returns 1 otherwise returns 0 (returning with $v0)
is_palindrome:
	li $t5, 1	# $t5 = 1 means that true, $t5 = 0 means that false. (Initially 1)
	li $t6, 0 	# palindrome_loop counter
	
	div $t3, $t0, 2 # $t3 is the loop size (integer division)
	
	# Calculating BACKWARD position
	add $a2, $a2, $t0 
	sub $a2, $a2, 1
	
palindrome_loop:
	lbu $t4, ($a1)	 # Gettting forward item	
	lbu $t7, 0($a2)  # Getting backward item

# START CUSTOM CHOICE #########################################################################	
	ble $t4, 90, make_lower_case_1 # If the character ow the word is upper case, then make it lower case.
lower_case_1:

	ble $t7, 90, make_lower_case_2 # If the character ow the word is upper case, then make it lower case.
lower_case_2:
# END CUSTOM CHOICE ###########################################################################
	
	bne $t4, $t7 not_palindrome # If the forward item and backword item not equal each other that means it is not palindrome 

	# Calculating next address of the string for both FORWARD and BACKWARD
	add $a1, $a1, 1
	sub $a2, $a2, 1
	
	addi $t6, $t6, 1 # Incrementing the palindrome_loop counter
	
	bne $t3, $t6, palindrome_loop # The palindrome_loop counter does not reach the higest value for it
	
	j end_is_palindrome
	
not_palindrome:
	li $t5, 0 # Making return value 0
	j end_is_palindrome

end_is_palindrome:
	move $v0, $t5 # Assinging the return value
	jr $ra

# START CUSTOM CHOICE #########################################################################		
make_lower_case_1:
	addi $t4, $t4, 32 # Adding 32 to the ASCII code of the character makes it lower case.
	j lower_case_1

make_lower_case_2:
	addi $t7, $t7, 32 # Adding 32 to the ASCII code of the character makes it lower case.
	j lower_case_2
# END CUSTOM CHOICE ###########################################################################

###############################################################################################

###############################################################################################			
# Terminating the program
terminate_program:
	# Printing bye message
	li $v0, 4
	la $a0, bye
	syscall

	# li $v0, 10 # To run this program on the spim not on the SPIMOS_GTU_X.s
	li $v0, 22
	syscall
###############################################################################################