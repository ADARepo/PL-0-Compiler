#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

// Author: Austin Abshier

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100
#define MAX_SYMBOL_TABLE_SIZE 500

// global variables to use throughout this file.
int DEBUGGING = 0;
instruction *code;
int cIndex;
symbol *table;
int tIndex;
int currLevel;
lexeme *currToken;
char nameStorage[12];
int tokenPtr;

int errorFlag = 0;
//------------------------------------------------

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();
void program();
void block();
void constDeclaration();
int varDeclaration();
void procedureDeclaration();
void statement();
void condition();
void expression();
void term();
void factor();
void mark();
int findSymbol(lexeme *tok, int num);
int multipleDeclarationCheck(lexeme tok);

instruction *parse(lexeme *list, int printTable, int printCode)
{
    // space allocation.
	code = malloc(MAX_CODE_LENGTH * sizeof(instruction));
	table = malloc(MAX_SYMBOL_TABLE_SIZE * sizeof(symbol));

    // currToken will be used to parse through the list.
    // other initialization of global variables.
	currToken = list;
	cIndex = 0;
	tIndex = 0;
	currLevel = 0;
	tokenPtr = 0;

	// start of program
    program();

    if (errorFlag == 1)
        return NULL;

    // printing table and assembly code if told to.
    if (printTable == 1)
        printsymboltable();

    if (printCode == 1)
        printassemblycode();

    // per pseudo to give "halt" instruction in vm.
    code[cIndex].opcode = -1;

	return code;
}

void program()
{
    if (DEBUGGING)
        printf("Entering program.\n");

    // emit jmp
    emit(7, 0, 0);

    addToSymbolTable(3, "main", 0, currLevel, 0, 0);

    currLevel = -1;

    block();

    // error check
    if (errorFlag == 1)
        return;

    if (currToken[tokenPtr].type != periodsym)
    {
        printparseerror(1);
        exit(0);
    }

    // halt
    emit(9, 0, 3);

    for (int i = 0; i < cIndex; i++)
    {
        if (code[i].opcode == 5)
            code[i].m = table[code[i].m].addr;
    }

    code[0].m = table[0].addr;

}

void block()
{
    if (DEBUGGING)
        printf("Entering block.\n");

    currLevel++;

    int procedureIdx = tIndex - 1;

    constDeclaration();

    if (errorFlag == 1)
        return;

    int x = varDeclaration();

    if (errorFlag == 1)
        return;

    procedureDeclaration();

    if (errorFlag == 1)
        return;

    //printf("%d %d\n", procedureIdx, cIndex);
    table[procedureIdx].addr = cIndex * 3;

    if (currLevel == 0)
        emit(6, 0, x);      // INC
    else
        emit(6, 0, x + 4);      // INC

    statement();

    if (errorFlag == 1)
        return;

    //if (DEBUGGING) printf("-------tIndex: %d\n", tIndex);
    mark();

    if (errorFlag == 1)
        return;
    if (DEBUGGING) printf("-------currLevel: %d\n", currLevel);
    currLevel--;
}

void constDeclaration()
{
    if (DEBUGGING)
        printf("Entering constDecl.\n");

    if (currToken[tokenPtr].type == constsym)
    {
        do
        {
            if (DEBUGGING)
                printf("Entering constDecl->constsym\n");
            tokenPtr++;

            if (currToken[tokenPtr].type != identsym)
            {
                printparseerror(2);
                exit(0);
            }

            int symIdx = multipleDeclarationCheck(currToken[tokenPtr]);

            if (symIdx != -1)
            {
                printparseerror(13);
                exit(0);
            }

            strcpy(nameStorage, currToken[tokenPtr].name);

            tokenPtr++;

            nameStorage[11] = '\0';

            if (currToken[tokenPtr].type != assignsym)
            {
                printparseerror(2);
                exit(0);
            }

            tokenPtr++;

            if (currToken[tokenPtr].type != numbersym)
            {
                printparseerror(2);
                exit(0);
            }

            addToSymbolTable(1, nameStorage, currToken[tokenPtr].value, currLevel, 0, 0);

            tokenPtr++;

        } while (currToken[tokenPtr].type == commasym);

        if (currToken[tokenPtr].type != semicolonsym)
        {
            if (currToken[tokenPtr].type == identsym)
            {
                printparseerror(13);
                exit(0);
            }
            else
            {
                printparseerror(14);
                exit(0);
            }
        }

        tokenPtr++;
    }
}

