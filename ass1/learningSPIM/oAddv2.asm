## Program to demonstrate addition (using hex constants)
        .text
        .globl	main

main:
        ori      	$8, $0, 0xAB       # put 0x000000AB into $8
        ori     	$9, $0, 0x55       # put 0x00000055 into $9
        addu 	    $10,$9, $8         # $10 <—— sum
        
        li			$v0, 10
        syscall

## End of file