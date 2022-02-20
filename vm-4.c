// Author: Austin Abshier

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

#define MAX_PAS_LENGTH 3000
#define INSTRUCTIONS 3

// global variables.
int bp, pc, sp, dp, gp, heap;
static int pas[MAX_PAS_LENGTH];
static int ir[INSTRUCTIONS];

int base (int L)
{
	int arb = bp;
	while (L > 0)
	{
		arb = pas[arb - 1];
		L--;
	}
	return arb;
}

void print_execution (int line, char *opname, int *ir, int pc, int bp, int sp, int dp, int *pas, int gp)
{
	int i;
	printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, ir[1], ir[2], pc, bp, sp, dp);

	for (i = gp; i <= dp; i++)
		printf("%d ", pas[i]);
	printf("\n");

	printf("\tstack: ");
	for(i = MAX_PAS_LENGTH - 1; i >= sp; i--)
		printf("%d ", pas[i]);
	printf("\n");
}

void execute_program(instruction *code, int printFlag)
{

	// counting lines and initializing values into pas.
	int i = 0, tempOP, tempL, tempM, ic;
	int pasIter = 0;

	while(code[i].opcode != -1)
	{
		pas[pasIter++] = code[i].opcode;
		pas[pasIter++] = code[i].l;
		pas[pasIter++] = code[i].m;
		i++;
	}
/*
	pas[pasIter++] = 9;
	pas[pasIter++] = 0;
	pas[pasIter++] = 3; */

	//pas[pasIter++] = -1;

	// setting up registers.
	ic = pasIter;
	gp = ic;
	dp = ic - 1;
	heap = ic + 40;
	bp = ic;
	pc = 0;
	sp = MAX_PAS_LENGTH;

	// outputting required headers and initial values.
	printf("\t\t\t\tPC\tBP\tSP\tDP\tdata\n");
	printf("Initial values:\t\t\t%d\t%d\t%d\t%d\n", pc, bp, sp, dp);

	// for getting the opcode and tracking the pc in loop
	char opcode[4];

	// going through the data in text segment.
	int j, lineNumber;

	int halt = 0;

	while(!halt)
	{
		// assigning values for..
		lineNumber = pc;
		ir[0] = pas[pc++]; // op
		ir[1] = pas[pc++]; // l
		ir[2] = pas[pc++]; // m

		//printf("%d %d %d\n", ir[0], ir[1], ir[2]);

		if (ir[0] == 1)
		{
			strcpy(opcode, "LIT");
			if (bp == gp)
			{
				dp = dp + 1;
				pas[dp] = ir[2];
			}

			else
			{
				sp = sp - 1;
				pas[sp] = ir[2];
			}
		}

		// OPR
		if (ir[0] == 2)
		{
			// RTN
			if (ir[2] == 0)
			{
				strcpy(opcode, "RTN");
				sp = bp + 1;
				pc = pas[sp - 4];
				bp = pas[sp - 3];
			}

			// NEG.
			if (ir[2] == 1)
			{
				strcpy(opcode, "NEG");
				if (bp == gp)
					pas[dp] = (-1 * pas[dp]);
				else
					pas[sp] = (-1 * pas[sp]);
			}

			// ADD.
			if (ir[2] == 2)
			{
				strcpy(opcode, "ADD");
				if (bp == gp)
				{
					dp = dp - 1;
					pas[dp] = pas[dp] + pas[dp + 1];
				}

				else
				{
					sp = sp + 1;
					pas[sp] = pas[sp] + pas[sp - 1];
				}
			}

			// SUB
			if (ir[2] == 3)
			{
				strcpy(opcode, "SUB");
				if (bp == gp)
				{
					dp = dp - 1;
					pas[dp] = pas[dp] - pas[dp + 1];
				}

				else
				{
					sp = sp + 1;
					pas[sp] = pas[sp] - pas[sp - 1];
				}
			}

			// MUL
			if (ir[2] == 4)
			{
				strcpy(opcode, "MUL");
				if (bp == gp)
				{
					dp = dp - 1;
					pas[dp] = pas[dp] * pas[dp + 1];
				}

				else
				{
					sp = sp + 1;
					pas[sp] = pas[sp] * pas[sp - 1];
				}
			}

			// DIV
			if (ir[2] == 5)
			{
				strcpy(opcode, "DIV");
				if (bp == gp)
				{
					dp = dp - 1;
					pas[dp] = pas[dp] / pas[dp + 1];
				}

				else
				{
					sp = sp + 1;
					pas[sp] = pas[sp] / pas[sp - 1];
				}
			}

			// ODD
			if (ir[2] == 6)
			{
				strcpy(opcode, "ODD");
				if (bp == gp)
					pas[dp] = pas[dp] % 2;
				else
					pas[sp] = pas[sp] % 2;
			}

			// MOD
			if (ir[2] == 7)
			{
				strcpy(opcode, "MOD");
				if (bp == gp)
				{
					dp = dp - 1;
					pas[dp] = pas[dp] % pas[dp + 1];
				}

				else
				{
					sp = sp + 1;
					pas[sp] = pas[sp] % pas[sp - 1];
				}
			}

			// EQL
			if (ir[2] == 8)
			{
				strcpy(opcode, "EQL");
				if (bp == gp)
				{
					dp = dp - 1;
					pas[dp] = pas[dp] == pas[dp + 1];
				}

				else
				{
					sp = sp + 1;
					pas[sp] = pas[sp] == pas[sp - 1];
				}
			}

			// NEQ
			if (ir[2] == 9)
			{
				strcpy(opcode, "NEQ");
				if (bp == gp)
				{
					dp = dp - 1;
					pas[dp] = pas[dp] != pas[dp + 1];
				}

				else
				{
					sp = sp + 1;
					pas[sp] = pas[sp] != pas[sp - 1];
				}
			}

			//LSS
			if (ir[2] == 10)
			{
				strcpy(opcode, "LSS");
				if (bp == gp)
				{
					dp = dp - 1;
					pas[dp] = pas[dp] < pas[dp + 1];
				}

				else
				{
					sp = sp + 1;
					pas[sp] = pas[sp] < pas[sp - 1];
				}
			}

			//LEQ
			if (ir[2] == 11)
			{
				strcpy(opcode, "LEQ");
				if (bp == gp)
				{
					dp = dp - 1;
					pas[dp] = pas[dp] <= pas[dp + 1];
				}

				else
				{
					sp = sp + 1;
					pas[sp] = pas[sp] <= pas[sp - 1];
				}
			}

			//GTR
			if (ir[2] == 12)
			{
				strcpy(opcode, "GTR");
				if (bp == gp)
				{
					dp = dp - 1;
					pas[dp] = pas[dp] > pas[dp + 1];
				}

				else
				{
					sp = sp + 1;
					pas[sp] = pas[sp] > pas[sp - 1];
				}
			}

			//GEQ
			if (ir[2] == 13)
			{
				strcpy(opcode, "GEQ");
				if (bp == gp)
				{
					dp = dp - 1;
					pas[dp] = pas[dp] >= pas[dp + 1];
				}

				else
				{
					sp = sp + 1;
					pas[sp] = pas[sp] >= pas[sp - 1];
				}
			}
		}

		// LOD
		if (ir[0] == 3)
		{
			strcpy(opcode, "LOD");
			if (bp == gp)
			{
				dp = dp + 1;
				pas[dp] = pas[gp + ir[2]];
			}
			else
			{
				if (base(ir[1]) == gp)
				{
					sp = sp - 1;
					pas[sp] = pas[gp + ir[2]];
				}
				else
				{
					sp = sp - 1;
					pas[sp] = pas[base(ir[1]) - ir[2]];
				}
			}
		}

		// STO
		if (ir[0] == 4)
		{
			strcpy(opcode, "STO");
			if (bp == gp)
			{
				pas[gp + ir[2]] = pas[dp];
				dp = dp - 1;
			}

			else
			{
				if (base(ir[1]) == gp)
				{
					pas[gp + ir[2]] = pas[sp];
					sp = sp + 1;
				}

				else
				{
					pas[base(ir[1]) - ir[2]] = pas[sp];
					sp = sp + 1;
				}
			}
		}

		// CAL
		if (ir[0] == 5)
		{
			strcpy(opcode, "CAL");
			pas[sp - 1] = 0;
			pas[sp - 2] = base(ir[1]);
			pas[sp - 3] = bp;
			pas[sp - 4] = pc;
			bp = sp - 1;
			pc = ir[2];
		}

		// INC
		if (ir[0] == 6)
		{
			strcpy(opcode, "INC");
			if (bp == gp)
				dp = dp + ir[2];
			else
				sp = sp - ir[2];
		}

		// JMP
		if (ir[0] == 7)
		{
			strcpy(opcode, "JMP");
			pc = ir[2];
		}

		// JPC
		if (ir[0] == 8)
		{
			strcpy(opcode, "JPC");
			if (bp == gp)
			{
				if (pas[dp] == 0)
					pc = ir[2];
				dp = dp - 1;
			}

			else
			{
				if (pas[sp] == 0)
					pc = ir[2];
				sp = sp + 1;
			}
		}

		// SYS
		if (ir[0] == 9)
		{
			strcpy(opcode, "SYS");
			if (ir[2] == 1)
			{
				if (bp == gp)
				{
					printf("Top of stack value: %d\n", pas[dp]);
					dp = dp - 1;
				}

				else
				{
					printf("Top of stack value: %d\n", pas[sp]);
					sp = sp + 1;
				}

			}

			if (ir[2] == 2)
			{
				if (bp == gp)
				{
					dp = dp + 1;
					printf("Please Enter an Integer: ");
					scanf("%d", &pas[dp]);
					//printf("\n");
				}

				else
				{
					sp = sp - 1;
					printf("Please Enter an Integer: ");
					scanf("%d", &pas[sp]);
					//printf("\n");
				}
			}
			if (ir[2] == 3)
			{
				halt = 1;
			}
		}

		if (ir[0] == -1)                   // this is the line for checking if opcode is -1 to stop.
            halt = 1;

        if (printFlag == 1)
            print_execution(lineNumber / 3, opcode, ir, pc, bp, sp, dp, pas, gp);
	}
    return;
}
