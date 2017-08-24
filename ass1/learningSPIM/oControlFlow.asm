## absVal.asm
##
## Calculate the absolute value of A
##
## Registers:
##   $8  --- A, two's comp. integer
##   $9  --- sign bit of A
##   $10 --- base register for .data section

        .text
        .globl  main

main: 
# Get A
        lui   $10,0x1000     #  Init base register
        lw    $8,0($10)      #  Load A
        sll   $0,$0,0

# Is A Negative?
        srl   $9,$8,31       #  Shift sign bit to position 0
        beq   $0,$9,done     #  sign bit == zero, done
        sll   $0,$0,0

# Store -A
        subu  $8,$0,$8       #  negate A
        sw    $8,0($10)      #  save it

done:   sll   $0,$0,0        #  target of the branch

        .data
A:      .word   -1

## End of File