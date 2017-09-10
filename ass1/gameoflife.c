/*
	Game of Life 

	MIPS & C Board Generator for COMP1521 UNSW

	23/08/2017
	
	Keiran Sampson

*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAXBOARD 1000


int main()
{
	int size;

	srand((unsigned) time(NULL));

	printf("\nWELCOME TO THE GAME OF LIFE\n");

	printf("\nEnter a Board Size: ");

	scanf("%d",&size);

	if(size > MAXBOARD-2)
	{
		printf("Invalid Entry\n");
		return 0;
	}

	FILE *fp = fopen("tmp","w");

	if(fp==NULL)
	{
		printf("Error Creating Temporary File\n");
		return 0;
	}

	for(int i=0;i<size;i++)
	{
		for(int j=0;j<size-1;j++)
		{
			fprintf(fp,"%d, ",rand() % 2);
		}
		fprintf(fp,"%d\n",rand() % 2);
	}

	fclose(fp);
	
	fp = fopen("tmp","r");

	if(fp==NULL)
	{
		printf("Error Opening Temporary File\n");
		return 0;
	}

	FILE *wfp = fopen("board.s","w");
	if(wfp==NULL)
	{
		printf("Error creating board.s\n");
		return 0;
	}

	FILE *wfp2 = fopen("board.h","w");
	if(wfp2==NULL)
	{
		printf("Error creating board.h\n");
		return 0;
	}

	//create mips and c boards

	fprintf(wfp,"# board.s ... Game of Life on a %dx%d grid\n\n\t.data\n\nN:\t.word %d  # gives board dimensions\n\nboard:\n",size,size,size);

	fprintf(wfp2,"// Game of Life on a %dx%d grid\n\n#define NN %d\n\nint N = NN;\n\nchar board[NN][NN] = {\n",size,size,size);

	char input[MAXBOARD]; 

	while(fgets(input, MAXBOARD, fp) != NULL)
	{		
		int len = strlen(input);
		input[len-1] = '\0'; //remove newline at end of string
		fprintf(wfp,"\t.byte %s\n",input);
		fprintf(wfp2,"\t{%s},\n",input);
	}

	fprintf(wfp,"\nnewBoard: .space %d\n",size*size);
	fprintf(wfp2,"};\n\nchar newboard[NN][NN];\n\n");


	fclose(wfp);
	fclose(wfp2);

	system("rm -r tmp");

	printf("\n...Boards Successfully Compiled...\n");

	printf("\nDo you want to perform tests now? y/n: ");

	char test;
	scanf("%c",&test);
	scanf("%c",&test);

	if (test=='n' || test=='N')
	{
		if(test=='n' || test=='N')
		{
			printf("\nDo you want to play the game of life? y/n: ");

			char test;
			scanf("%c",&test);
			scanf("%c",&test);
			if (test=='y' || test=='Y')
			{
				system("cat board.s prog.s > test.s");
				system("spim -file test.s");
				return 0;
			}
		}

		return 0;
	}

	printf("\n...Preparing Files...\n");

	system("cat board.s prog.s > test.s");
	system("dcc life.c");
	system("echo 10 | ./a.out > c.out");
	system("echo 10 | spim -file test.s | tail -n +6 > mips.out");

	printf("\n...Performing Tests...\n");

	printf("\n...Differences...\n\n");

	system("diff c.out mips.out");



	return 0;

}