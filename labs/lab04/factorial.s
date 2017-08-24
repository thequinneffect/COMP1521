# factorial version of fac

               .data

prompt1:       .asciiz "enter an int:"
prompt2:       .asciiz "factorial is:"
lf:            .asciiz "\n"


               .text
               .globl  main
main:
                                  # prolog        
         sub      $sp,$sp,4        #   1. Push return address
         sw       $ra,($sp)
         sub      $sp,$sp,4        #   2. Push caller's frame pointer
         sw       $fp,($sp)
                                  #   3. No S registers to push
         sub      $fp,$sp,8        #   4. $fp = $sp - space_for_variables
         move     $sp,$fp          #   5. $sp = $fp

                                  # write("enter an int:")
         li       $v0,4             #   print string service
         la       $a0,prompt1       #   address of prompt
         syscall
                                  # read( a )
         li       $v0,5             #   read integer service
         syscall                  #   $v0 gets the integer
         sw       $v0,0($fp)        #   save in variable a
                        
                                  # subroutine call
                                  #   1. No T registers to push
         lw       $a0,0($fp)       #   2. Put argument into $a0
         jal      fact             #   3. Jump and link to subroutine
         
                                  # return from subroutine 
                                  #   1. No T registers to restore
                                  
         sw       $v0,4($fp)        # b = fact( a )
        
                                  # write("factorial is:")
         li       $v0,4             #   print string service
         la       $a0,prompt2       #   address of prompt
         syscall
                                  # print( b )
         lw       $a0,4($fp)        # load a into $a0
         li       $v0,1             # print integer service
         syscall                                  

                                  # end the print line
         li       $v0,4             #   print string service
         la       $a0,lf            #   address of line feed
         syscall
                                  # epilog
                                  #   1. No return value         
         add      $sp,$fp,8        #   2. $sp = $fp + space_for_variables       
                                  #   3. No S registers to pop      
         lw       $fp,($sp)        #   4. Pop $fp
         add      $sp,$sp,4        #           
         lw       $ra,($sp)        #   5. Pop $ra
         add      $sp,$sp,4        #                                    
         jr       $ra              # return to OS 


############################################################################################################


               .text
               .globl  fact
fact:
                                  # prolog        
         sub      $sp,$sp,4        #   1. Push return address
         sw       $ra,($sp)
         sub      $sp,$sp,4        #   2. Push caller's frame pointer
         sw       $fp,($sp)
         sub      $sp,$sp,4        #   3. Push register $s1
         sw       $s1,($sp)
         sub      $fp,$sp,0        #   4. $fp = $sp - space_for_variables (==0)
         move     $sp,$fp          #   5. $sp = $fp
         
                                  # body of subroutine
         move     $s1,$a0          # save argument in $s1
         li       $t1,1            # get a 1
         bgt      $s1,$t1,recurse  # if ( n<=1)
         li       $v0,1            #   return 1
         b        epilog    

recurse:                          # else
                                  #  return n*fact(n-1)
         sub      $a0,$s1,1        #     n-1
                       
                                  # subroutine call
                                  #   1. No T registers to push
                                  #   2. Argument is in $a0 
         jal      fact             #   3. Jump and link to subroutine

         mul      $v0,$v0,$s1      # n*fact(n-1)
                            
epilog:                           # epilog
                                  #   1. Return value is already in $v0        
         add      $sp,$fp,0        #   2. $sp = $fp + space_for_variables (==0)
         lw       $s1,($sp)        #   3. Pop register $s1
         add      $sp,$sp,4        #          
         lw       $fp,($sp)        #   4. Pop $fp
         add      $sp,$sp,4        #           
         lw       $ra,($sp)        #   5. Pop $ra
         add      $sp,$sp,4        #            
         jr       $ra              #   6. return to caller 

         # end prog.