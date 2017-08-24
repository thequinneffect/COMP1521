# this is an if else control flow - this is not a program
                ...                #   
if:     beq  $8,$9,yes     # is a==b ?
        nop                #    no
        ...                #    false block
        ...                #    
        ...                #  
        j    endif
        nop
yes:    ...                #   yes
        ...                #   true block
        ...                #
endif:  nop                # always executed