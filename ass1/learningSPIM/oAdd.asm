## Program to add two plus three 
        .text					# text directive (everything under here is source code)
        .globl  main			# the symbol main is globally known between all files

main:
        ori     $8,$0,0x2       # put two's comp. two into register 8
        ori     $9,$0,0x3       # put two's comp. three into register 9
        addu    $10,$8,$9       # add register 8 and 9, put result in 10
        
        li 		$v0, 10
        syscall

## End of file