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
debug_print:		.asciiz "code\n"

###############################################################################################################

					.text
					.globl main

main:
					sw 		$ra, main_ret_save 			# save main return address

					li 		$v0, 4 						# print user prompt
					la 		$a0, prompt
					syscall

					li 		$v0, 5						# read in integer
					syscall

					sw 		$v0, maxiters				# store user input in data

					li 		$s0, 1						# initialise outer counter
					lw 		$s3, maxiters 				# load maxiters into register
					li 		$s4, 10 					# TODO: this need to be changed to lw N into register from other file

	outer_l:		bgt		$s0, $s3, exit_l
					li 		$s1, 0						# initialise middle counter
	middle_l:		bge 	$s1, $s4, incr_out
					li 		$s2, 0						# initialise inner counter
	inner_l:		bge 	$s2, $s4, incr_mid

					#program code goes here
					##############################################################
					li 		$v0, 4
					la 		$a0, debug_print
					syscall
					##############################################################

					addi 	$s2, 1
					j 		inner_l
	incr_mid: 		addi 	$s1, 1
					j 		middle_l
	incr_out: 		addi 	$s0, 1
					j 		outer_l
	exit_l:			nop

					j 		end_main


	end_main:
					lw   $ra, main_ret_save 			# load return address of main into return address reg.
					jr   $ra 							# jump to main caller


###############################################################################################################

neighbours:

###############################################################################################################

copy_show: