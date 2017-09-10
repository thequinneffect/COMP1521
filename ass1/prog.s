# prog.s ... Game of Life on a NxN grid
#
# Needs to be combined with board.s
# The value of N and the board data
# structures come from board.s
#
# Written by Nicholas Quinn (z5117408), August 2017

                    .data

                    .align  2
main_ret_save:      .space  4   
input_prompt:       .asciiz "# Iterations: "
iteration_msg1:     .asciiz "=== After iteration "
iteration_msg2:     .asciiz " ===\n"
alive:              .asciiz "#"
dead:               .asciiz "."
newline:            .asciiz "\n"

#=============================================================================================================#

                    .text
                    .globl main

main:
                    sw      $ra, main_ret_save          # save main return address
                        
                    la      $a0, input_prompt           # print user prompt asking for number of iterations
                    li      $v0, 4
                    syscall

                    li      $v0, 5                      # read in integer
                    syscall

                    move    $s3, $v0                    # move number of iterations
                    lw      $s4, N                      # load board dimension
                    li      $s0, 1                      # initialise loop1 counter
                    la      $s6, board                  # $s6 is base address for board
                    la      $s7, newBoard               # $s7 is base address for newBoard

    m_loop1:        bgt     $s0, $s3, end_main          # branch when counter is > num iterations
                    li      $s1, 0                      # initialise middle counter
    
    m_loop2:        bge     $s1, $s4, incr_m_loop1      # branch when counter is >= board dimensions
                    li      $s2, 0                      # initialise inner counter
    
    m_loop3:        bge     $s2, $s4, incr_m_loop2      # branch when counter is >= board dimensions
                    
                    move    $a0, $s1                    # load arguments for neighbours function
                    move    $a1, $s2
                    jal     neighbours                  # call neighbours function
                    move    $s5, $v0                    # move return value of neighbours into $s5
                    mul     $t0, $s1, $s4               # calculate decimal offset (i * N)
                    add     $t0, $t0, $s2               # calculate decimal offset (+ j), $t0 is now the current offset
                    add     $t3, $s6, $t0               # add offset to base register of board
                    add     $t4, $s7, $t0               # add offset to base resister of newBoard

    m_if:           lb      $t2, ($t3)                  # load the byte (char) from the offset board location
                    bne     $t2, 1, m_elif              # branch depending whether the char is a 1 or 0
    
    m_if2:          bge     $s5, 2, m_elif2             # branch if ret val of neighbours is >= 2
                    sb      $zero, ($t4)                # newboard[i][j] = 0;
                    j       incr_m_loop3

    m_elif2:        beq     $s5, 2, continue            # branch if ret val of neighbours is == 2
                    bne     $s5, 3, m_else2             # branch if ret val of neighbours is != 2
    
    continue:       li      $t1, 1                      #newboard[i][j] = 1;
                    sb      $t1, ($t4)
                    j       incr_m_loop3

    m_else2:        sb      $zero, ($t4)                # newboard[i][j] = 0;
                    j       incr_m_loop3

    m_elif:         bne     $s5, 3, m_else              # branch if ret val of neighbouts is != 3
                    li      $t1, 1                      # newboard[i][j] = 1;
                    sb      $t1, ($t4)
                    j       incr_m_loop3

    m_else:         sb      $zero, ($t4)                # newboard[i][j] = 0;
                    j       incr_m_loop3                    

    incr_m_loop3:   addi    $s2, $s2, 1                 # increment loop3
                    j       m_loop3
    incr_m_loop2:   addi    $s1, $s1, 1                 # increment loop2
                    j       m_loop2
    incr_m_loop1:   la      $a0, iteration_msg1         # print iteration messages "after iteration x"
                    li      $v0, 4
                    syscall
                    move    $a0, $s0
                    li      $v0, 1
                    syscall
                    la      $a0, iteration_msg2
                    li      $v0, 4
                    syscall
                    jal     copy_show                   # call copy show function
                    addi    $s0, $s0, 1                 # increment loop1
                    j       m_loop1

    end_main:
                    lw      $ra, main_ret_save          # load return address of main into return address reg.
                    jr      $ra                         # jump to main caller

                    # C code for this function #

                    #int maxiters;
                    #printf("# Iterations: ");
                    #scanf("%d", &maxiters);
                    #for (int n = 1; n <= maxiters; n++) {
                    #  for (int i = 0; i < N; i++) {
                    #     for (int j = 0; j < N; j++) {
                    #        int nn = neighbours(i,j);
                    #        if (board[i][j] == 1) {
                    #           if (nn < 2)
                    #                            newboard[i][j] = 0;
                    #           else if (nn ==2 || nn == 3)
                    #              newboard[i][j] = 1;
                    #           else
                    #              newboard[i][j] = 0;
                    #        }
                    #        else if (nn == 3)
                    #           newboard[i][j] = 1;
                    #        else
                    #           newboard[i][j] = 0;
                    #     }
                    #  }
                    #  printf("=== After iteration %d ===\n", n);
                    #  copyBackAndShow();
                    #}              
                    #return 0;

