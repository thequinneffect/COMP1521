# prog.s ... Game of Life on a NxN grid
#
# Needs to be combined with board.s
# The value of N and the board data
# structures come from board.s
#
# Written by Nicholas Quinn (z5117408), August 2017

					.data

N: 					.word 	10 							# changing this value is equivalent to #define NN change
main_ret_save:		.space 	4 							# allocate 4 bytes for main return address
prompt: 			.asciiz "# iterations: "
iterations: 		.word 0

###############################################################################################################

					.text
					.globl main

main:
					sw 		$ra, main_ret_save 			# save main return address

					li 		$v0, 4 						# print prompt
					la 		$a0, prompt
					syscall

					li 		$v0, 5						# read in integer
					syscall

					sw 		$v0, iterations

					j 		end_main


end_main:
					lw   $ra, main_ret_save 			# load return address of main into return address reg.
					jr   $ra 							# jump to main caller


###############################################################################################################

neighbours:

###############################################################################################################

copy_show: