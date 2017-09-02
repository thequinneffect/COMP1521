# prog.s ... Game of Life on a NxN grid
#
# Needs to be combined with board.s
# The value of N and the board data
# structures come from board.s
#
# Written by Nicholas Quinn (z5117408), August 2017

					.data

main_ret_save:		.space 	4 							# allocate 4 bytes for main return address
maxiters: 			.word 0
prompt: 			.asciiz "# iterations: "
iteration_msg1: 	.asciiz "=== After iteration "
iteration_msg2: 	.asciiz " ===\n"
alive: 				.asciiz "#"
dead: 				.asciiz "."
newline: 			.asciiz "\n"

###############################################################################################################

					.text
					.globl main

main:
					sw 		$ra, main_ret_save 			# save main return address
						
					la 		$a0, prompt 				# print user prompt to get number of iterations
					jal 	print_msg

					li 		$v0, 5						# read in integer
					syscall

					sw 		$v0, maxiters				# store user input in data (maziters)

					li 		$s0, 1						# initialise outer counter
					lw 		$s3, maxiters 				# $s3 = maxiters
					lw 		$s4, N

	outer_l:		bgt		$s0, $s3, exit_l
					li 		$s1, 0						# initialise middle counter
	
	middle_l:		bge 	$s1, $s4, incr_out
					li 		$s2, 0						# initialise inner counter
	
	inner_l:		bge 	$s2, $s4, incr_mid

					#program code goes here
					####################################
					
					move 	$a0, $s1 # middle			# load arguments for neighbours function
					move 	$a1, $s2 # inner
					jal 	neighbours 					# call neighbours function
					move 	$s5, $v0 					# move return value of neighbours into $s5
					mul 	$t0, $s1, $s4				# calculate decimal offset (i * N)
					add 	$t0, $t0, $s2				# calculate decimal offset (+ j), $t0 is now the current offset
					la 		$s6, board 					# $s6 is base address for board
					add 	$s6, $s6, $t0 				# add on offset	
					la 		$s7, newBoard 				# $s7 is base address for newBoard
					add 	$s7, $s7, $t0

	if_m: 			lw 		$t2, ($s6)
					bne 	$t2, 1, elif_m        		
	
	if_m2:			bge 	$s5, 2, elif_m2
					#newboard[i][j] = 0;
					sw 		$zero, ($s7)
					j 		incr_inn

	elif_m2:		be 		$s5, 2, true
					bne 	$s5, 3, else_m2
	
	true:			#newboard[i][j] = 1;
					li 		$t1, 1
					sw 		$t1, ($s7)
					j 		incr_inn

	else_m2: 		#newboard[i][j] = 0;
					sw 		$zero, ($s7)
					j 		incr_inn

	elif_m: 		bne 	$s5, 3, else_m
					#newboard[i][j] = 1;
					li 		$t1, 1
					sw 		$t1, ($s7)
					j 		incr_inn

	else_m: 		#newboard[i][j] = 0;
					sw 		$zero, ($s7)
					j 		incr_inn					

					####################################

	incr_inn		addi 	$s2, 1
					j 		inner_l
	incr_mid: 		addi 	$s1, 1
					j 		middle_l
	incr_out: 		jal 	output
					jal 	copy_show
					addi 	$s0, 1
					j 		outer_l
	exit_l:			nop

					j 		end_main


	end_main:
					lw   	$ra, main_ret_save 			# load return address of main into return address reg.
					jr   	$ra 						# jump to main caller

###############################################################################################################

neighbours:
					# code
					# 		$a0 = i
					# 		$a1 = j
					move 	$v0, $zero 					# initialise return value to 0
					li 		$t0, -1						# init outer counter
					lw 		$t4, N
					sub 	$t4, $t4, 1

	outer_l1:		bgt 	$t0, 1, ret_n
					li 		$t1, -1

	inner_l1: 		bgt		$t1, 1, incr_out1

					####################################
					add 	$t2, $a0, $t0				# $t2 = i + x
					bltz 	$t2, incr_inn1
					bgt 	$t2, $t4, incr_inn1
					add 	$t3, $a1, $t1 				# $t3 = j + y
					bltz 	$t3, incr_inn1
					bgt 	$t3, $t4, incr_inn1
					bne 	$t0, $zero, if_n
					bne 	$t1, $zero, if_n
					j 		incr_inn1

	if_n:			la 		$t5, board
					mul 	$t6, $t2, $s4
					add 	$t6, $t6, $t3
					add 	$t5, $t5, $t6
					lw 		$t7, ($t5)
					bne 	$t7, 1, incr_inn1
					addi 	$v0, 1


	incr_inn1:		addi 	$t1, 1
					j 		inner_l1

					####################################

	incr_out1:		addi	$t0, 1
					j 		outer_l1

	ret_n: 			jr 		$ra

					#int nn = 0;
   					#for (int x = -1; x <= 1; x++) {
      				#	for (int y = -1; y <= 1; y++) {
         			#		if (i+x < 0 || i+x > N-1) continue;
         			#		if (j+y < 0 || j+y > N-1) continue;
         			#		if (x == 0 && y == 0) continue;
         			#		if (board[i+x][j+y] == 1) nn++;
        			#	}
    				#}
   					#return nn;

###############################################################################################################

copy_show:
					# code
					li 		$t0, 0

	outer_l2:		bge 	$t0, $s4, ret_c  			# $t0 = i
					li 		$t1, 0 						# $t1 = j

	inner_l2:		bge 	$t1, $s4, incr_out2

					####################################

					la 		$t2, newboard
					la 		$t3, board
					mult 	$t4, $t0, $s4
					add 	$t4, $t4, $t1 				# offset is now stored in $t4
					add 	$t2, $t2, $t4
					add 	$t3, $t3, $t4
					lw 		$t5, ($t2)
					sw 		$t5, ($t3)
					lw 		$t5, ($t3)
					#board[i][j] = newboard[i][j];

	if_c: 			bne 	$t5, $zero, else_c
					la 		$a0, dead
					jal 	print_char
					j 		incr_inn2	

	else_c:			la 		$a0, alive	
					jal 	print_char

	incr_inn2:		addi 	$t1, 1
					j 		inner_l2

					####################################
	
	incr_out2: 		la 		$a0, newline
					jal 	print_char 					# TODO: check if this needs to be print string instead 
					addi 	$t0, 1
					j 		outer_l2

	ret_c:			jr 		$ra

					#for (int i = 0; i < N; i++) {
      				#	for (int j = 0; j < N; j++) {
         			#		board[i][j] = newboard[i][j];
         			#		if (board[i][j] == 0)
            		#			putchar('.');
         			#		else
            		#			putchar('#');
      				#	}
      				#	putchar('\n');
   					#}

###############################################################################################################

print_msg:
					li 		$v0, 4
					syscall

###############################################################################################################

print_int: 			li 		$v0, 1
					syscall

###############################################################################################################

print_char: 		li 		$v0, 11
					syscall

###############################################################################################################

output: 			la 		$a0, iteration_msg1
					jal 	print_msg
					la 		$a0, $s0
					jal 	print_int
					la 		$a0, iteration_msg2
					jal 	print_msg