int varDeclaration()
{
    if (DEBUGGING)
        printf("Entering varDecl.\n");

    int numVars = 0;

    if (currToken[tokenPtr].type == varsym)
    {
        do
        {
            if (DEBUGGING)
                printf("Entering varDecl->varSym\n");

            // incrementing for variable found and incrementing token ptr.
            numVars++;
            tokenPtr++;

            if (currToken[tokenPtr].type != identsym)
            {
                printparseerror(3);
                exit(0);
            }

            int symIdx = multipleDeclarationCheck(currToken[tokenPtr]);

            if (symIdx != - 1)
            {
                printparseerror(18);
                exit(0);
            }

            if (currLevel == 0)
                addToSymbolTable(2, currToken[tokenPtr].name, 0, currLevel, numVars - 1, 0);
            else
                addToSymbolTable(2, currToken[tokenPtr].name, 0, currLevel, numVars + 3, 0);

            tokenPtr++;

        } while (currToken[tokenPtr].type == commasym);

        if (currToken[tokenPtr].type != semicolonsym)
        {

            if (currToken[tokenPtr].type == identsym)
            {
                printparseerror(13);
                exit(0);
            }

            else
            {
                printparseerror(14);
                exit(0);
            }
        }
        tokenPtr++;
    }

    return numVars;
}

void procedureDeclaration()
{
    if (DEBUGGING)
        printf("Entering procedureDecl.\n");


    while (currToken[tokenPtr].type == procsym)
    {

        if (DEBUGGING)
            printf("Entering procDecl->procsym\n");

        tokenPtr++;

        if (currToken[tokenPtr].type != identsym)
        {
            printparseerror(14);
            exit(0);
        }

        int symIdx = multipleDeclarationCheck(currToken[tokenPtr]);

        if (symIdx != -1)
        {
            printparseerror(4);
            exit(0);
        }

        addToSymbolTable(3, currToken[tokenPtr].name, 0, currLevel, 0, 0);

        tokenPtr++;

        if (currToken[tokenPtr].type != semicolonsym)
        {
            printparseerror(4);
            exit(0);
        }

        tokenPtr++;

        block();

        if (errorFlag == 1)
            return;

        if (currToken[tokenPtr].type != semicolonsym)
        {
            printparseerror(4);
            exit(0);
        }

        tokenPtr++;

        // emit rtn.
        emit(2, 0, 0);
    }
}

void statement()
{
    if (DEBUGGING)
        printf("Entering statement.\n");

    if (currToken[tokenPtr].type == identsym)
    {
        if (DEBUGGING)
            printf("Entering identsym.\n");

        int symIdx = findSymbol(currToken, 2);

        if (symIdx == -1)
        {
//            if (DEBUGGING)
//                printsymboltable();
            if (findSymbol(currToken, 1) != findSymbol(currToken, 3))
            {
                printparseerror(6);
                exit(0);
            }

            else
            {
                printparseerror(19);
                exit(0);
            }
        }

        tokenPtr++;

        if (currToken[tokenPtr].type != assignsym)
        {
            printparseerror(5);
            exit(0);
        }

        tokenPtr++;

        expression();

        if (errorFlag == 1)
            exit(0);

        // STO
        emit(4, currLevel - table[symIdx].level, table[symIdx].addr);

        return;
    }

    if (currToken[tokenPtr].type == dosym)
    {
        if (DEBUGGING)
            printf("Entering dosym.\n");

        do
        {
            tokenPtr++;
            statement();
            if (errorFlag == 1)
                return;
        } while (currToken[tokenPtr].type == semicolonsym);

        if (currToken[tokenPtr].type != odsym)
        {
            if (currToken[tokenPtr].type == identsym || currToken[tokenPtr].type == dosym ||
                currToken[tokenPtr].type == whensym || currToken[tokenPtr].type == whilesym ||
                currToken[tokenPtr].type == readsym || currToken[tokenPtr].type == writesym ||
                currToken[tokenPtr].type == callsym)
            {
                printparseerror(15);
                exit(0);
            }

            else
            {
                printparseerror(16);
                exit(0);
            }
        }

        tokenPtr++;
        return;
    }

    if (currToken[tokenPtr].type == whensym)
    {
        if (DEBUGGING)
            printf("Entering whensym.\n");

        tokenPtr++;

        condition();

        if (errorFlag == 1)
            return;

        int jpcIdx = cIndex;

        // emit jpc
        emit(8, 0, 0);

        if (currToken[tokenPtr].type != dosym)
        {
            printparseerror(8);
            exit(0);
        }

        tokenPtr++;

        statement();

        if (errorFlag == 1)
            return;

        if (currToken[tokenPtr].type == elsedosym)
        {

            if (DEBUGGING)
                printf("Entering elsedosym.\n");

            int jmpIdx = cIndex;

            emit(7, 0, 0);

            code[jpcIdx].m = cIndex * 3;

            tokenPtr++;

            statement();

            if (errorFlag == 1)
                return;

            code[jmpIdx].m = cIndex * 3;
        }

        else
        {
            code[jpcIdx].m = cIndex * 3;
        }

        return;
    }

    if (currToken[tokenPtr].type == whilesym)
    {
        if (DEBUGGING)
            printf("Entering whilesym.\n");

        tokenPtr++;
        int loopIdx = cIndex;

        condition();

        if (errorFlag == 1)
            return;

        if (currToken[tokenPtr].type != dosym)
        {
            printparseerror(9);
            exit(0);
        }

        tokenPtr++;

        int jpcIdx = cIndex;

        // emit jpc.
        emit(8, 0, 0);

        statement();

        if (errorFlag == 1)
            return;

        emit(7, 0, loopIdx * 3);

        code[jpcIdx].m = cIndex * 3;

        return;
    }

    if (currToken[tokenPtr].type == readsym)
    {
        if (DEBUGGING)
            printf("Entering readsym.\n");

        tokenPtr++;

        if (currToken[tokenPtr].type != identsym)
        {
            printparseerror(6);
            exit(0);
        }

        int symIdx = findSymbol(currToken, 2);

        if (symIdx == -1)
        {
            if (findSymbol(currToken, 1) != findSymbol(currToken, 3))
            {
                printparseerror(6);
                exit(0);
            }

            else
            {
                printparseerror(19);
                exit(0);
            }
        }

        tokenPtr++;

        // emit read and sto.
        emit(9, 0, 2);
        emit(4, currLevel - table[symIdx].level, table[symIdx].addr);

        return;
    }

    if (currToken[tokenPtr].type == writesym)
    {
        if (DEBUGGING)
            printf("Entering writesym.\n");

        tokenPtr++;

        expression();

        if (errorFlag == 1)
            return;

        // emit write.
        emit(9, 0, 1);

        return;
    }

    if (currToken[tokenPtr].type == callsym)
    {
        if (DEBUGGING)
            printf("Entering callsym.\n");

        tokenPtr++;

        int symIdx = findSymbol(currToken, 3);

        if (symIdx == -1)
        {
            if (findSymbol(currToken, 1) != findSymbol(currToken, 2))
            {
                printparseerror(7);
                exit(0);
            }

            else
            {
                printparseerror(19);
                exit(0);
            }
        }

        tokenPtr++;

        // emit cal.
        emit(5, currLevel - table[symIdx].level, symIdx);
    }
}

