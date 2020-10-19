.data
seperator: .asciiz " "
result_input: .asciiz "Collatz conjecture "
colon: .asciiz ": "
new_line: .asciiz "\n"

.text
.globl main
main:
###############################################################################################	
# This is main function.
# It displays the messages.
# It creates 2 loops and call function to calculate collatz conjecture.

li $t2, 1 # It is for counter of number from 1 to 25. (Including 1 but not including 25) 
	
# This loop works if $t2 < 25. It is for counting of collatz's number.
outer_loop:
	beq $t2, 25, terminate_program
	# This part woks if the number is smaller than 25.
	li $v0, 4
	la $a0, result_input # Printing result_input
	syscall	
	
	li $v0, 1
	la $a0, ($t2) # Printing number
	syscall
	
	li $v0, 4
	la $a0, colon # Printing colon
	syscall	
	
	move $t0, $t2 # Making copy of number to calculate ollatz conjecture.
	
# This loop works until the number will become 1
inner_loop:
	add $a0, $t0, $zero # Assign the value of $t0 to $a0 (Function parameter)
	jal even_or_odd_number # Calling even_or_odd_number function
	move $t0, $v0 # Moving result of the ieven_or_odd_number to $t0
	bne $t0, 1, inner_loop # If $t0 != 1 then goto inner_loop again

	# This part work when the number become 1
	li $v0, 4
	la $a0, new_line # Printing new_line
	syscall
	
	addi $t2, $t2 ,1 # Inceremting the number counter.
	j outer_loop
###############################################################################################


###############################################################################################
# This checks that the number is even or odd. 
# If the number is even then number =  number / 2 and return number
# If the number is odd then number = 3 * number + 1 and return number
even_or_odd_number:
	li $v0, -1
	add $t1, $a0, $zero # Assign the function parameter to $t1 
	
	rem $t3, $t1, 2 # Modulo operation ($t3 = $t1 % 2) 

	beq $t3, $zero, even # If $t3 is equal to 0 then goto even
	j odd # If $t3 is not to 0 then goto odd

even:
	div $t1, $t1, 2 # number = number / 2
	j end_even_or_odd_number # Goto end_even_or_odd_number

odd:
	mul $t1, $t1, 3 # number = number * 3
	addi $t1, $t1, 1 #number = number  + 1
	j end_even_or_odd_number # Goto end_even_or_odd_number

end_even_or_odd_number:

	li $v0, 1
	la $a0, ($t1) # Printing number
	syscall
	
	li $v0, 4
	la $a0, seperator # Printing seperator
	syscall
	
	add $v0, $t1, $zero # Make the return value $t1
	jr $ra # Goto back where it was called from

###############################################################################################

###############################################################################################
terminate_program:
# Terminating the program
li $v0, 24 # This is new call to exit and execute new process
# li $v0, 10 # To run this program on the spim not on the SPIMOS_GTU_X.s
syscall
###############################################################################################
