# basics of the data section of mips

        	.data		# create and initialise variables with symbols to point to them
num:		.word		42
s_num:		.halfword	24
char:		.byte		'c'
														
        	.text   																		
main:               	
			jr $ra		

# end of program	