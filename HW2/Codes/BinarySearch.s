.data
my_list: .word -60, -20, -10, 0, 30, 40, 50, 70, 80, 90 # Sorted integer list
size: .word 10 # Size of my_list
request: .asciiz "\n--BinarySearch.asm-- Enter a target to search in the list : "
contains: .asciiz "\n--BinarySearch.asm-- Target is in the my_list["
square_bracket: .asciiz "]\n"
not_contains: .asciiz "\n--BinarySearch.asm-- Target is not in the list!\n"
new_line: .asciiz "\n"

.text
.globl main
main:
###############################################################################################
# Definiation of registers
li $t0, 0 # target number
li $t1, 0 # index of list
la $t2, my_list # sorted integer list
li $t3, -1 # flag for list does not contains target
lw $t4, size # size variable
li $t5, 0 # left index
li $t6, 0 # right index
##############################################################################################

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
# while (left <= right) 
#{ 
#        int middle_index = (left + right) / 2; 
#        
#        if (my_list[middle_index] == target_item) 
#            return middle_index; 
#        
#        if (my_list[middle_index] < target_item) 
#            left = middle_index + 1; 
#             
#        else
#            right = middle_index - 1; 
#} 
# Operations
li $t5, 0 # left = 0
addi $t6, $t4, -1 # right = size - 1
my_loop:
	la $t2, my_list # Making first address of my_list to $t2
	bgt $t5, $t6, end_loop # if right > left then terminate the loop
	
	add $t8, $t5, $t6 # left + right
	div $t8, $t8, 2 # (left + right) / 2
	
	move $s0, $t8 # middle_index
	mul $t8, $t8, 4 # Words are 4 bytes, to found index in terms of words multiply index value by 4 
	add $t2, $t2, $t8 # Adding words value to first address of my_list
	lw $s1, 0($t2) # my_list[middle_index] 
	
	beq $s1, $t0, found_item # if my_list[middle_index] = target_item
	j continue_if1 # Otherwise
	
found_item:
	addi $t3, $t3, 1 # Editing flag for my_list does not contains target
	li $v0, 4
	la $a0, contains # Printing contains
	syscall
	
	addi $a0, $s0, 0 # Editing index value
	li $v0, 1 # Printing index value
	syscall

	li $v0, 4
	la $a0, square_bracket # Printing square_bracket
	syscall
	j end_loop # Ending loop
	
continue_if1:
	blt $s1, $t0, go_to_rigth # if my_list[middle_index] < target_item
	j continue_if2 # Otherwise

go_to_rigth:
	addi $s0, $s0, 1 # Incrementing middle_index by 1
	move $t5, $s0 # Assignment the value of middle_index to left
	j my_loop # Jump to begining of my_loop
	
continue_if2:
	bge $s1, $t0, go_to_letf # if my_list[middle_index] >= target_item
	j my_loop # Jump to begining of my_loop
	
go_to_letf:
	addi $s0, $s0, -1 # Decreasing middle_index by 1
	move $t6, $s0 # Assignment the value of middle_index to right
	j my_loop  # Jump to begining of my_loop
	
end_loop:
	bne $t3, -1, end_if # If $t3 = -1 target item is not in the list

	li $v0, 4
	la $a0, not_contains # Printing not_contains
	syscall

end_if:
###############################################################################################

###############################################################################################
# Terminating the program
li $v0, 24 # This is new call to exit and execute new process
# li $v0, 10 # To run this program on the spim not on the SPIMOS_GTU_X.s
syscall
##############################################################################################