void condition()
{
    if (DEBUGGING)
        printf("Entering condition.\n");

    if (currToken[tokenPtr].type == oddsym)
    {
        tokenPtr++;
        expression();
        if (errorFlag == 1)
            return;

        // emit odd
        emit(2, 0, 6);
    }
    else
    {
        expression();

        if (errorFlag == 1)
            return;

        if (currToken[tokenPtr].type == eqlsym)
        {
            tokenPtr++;
            expression();
            if (errorFlag == 1)
                return;
            emit(2, 0, 8);
        }
        else if (currToken[tokenPtr].type == neqsym)
        {
            tokenPtr++;
            expression();
            if (errorFlag == 1)
                return;
            emit(2, 0, 9);
        }
        else if (currToken[tokenPtr].type == lsssym)
        {
            tokenPtr++;
            expression();
            if (errorFlag == 1)
                return;
            emit(2, 0, 10);
        }
        else if (currToken[tokenPtr].type == leqsym)
        {
            tokenPtr++;
            expression();
            if (errorFlag == 1)
                return;
            emit(2, 0, 11);
        }
        else if (currToken[tokenPtr].type == gtrsym)
        {
            tokenPtr++;
            expression();
            if (errorFlag == 1)
                return;
            emit(2, 0, 12);
        }
        else if (currToken[tokenPtr].type == geqsym)
        {
            tokenPtr++;
            expression();
            if (errorFlag == 1)
                return;
            emit(2, 0, 13);
        }
        else
        {
            printparseerror(10);
            exit(0);
        }
    }
}

void expression()
{
    if (DEBUGGING)
        printf("Entering expression.\n");

    if (currToken[tokenPtr].type == subsym)
    {
        tokenPtr++;
        term();
        if (errorFlag == 1)
                return;

        // emit neg.
        emit(2, 0, 1);

        while (currToken[tokenPtr].type == addsym || currToken[tokenPtr].type == subsym)
        {
            if (currToken[tokenPtr].type == addsym)
            {
                tokenPtr++;
                term();
                if (errorFlag == 1)
                    return;

                // emit add.
                emit(2, 0, 2);
            }
            else
            {
                tokenPtr++;
                term();
                if (errorFlag == 1)
                    return;

                // emit sub.
                emit(2, 0, 3);
            }
        }
    }

    else
    {
        if (currToken[tokenPtr].type == addsym)
            tokenPtr++;

        term();
        if (errorFlag == 1)
                return;

        while (currToken[tokenPtr].type == addsym || currToken[tokenPtr].type == subsym)
        {
            if (currToken[tokenPtr].type == addsym)
            {
                tokenPtr++;
                term();
                if (errorFlag == 1)
                    return;

                // emit add.
                emit(2, 0, 2);
            }
            else
            {
                tokenPtr++;
                term();
                if (errorFlag == 1)
                    return;

                // emit sub.
                emit(2, 0, 3);
            }
        }
    }

     if (currToken[tokenPtr].type == lparensym || currToken[tokenPtr].type == identsym ||
         currToken[tokenPtr].type == numbersym || currToken[tokenPtr].type == oddsym)
     {
         printparseerror(17);
         exit(0);
     }
}

