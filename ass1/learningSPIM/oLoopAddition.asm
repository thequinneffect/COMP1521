##
##  Sum of integers 0 .. 9
##
## Registers:
##  $8  --- loop control
##  $9  --- scratch
## $10  --- sum

init:   ori    $10,$0,0       # sum = 0
        ori    $8,$0,0        # count = 0
test:   sltiu  $9,$8,10       # count < 10
        beq    $9,$0,endLp    # end loop if count >= 10
        sll    $0,$0,0        # delay

        addu   $10,$10,$8     # sum += count

        addiu  $8,$8,1        # count++ ;
        j      test
        sll    $0,$0,0        # delay

endLp:  sll    $0,$0,0        # jump target