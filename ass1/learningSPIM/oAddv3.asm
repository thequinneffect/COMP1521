## Program to demonstrate addition (using non hex constants)
        .text
        .globl	main

main:
        ori      $8, $0, 171       # put 171 into $8
        ori      $9, $0,  85       # put  85 into $9
        addu     $10,$9,  $8       # $10 <—— sum
        
        li			$v0, 10
        syscall

## End of file