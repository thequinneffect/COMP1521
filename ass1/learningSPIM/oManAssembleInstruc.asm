## Program to assemble the instruction ori  $8,$9,0x004A
##
        .text
        .globl  main

main:
        or    $25,$0,$0        # clear $25
        ori   $11,$0,0xD       # opcode
        ori   $12,$0,0x9       # operand $s
        ori   $13,$0,0x8       # dest. $d
        ori   $14,$0,0x004A    # immediate operand
        
        sll   $11,$11,26       # shift opcode into position
        or    $25,$25,$11      # or it into the instruction
        
        sll   $12,$12,21       # shift operand $s into position
        or    $25,$25,$12      # or it into the instruction
        
        sll   $13,$13,16       # shift dest $d into position
        or    $25,$25,$13      # or it into the instruction
        
        or    $25,$25,$14      # or const into the instruction
        
        ori   $8,$9,0x004A     # The actual assembler
                               # should create the same machine
                               # instruction as we now have in $25

## end of file