.data
my_list: .word 70, 40, -10, 0, 80, 50, -20, 90, 60, 30 # Integer list
size: .word 10 # Size of my_list
request: .asciiz "\n--LinearSearch-- Enter a target to search in the list : "
contains: .asciiz "\n--LinearSearch-- Target is in the my_list["
square_bracket: .asciiz "]\n"
not_contains: .asciiz "\n--LinearSearch-- Target is not in the list!\n"
new_line: .asciiz "\n"

.text
.globl main
main:
###############################################################################################
# Definiation of registers
li $t0, 0 # target number
li $t1, 0 # index of list
la $t2, my_list # integer list
li $t5, -1 # flag for list does not contains target
lw $t6, size # size variable
###############################################################################################

###############################################################################################	
# Dislaying the messages and initialization the registers of numbers
	li $v0, 4
	la $a0, request	# Printing request
	syscall
	
	# Initialization of the target number
	li $v0, 5
	syscall
	move $t0, $v0
	###############
###############################################################################################	

###############################################################################################
# Operations
#for (i = 0; i < n; i++)
#{
#        if (my_list[i] == target_item) 
#            return i;
#}
my_loop:
	slt $t3, $t1, $t6 # t3 will be 1 if $t1 is less than $t6, else it will be 0
	beq $t3, $zero, end_loop # if $t3 = 0
	lw $t4, ($t2) # Current item
	addi $t2, $t2, 4 # Pointing new position in the my_list
	addi $t1, $t1, 1 # Incrementing index value
	
	bne $t0, $t4, continue # Current item != target item
	addi $t5, $t5, 1 # Editing flag for my_list does not contains target
	li $v0, 4
	la $a0, contains # Printing contains
	syscall
	
	addi $a0, $t1, -1 # Editing index value
	li $v0, 1 # Printing index value
	syscall

	li $v0, 4
	la $a0, square_bracket # Printing square_bracket
	syscall
	j end_loop # Ending loop

	
continue:
	j my_loop
	
end_loop:
	bne $t5, -1, end_if # If $t5 = -1 target item is not in the list

	li $v0, 4
	la $a0, not_contains # Printing not_contains
	syscall

end_if:
###############################################################################################

###############################################################################################
# Terminating the program
# li $v0, 10 # To run this program on the spim not on the SPIMOS_GTU_X.s
li $v0, 22
syscall
###############################################################################################