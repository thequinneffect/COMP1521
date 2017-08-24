## The program adds +146 to -82, leaving the result in $10
# note: this is the much better way talked about in addv3

        .text
        .globl  main

main:
        ori      $7, $0,146        # put +146 into $7
   	 	addiu    $10,$7,-82         # add -82

## End of file

