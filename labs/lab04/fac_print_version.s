# COMP1521 Lab 04 ... Simple MIPS assembler

               .data

main_ret_save: .word 4
fac_ret_save:  .space 4
msg1:          .asciiz "n: "
msg2:          .asciiz "n! = "
eol:           .asciiz "\n"

               .text
               .globl main

main:
         sw    $ra, main_ret_save               # save ret. link to __start: 

         # start of body
         
         la    $a0, msg1                        # load the address of first byte of string  
         jal   printS                           # jump and link to print string sub routine
         nop                                    # making compatable for actual MIPS architecture
         
         jal   readW                            # jump and link to read word subroutine
         nop                                    
         move $a0, $v0                          # copy contents of reg. $v0 to reg. $a0

         jal   fac                              # jump and link to fac
         nop                                    

         move $t0, $v0                          # move returned value from fac into temp reg
   
         la    $a0, msg2                        # load the address of first byte of string  
         jal   printS                           # jump and link to print string sub routine
         nop

         move  $a0, $t0                         # move returned value into argument reg. $a0
         jal printW                             # jump and link to print word sub routine
         nop
   
         la    $a0, eol                         # load the address of first byte of string  
         jal   printS                           # jump and link to print string sub routine
         nop
         # end of body

         li    $v0, 10                          # shutdown program instead of returning to start
         syscall
         #lw    $ra, main_ret_save
         #jr    $ra          

##########################################################################################################

fac:
         sw    $ra, fac_ret_save

         # start of body
         li    $t0, 1                           # let $t0 be the counter register (i)
         li    $t1, 1                           # let $t1 be the result register

   sFor: bgt   $t0, $a0, eFor                   # (for (i = 0; i <= n; i++))
         mul   $t1, $t1, $t0                    # f *= i
         addi  $t0, 1                           # increment i
         j     sFor                             # test condition again (loop)
         nop

   eFor: move  $v0, $t1                         # store result in return value reg. $v0     

         lw    $ra, fac_ret_save
         jr    $ra 
         nop       

##########################################################################################################

printS: 
         li    $v0, 4                           # print string syscall code     
         syscall                                # give OS control, print, then return control back to MIPS
         jr    $ra
         nop

##########################################################################################################

readW:     
         li    $v0, 5                           # read int syscall code
         syscall                                # give OS control, read int, then return control back to MIPS
         jr    $ra
         nop

##########################################################################################################

printW:  
         li    $v0, 1                           # print int syscall code
         syscall                                # give OS control, read int, then return control back to MIPS
         jr    $ra
         nop