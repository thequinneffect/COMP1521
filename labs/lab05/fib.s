# MIPS assembler to compute Fibonacci numbers
            .data
msg1:       .asciiz "n = "
msg2:       .asciiz "fib(n) = "
err_msg:    .asciiz "n must be > 0\n"
newline:    .asciiz "\n"

# int main(void)
# {
#    int n;
#    printf("n = ");
#    scanf("%d", &n);
#    if (n >= 1)
#       printf("fib(n) = %d\n", fib(n));
#    else {
#       printf("n must be > 0\n");
#       exit(1);
#    }
#    return 0;
# }
            .text
            .globl main
            
main:
            # prologue
            addi $sp, $sp, -4
            sw   $fp, ($sp)
            move $fp, $sp
            addi $sp, $sp, -4
            sw   $ra, ($sp)

            # function body
            la   $a0, msg1       # printf("n = ");
            li   $v0, 4
            syscall

            li   $v0, 5          # scanf("%d", &n);
            syscall
            move $a0, $v0

            bge      $a0, 1, continue
            li       $v0, 4
            la       $a0, err_msg
            syscall
            li       $v0, 1
            j        epilogue
            nop
continue:
            jal  fib             # $s0 = fib(n);
            nop
            move $s0, $v0

            la   $a0, msg2       # printf((fib(n) = ");
            li   $v0, 4
            syscall

            move $a0, $s0        # printf("%d", $s0);
            li   $v0, 1
            syscall

            la   $a0, newline
            li   $v0, 4
            syscall
            
epilogue:   # epilogue
            li $v0, 10
            syscall
         
            lw   $ra, ($sp)
            addi $sp, $sp, 4
            lw   $fp, ($sp)
            addi $sp, $sp, 4
            jr   $ra

######################################################################

fib:
            # prologue
            sub   $sp, $sp, 4
            sw       $ra, ($sp)
            sub   $sp, $sp, 4
            sw       $fp, ($sp)
            
            sub   $sp, $sp, 4 # our var is accessed as 4($fp)
            move  $fp, $sp
               

            # function body
            sw       $a0, ($fp)
               
fib_if:           
            bge      $a0, 1, fib_elif
            li       $t3, 0
            add   $s7, $s7, $t3 # ???
            j     fib_epi
            nop
                  
fib_elif:         
            bgt      $a0, 1, fib_else
            li    $t3, 1
            add   $s7, $s7, $t3
            j     fib_epi
            nop
                  
fib_else:         # recursion goes here
            lw       $t2, ($fp)
            sub   $a0, $t2, 1
            jal   fib
            nop
            
            lw       $t2, ($fp)
            sub   $a0, $t2, 2
            jal   fib 
            nop

fib_epi:       # epilogue
            move  $v0, $s7

            add   $sp, $sp, 4
            lw    $fp, ($sp)
            add   $sp, $sp, 4
            lw    $ra, ($sp)
            add   $sp, $sp, 4
            jr    $ra