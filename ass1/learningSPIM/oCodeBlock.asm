# rule one of program structure - one entry point, one exit point

start:   ori   $8,$0,4      # $8 = 4
         ori   $9,$0,12     # $9 = 12
midblk:  addu  $10,$8,$9    # $10 = 12+4 = 16
         sll   $10,$10,2    # $10 = 16*4 = 64

         ....

         ori   $8,$0,99     # $8 = 99
         ori   $9,$0,43     # $9 = 43
         j     midblk       # jump to the second statement after start