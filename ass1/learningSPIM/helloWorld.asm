		.data
msg:	.asciiz	"hello world"
		
		.text
		
main:
		li $v0 4 # load immediately, the print string service value, into register value0
		la $a0 msg # load address of msg: into register argument0
		syscall # complete given instructions
		
		li $v0 10 # load immediately, the exit program service value, into register value0
		syscall
		