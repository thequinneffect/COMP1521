## Program to logical shift left a pattern
        .text
        .globl  main

main:
        ori      $8, $0, 0x6F       # put bit pattern into register $8
        sll      $9, $8, 2          # shift left logical by two
        
        li $v0 10
        syscall

## End of file