void term()
{
    if (DEBUGGING)
        printf("Entering term.\n");

    factor();
    if (errorFlag == 1)
                return;

    while (currToken[tokenPtr].type == multsym || currToken[tokenPtr].type == divsym || currToken[tokenPtr].type == modsym)
    {
        if (currToken[tokenPtr].type == multsym)
        {
            tokenPtr++;
            factor();
            if (errorFlag == 1)
                return;

            // emit mult.
            emit(2, 0, 4);
        }

        else if (currToken[tokenPtr].type == divsym)
        {
            tokenPtr++;
            factor();
            if (errorFlag == 1)
                return;

            // emit div
            emit(2, 0, 5);
        }
        else
        {
            tokenPtr++;
            factor();
            if (errorFlag == 1)
                return;

            // emit mod
            emit(2, 0, 7);
        }
    }
}

void factor()
{
    if (DEBUGGING)
        printf("Entering factor.\n");

    if (currToken[tokenPtr].type == identsym)
    {
        int symidxVar = findSymbol(currToken, 2);
        int symidxConst = findSymbol(currToken, 1);

        if (symidxVar == -1 && symidxConst == -1)
        {
            if (findSymbol(currToken, 3) != -1)
            {
                printparseerror(11);
                exit(0);
            }
            else
            {
                printparseerror(19);
                exit(0);
            }
        }

        if (symidxVar == -1)// (const)
            emit(1, 0, table[symidxConst].val); // LIT

        else if (symidxConst == -1 || table[symidxVar].level > table[symidxConst].level)
            emit(3, currLevel - table[symidxVar].level, table[symidxVar].addr);   // LOD

        else
            emit(1, 0, table[symidxConst].val); // LIT

        tokenPtr++;
    }

    else if (currToken[tokenPtr].type == numbersym)
    {
        emit(1, 0, currToken[tokenPtr].value);  // LIT
        tokenPtr++;
    }

    else if (currToken[tokenPtr].type == lparensym)
    {
        tokenPtr++;
        expression();
        if (errorFlag == 1)
            return;

        if (currToken[tokenPtr].type != rparensym)
        {
            printparseerror(12);
            exit(0);
        }

        tokenPtr++;
    }

    else
    {
        printparseerror(11);
        exit(0);
    }
}

void mark()
{
    if (DEBUGGING)
        printf("Entering mark.\n");

    for (int i = tIndex - 1; i >= 0; i--)
    {
        if (table[i].mark == 0)
        {
            if ((table[i].level < currLevel))
                return;

            if (table[i].level == currLevel)
                table[i].mark = 1;
        }

    }
}

int findSymbol(lexeme *tok, int num)
{
    if (DEBUGGING)
        printf("Entering findSymbol.\n");

    for (int i = tIndex; i >= 0; i--)
    {
        if (strcmp(tok[tokenPtr].name, table[i].name) == 0 && table[i].kind == num && table[i].mark == 0)
            return i;
    }

    return -1;
}

int multipleDeclarationCheck(lexeme tok)
{
    if (DEBUGGING)
        printf("Entering multDeclCheck.\n");

    for (int i = 0; i < tIndex; i++)
    {
        if (strcmp(tok.name, table[i].name) == 0)
            if (table[i].mark == 0)
                if (table[i].level == currLevel)
                    return i;
    }

    return -1;
}

void emit(int opname, int level, int mvalue)
{
    if (DEBUGGING)
        printf("Entering emit.\n");

	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
    if (DEBUGGING)
        printf("Entering addSymbTable.\n");

	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}


void printparseerror(int err_code)
{
    if (DEBUGGING)
        printf("Entering printparseerror.\n\n");
    if (DEBUGGING)
        printsymboltable();

	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
            printf("Parser Error: when must be followed by do\n");
            break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
            printf("Parser Error: Statements within do-od must be separated by a semicolon\n");
            break;
		case 16:
            printf("Parser Error: do must be followed by od\n");
            break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}

	free(code);
	free(table);
    //exit(0);
	errorFlag = 1;
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}
