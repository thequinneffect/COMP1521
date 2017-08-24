## The program adds +146 to -82, leaving the result in $10
# note: there are MUCH better ways to load a register with a negative value

        .text
        .globl  main

main:
        ori      $7, $0, 146        # put +146 into $7
        ori      $8, $0,  82        # put 82 into $8
        nor      $8, $8,  $0        # reflect
        ori      $9, $0,   1        # 
        addu     $8, $8,  $9        # add 1: $8 = -82
        addu    $10, $7,  $8        # (+146) + (-82)

## End of file