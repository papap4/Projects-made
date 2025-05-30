# manos papadakis
#csd4805	

# NOTE TO REMEMBER (irrelevant to the excersise)
# Rars1_6 was released. I am sure nothing have change code wise, BUT
# Now there is the option change the coding screen's color to something, lets say DARK THEMED instead of the eye piercing white it has
	.data
read_line: 	.asciz "\nGive me a number: "
n_num_line: 	.asciz "\n------------------------------------------Phase 2 : ----------------------------------------------"
dash_line: 	.asciz " "
	.text
main:
	sw s0, 0(sp)			# we dont know if our programm is the main or just a called function
	sw s1, -4(sp)			# therefore, we save the saved register we use and we will restore them at the end
	sw s2, -8(sp)			
	addi sp, sp, -12		# and then we set sp to point to the next free cell it has
	
	jal  node_alloc 	# Goes and does noce_alloc sub-programm and then it continues from here and after
	
	add s0, a0, x0		# a0 is s0 which contains the pointer to the firstnode of list
		
	sw x0, 0(s0)		# dummy node
	sw x0, 4(s0) 
	add s1, a0, x0		# s1 has the end of the pointer, but since the list is empty, the start is also the end, for now
	
	
read_connect_loop:		# the loop starts here
	jal read_int
	
	add t0, x0, a0		# we pass the value of a0 to t0
	ble t0, x0, end_of_part_1# if the value we just passed is zero, then the first part ends
	
	jal ra, node_alloc	# otherwise, we make room with node_alloc in the memory
	
	sw a0, 4(s1)		# we set the last pointer to a0 which is the place were we freed some memory(in the heap), aka the new node
	sw t0, 0(a0)		# the value of the new node
	sw x0, 4(a0)		# and then the pointer to the last node is made null, because it is the end of the list
	add s1, x0, a0		# we set to s1 the pointer to the last node
	
	j read_connect_loop	# a loop is performed here
	
end_of_part_1:
	
	sw x0, 4(s1)		# set the last pointer to null
		
part_2:	
	add s2, s0, x0		
	
	addi a7, x0, 4		# 4 is the ecall for printing a string
	la a0, n_num_line	# is the argument of the ecall function code 4, so we load the string to a0
	ecall			# System checks a7 to see which function will perform, then checks arguments (in our case a0 and only a0) to load the string
	
	jal read_int		# function that reads an integer
	add s1, x0, a0		# we set s1 value as a0, because from read_int function, a0 is the returning argument, aka the number we just read
	
	jal search_loop	
	j part_2
	
search_loop:
	beq s2, x0, return	# if value of s2 is zero then go to part2
	blt s1, x0, exit	# if s1, which is the number n for the 2nd part of the exercise, is less than zero, then end the programm (goes to exit)
		
	

	addi a7, x0, 4				
	la a0, dash_line		
	ecall
	
	lw t1, 0(s2)		#load the value of the first node to t0
			
	sw ra,  0(sp)
	sw s2, -4(sp)
	
	addi sp, sp, -8
	jal print_node
	addi sp, sp, 8
	
	lw ra, 0(sp)
	lw s2, -4(sp)
	
	lw s2, 4(s2)
	
	j search_loop
#----------------------------------------
node_alloc:
	addi a7, x0, 9	#Sbrk allocate heap memory
			# a0 (x10) takes as input how many bytes we want to free
			# And then returns the pointer from the memory that we have freed to a0
	addi a0, x0, 8	# a0 is the input for Sbrk and it contains the amount of space we want to free
			# So 8 is for 8 Bytes
	ecall		# we use ecall to execute Sbrk and free the space we need
	
	jr ra		# ra (aka return address) tells where to continue our programm
			# so it will continue from where we left off
			
	
read_int:
	addi a7, x0, 4				
	la x10, read_line		
	ecall
	 
	addi a7, x0, 5
	ecall	

	jr ra
	
print_node:
	lw a0, 0(s2)
	add s2, x0, a0
	ble a0, s1, return
	
	addi a7, x0, 1
	ecall
		
	j return
	
return:					
	jr ra
	
exit:			# plays a few notes when the program ends, also restore the save reg's value
	addi sp, sp, 12
	lw s0, 0(sp)			
	lw s1, -4(sp)			
	lw s2, -8(sp)	
	
	
	
	
	addi a7, x0, 33
	
	addi a0,x0, 57
	addi a1, x0, 200
	addi a2, x0, 2
	addi a3, x0, 20
	
	ecall
	
	addi a0, x0, 60
	ecall
	
	addi a0, x0, 62
	ecall
	
	addi a0, x0, 64
	ecall
	
	addi a0, x0, 66
	ecall
	
	addi a0, x0, 67
	ecall
	
	addi a0, x0, 69
	ecall
	
	addi a7, x0, 10
	ecall