#=============================================================================================================#

neighbours:
                    #       $a0 = i
                    #       $a1 = j
                    move    $v0, $zero                  # initialise return value to 0
                    li      $t0, -1                     # init loop1 counter
                    sub     $t4, $s4, 1                 # store N-1 value for comparison (using as global as it's #defined)
                    la      $t5, board                  # $t5 is base register for board

    n_loop1:        bgt     $t0, 1, end_n               # branch if counter is > 1
                    li      $t1, -1                     # init loop2 counter

    n_loop2:        bgt     $t1, 1, incr_n_loop1        # branch if counter is > 1

                    add     $t2, $a0, $t0               # $t2 = i + x
                    bltz    $t2, incr_n_loop2           
                    bgt     $t2, $t4, incr_n_loop2
                    add     $t3, $a1, $t1               # $t3 = j + y
                    bltz    $t3, incr_n_loop2
                    bgt     $t3, $t4, incr_n_loop2
                    bne     $t0, $zero, n_if            # branch if l1 counter != 0
                    bne     $t1, $zero, n_if            # branch if l2 counter != 0
                    j       incr_n_loop2

    n_if:           mul     $t6, $t2, $s4               # calculate row position
                    add     $t6, $t6, $t3               # calculate column position
                    add     $t6, $t5, $t6               # combine offsets with base register
                    lb      $t7, ($t6)                  # load the byte at base+offset
                    bne     $t7, 1, incr_n_loop2        # branch if char at board+offset != 1
                    addi    $v0, $v0, 1                 # increment ret val


    incr_n_loop2:   addi    $t1, $t1, 1                 # increment loop2
                    j       n_loop2

    incr_n_loop1:   addi    $t0, $t0, 1                 # increment loop1
                    j       n_loop1

    end_n:          jr      $ra

                    # C code for this function #

                    #int nn = 0;
                    #for (int x = -1; x <= 1; x++) {
                    #   for (int y = -1; y <= 1; y++) {
                    #       if (i+x < 0 || i+x > N-1) continue;
                    #       if (j+y < 0 || j+y > N-1) continue;
                    #       if (x == 0 && y == 0) continue;
                    #       if (board[i+x][j+y] == 1) nn++;
                    #   }
                    #}
                    #return nn;

#=============================================================================================================#

copy_show:
                    li      $t0, 0                      # $t0 = i, init l1 counter
                    la      $t2, newBoard               # $t2 is base address for newBoard
                    la      $t3, board                  # $t3 is base address for board

    c_loop1:        bge     $t0, $s4, end_c             # branch if counter is >= N
                    li      $t1, 0                      # $t1 = j, init l2 counter

    c_loop2:        bge     $t1, $s4, incr_c_loop1      # branch if counter is greater 

                    mul     $t4, $t0, $s4               # calculate row offset
                    add     $t4, $t4, $t1               # calculate column offset
                    add     $t6, $t2, $t4               # t6 now has newBoard + offset
                    add     $t7, $t3, $t4               # t7 now has board + offset
                    lb      $t5, ($t6)                  # load the char at new board base+offset
                    sb      $t5, ($t7)                  # store the char at board base+offset
                    lb      $t5, ($t7)                  # load the char from the new board at base+offset

    c_if:           bne     $t5, $zero, c_else          # branch if the char loaded from board is == 0
                    la      $a0, dead                   # print dead character
                    li      $v0, 4
                    syscall
                    j       incr_c_loop2    

    c_else:         la      $a0, alive                  # print alive character
                    li      $v0, 4
                    syscall

    incr_c_loop2:   addi    $t1, $t1, 1                 # increment l2 counter
                    j       c_loop2
    
    incr_c_loop1:   la      $a0, newline                # print a newline
                    li      $v0, 4  
                    syscall
                    addi    $t0, $t0, 1                 # increment l1 counter
                    j       c_loop1

    end_c:          jr      $ra                         # return

                    # C code for this function #

                    #for (int i = 0; i < N; i++) {
                    #   for (int j = 0; j < N; j++) {
                    #       board[i][j] = newboard[i][j];
                    #       if (board[i][j] == 0)
                    #           putchar('.');
                    #       else
                    #           putchar('#');
                    #   }
                    #   putchar('\n');
                    #}
                    
#=============================================================================================================#