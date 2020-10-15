.data
my_list: .word 7, 4, -1, 0, 8, 5, -2, 9, -6, 3 # Integer list
# my_list: .word 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
size: .word 10 # Size of my_list
before_sort: .asciiz "The list before sorting : "
after_sort: .asciiz "The list after sorting : "
already_sorted: .asciiz "The list is already sorted!\n"
new_line: .asciiz "\n"
seperator: .asciiz " "

.text
.globl main
main:
###############################################################################################
# Definiation registers
la $t0, my_list # integer list
li $t1, 0 # index of list in print
lw $t2, size # size variable
li $t3, 0 # my_list[i]
li $t4, 0 # my_list[min_index]
li $t5, 0 # already sorted flag
li $t6, 0 # outer loop size
li $t7, 0 # innder loop size
###############################################################################################

###############################################################################################
# Dislaying messages, calling print_list and selection_sort functions
	li $v0, 4
	la $a0, before_sort # Printing before_sort	
	syscall
	jal print_list	# Calling print_list
	
	li $v0, 4
	la $a0, new_line # Printing new_line
	syscall
	
	jal selection_sort # Calling print_list
	
	li $v0, 4
	la $a0, after_sort # Printing after_sort
	syscall
	
	beq $t5, $zero, if_already_sorted # If $t5 = 0 my_list is already sorted
	j not_sorted

if_already_sorted:
	li $v0, 4
	la $a0, already_sorted # Printing already_sorted
	syscall
	j end_if_already_sorted	

not_sorted:
	jal print_list # Calling print_list
	li $v0, 4
	la $a0, new_line # Printing new_line
	syscall
	
end_if_already_sorted:
###############################################################################################

###############################################################################################
# Terminating the program
li $v0, 10
syscall
###############################################################################################

###############################################################################################
# Function that sort the list 
#for (i = 0; i < n - 1; i++) 
#{ 
#        min_index = i; 
#        for (j = i + 1; j < n; j++) 
#        	if (my_list[j] < my_list[min_index]) 
#            		min_index = j;  
#        swap(&my_list[min_index], &my_list[i]); 
#}
selection_sort:
	# Saving registers in stack
  	addi $sp, $sp, -24
  	sw $ra, 0($sp)
  	sw $s0, 4($sp) # It is for inner_loop
  	sw $s1, 8($sp) # It is for outerr_loop
  	sw $s2, 12($sp) # It is for i
  	sw $s3, 16($sp) # It is for j
  	sw $s4, 20($sp) # It is for min_index
  	
  	la $s0, my_list # $s0 points start of my_list
  	la $s1, my_list # $s1 points start of my_list 
  	la $s2, my_list # $s2 points start of my_list
  	la $s3, 4($s2) # $s3 points start of $2 + 4
  	
  	mul $t6, $t2, 4 # Calculation of byte value of my_list for inner # 40
  	addi $t2, $t2, -1 # Decrease the size value
  	mul $t7, $t2, 4 # Calculation of byte value of my_list for outer # 36    
  	add $s0, $s0, $t6 # It means that end of the my_list for inner (n)
  	add $s1, $s1, $t7 # It means that end ot the my_list for outer (n - 1)
	addi $t2, $t2, 1 # Increasing the size value
outer_loop:
	beq $s2, $s1, sort_end # if $s2 reaches the addrees of n - 1
    	move $s4, $s2 # min_index = i
    	
inner_loop:
	beq $s3, $s0, inner_loop_end # if $s3 reaches the addrees of n
	
	lw $t3, 0($s3) # Content of $s3, my_list[j]
	lw $t4, 0($s4) # Content of $s4, my_list[min_index]
	
	blt $t3, $t4, change_min_index # my_list[j] < my_list[min_index]
	addi $s3, $s3, 4 # Incrementing address of inner loop, means that j++
	j inner_loop

change_min_index:
	addi $t5, $t5, 1 # Editing flag for my_list is not sorted
	move $s4, $s3 # min_index = j
	addi $s3, $s3, 4 # Incrementing address of inner loop, means that j++
	j inner_loop

inner_loop_end:
	# Fuction arguments in address locations of array[min_index], array[i] into swap
	move $a1, $s4 # $a1 = &my_list[min_index]
	move $a2, $s2 # $a1 = &my_list[i]
	jal swap # Calling swap function
	addi $s2, $s2, 4 # i++
	addi $s3, $s2, 4 # j = i + 1
	j outer_loop

sort_end:
	# Restore registers from stack
  	lw $ra, 0($sp)
  	lw $s0, 4($sp)
  	lw $s1, 8($sp)
  	lw $s2, 12($sp)
  	lw $s3, 16($sp)
  	lw $s4, 20($sp)
	addi $sp, $sp, 24
	jr $ra
###############################################################################################
# Function that swap of contents of the two registers 	
swap:
  	lw $t3, 0($a1) # Getting my_list[min_index]
  	lw $t4, 0($a2) # Getting my_list[i]
  	
  	sw $t3, 0($a2) # Storing the element
  	sw $t4, 0($a1) # Storing the element
  	
	jr $ra
###############################################################################################

###############################################################################################
# Function that prints the list 
print_list:
	la $t0, my_list # $t0 points start of my_list
	li $t1, 0 # index of my_list
	print_list_loop:
		slt $t3, $t1, $t2 # t3 will be 1 if $t1 is less than $t2, otherwise 0
		beq $t3, $zero, end_loop
		lw $t4, ($t0) # Cureent item
		addi $t0, $t0, 4 # Pointing new position in the my_list
		addi $t1, $t1, 1 # Incrementing index value
		
		li $v0, 1
		la $a0, ($t4) # Pirinting current item of my_list
		syscall
	
		li $v0, 4
		la $a0, seperator # Pirinting seperator
		syscall
		j print_list_loop
		
	end_loop:
		jr $ra
###############################################################################################