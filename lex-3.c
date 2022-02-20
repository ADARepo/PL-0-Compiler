/*
	This is the lex.c file for the UCF Fall 2021 Systems Software Project.
	For HW2, you must implement the function lexeme *lexanalyzer(char *input).
	You may add as many constants, global variables, and support functions
	as you desire.

	If you choose to alter the printing functions or delete list or lex_index,
	you MUST make a note of that in you readme file, otherwise you will lose
	5 points.
*/

// Author: Austin Abshier


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

#define MAX_NUMBER_TOKENS 1000
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme *list;
int lex_index;

void printlexerror(int type);
void printtokens();


lexeme *lexanalyzer(char *input, int printFlag)
{
    list = malloc(MAX_NUMBER_TOKENS * sizeof(lexeme));

    // using i and j to remove all comments.
    int i = 0, j = 0, lineNumber = 1;

    // loop to remove all comments till eof.
    while (input[i] != '\0')
    {
        // skips line comments
        if (input[i] == '/' && input[i + 1] == '/')
        {
            while (input[i] != '\n')
                i++;
            i++;
        }

        // skips block comments
        if (input[i] == '/' && input[i + 1] == '*')
        {
            while (input[i] != '*' && input[i + 1] != '/')
                i++;
            i += 2;
        }

        // setting the input char forward ahead of comments found each loop.
        input[j++] = input[i++];
    }

    input[j] = '\0';

    // resetting iterators.
    j = 0; i = 0, lex_index = 0;

    lexeme temp;

    while (input[i] != '\0' && i < MAX_NUMBER_TOKENS)
    {
        // moving past whitespace of any kind.
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\f' || input[i] == '\v')
        {
            i++;
            continue;
        }

        // newline.
        if (input[i] == '\n')
        {
            if (input[i - 1] == '\r')
            {
                i++;
            }

            i++;
            lineNumber++;

            continue;
        }

        // Our c's. Callsym and constsym
        if (input[i] == 'c')
        {

            if (input[i + 1] == 'a' && input[i + 2] == 'l' && input[i + 3] == 'l' && isalpha(input[i + 4]) == 0 )
            {
                temp.type = callsym;
                strcpy(temp.name, "call");
                temp.value = callsym;
                list[lex_index++] = temp;

                i += 4;
                continue;
            }

            if (input[i + 1] == 'o' && input[i + 2] == 'n' && input[i + 3] == 's' && input[i + 4] == 't' && isalpha(input[i + 5]) == 0 )
            {
                temp.type = constsym;
                strcpy(temp.name, "const");
                temp.value = constsym;
                list[lex_index++] = temp;

                i += 5;
                continue;
            }
        }

      /* // beginsym.
        if (input[i] == 'b' && input[i + 1] == 'e' && input[i + 2] == 'g' && input[i + 3] == 'i' && input[i + 4] == 'n' && isalpha(input[i + 5]) == 0 )
        {
            temp.type = beginsym;
            strcpy(temp.name, "begin");
            temp.value = beginsym;
            list[lex_index++] = temp;

            i += 5;
            continue;
        }*/

        // dosym.
        if (input[i] == 'd' && input[i + 1] == 'o' && isalpha(input[i + 2]) == 0 )
        {
            temp.type = dosym;
            strcpy(temp.name, "do");
            temp.value = dosym;
            list[lex_index++] = temp;

            i += 2;
            continue;
        }

        // ifsym.
        /*if (input[i] == 'i' && input[i + 1] == 'f' && isalpha(input[i + 2]) == 0 )
        {
            temp.type = ifsym;
            strcpy(temp.name, "if");
            temp.value = ifsym;
            list[lex_index++] = temp;

            i += 2;
            continue;
        }*/

        // varsym.
        if (input[i] == 'v' && input[i + 1] == 'a' && input[i + 2] == 'r' && input[i + 3] == ' ' )
        {
            temp.type = varsym;
            strcpy(temp.name, "var");
            temp.value = varsym;
            list[lex_index++] = temp;

            i += 3;
            continue;
        }

        // readsym
        if (input[i] == 'r' && input[i + 1] == 'e' && input[i + 2] == 'a' && input[i + 3] == 'd' && isalpha(input[i + 4]) == 0 )
        {
            temp.type = readsym;
            strcpy(temp.name, "read");
            temp.value = readsym;
            list[lex_index++] = temp;

            i += 4;
            continue;
        }

        // procsym.
        if (input[i] == 'p' && input[i + 1] == 'r' && input[i + 2] == 'o' && input[i + 3] == 'c' && input[i + 4] == 'e' && input[i + 5] == 'd' &&
            input[i + 6] == 'u' && input[i + 7] == 'r' && input[i + 8] == 'e' && input[i + 9] == ' ')
        {
            temp.type = procsym;
            strcpy(temp.name, "procedure");
            temp.value = procsym;
            list[lex_index++] = temp;

            i += 9;
            continue;
        }

        // whensym.
        if (input[i] == 'w' && input[i + 1] == 'h' && input[i + 2] == 'e' && input[i + 3] == 'n' && isalpha(input[i + 4]) == 0 )
        {
            temp.type = whensym;
            strcpy(temp.name, "when");
            temp.value = whensym;
            list[lex_index++] = temp;

            i += 4;
            continue;
        }

        // periodsym.
        if (input[i] == '.')
        {
            temp.type = periodsym;
            strcpy(temp.name, ".");
            temp.value = periodsym;
            list[lex_index++] = temp;

            i += 1;
            continue;
        }

        // w's. while and write.
        if (input[i] == 'w')
        {
            if (input[i + 1] == 'h' && input[i + 2] == 'i' && input[i + 3] == 'l' && input[i + 4] == 'e' && isalpha(input[i + 5]) == 0 )
            {
                temp.type = whilesym;
                strcpy(temp.name, "while");
                temp.value = whilesym;
                list[lex_index++] = temp;

                i += 5;
                continue;
            }

            if (input[i + 1] == 'r' && input[i + 2] == 'i' && input[i + 3] == 't' && input[i + 4] == 'e' && isalpha(input[i + 5]) == 0 )
            {
                temp.type = writesym;
                strcpy(temp.name, "write");
                temp.value = writesym;
                list[lex_index++] = temp;

                i += 5;
                continue;
            }
        }

        // semicolonsym
        if (input[i] == ';')
        {
            temp.type = semicolonsym;
            strcpy(temp.name, ";");
            temp.value = semicolonsym;
            list[lex_index++] = temp;

            i++;
            continue;
        }

        // := (assignsym).
        if (input[i] == ':' && input[i + 1] == '=')
        {
            temp.type = assignsym;
            strcpy(temp.name, ":=");
            temp.value = assignsym;
            list[lex_index++] = temp;

            i += 2;
            continue;
        }

        // commasym.
        if (input[i] == ',')
        {
            temp.type = commasym;
            strcpy(temp.name, ",");
            temp.value = commasym;
            list[lex_index++] = temp;

            i++;
            continue;
        }

        // lparensym.
        if (input[i] == '(')
        {
            temp.type = lparensym;
            strcpy(temp.name, "(");
            temp.value = lparensym;
            list[lex_index++] = temp;

            i++;
            continue;
        }

        // rparensym.
        if (input[i] == ')')
        {
            temp.type = rparensym;
            strcpy(temp.name, ")");
            temp.value = rparensym;
            list[lex_index++] = temp;

            i++;
            continue;
        }

        // eqlsym
        if (input[i] == '=') {
            temp.type = eqlsym;
            strcpy(temp.name, "=");
            temp.value = eqlsym;
            list[lex_index++] = temp;

            i += 2;
            continue;
        }

        // addsym.
        if (input[i] == '+') {
            temp.type = addsym;
            strcpy(temp.name, "+");
            temp.value = addsym;
            list[lex_index++] = temp;

            i++;
            continue;
        }

        // subsym.
        if (input[i] == '-') {
            temp.type = subsym;
            strcpy(temp.name, "-");
            temp.value = subsym;
            list[lex_index++] = temp;

            i++;
            continue;
        }

        // multsym.
        if (input[i] == '*')
        {
            temp.type = multsym;
            strcpy(temp.name, "*");
            temp.value = multsym;
            list[lex_index++] = temp;

            i++;
            continue;
        }

        // oddsym.
        if (input[i] == 'o' && input[i + 1] == 'd' && input[i + 2] == 'd' )
        {
            temp.type = oddsym;
            strcpy(temp.name, "odd");
            temp.value = oddsym;
            list[lex_index++] = temp;

            i += 3;
            continue;
        }

        // divsym
        if (input[i] == '/')
        {
            temp.type = divsym;
            strcpy(temp.name, "/");
            temp.value = divsym;
            list[lex_index++] = temp;

            i++;
            continue;
        }

        // geqsym.
        if (input[i] == '>')
        {
            if (input[i + 1] == '=')
            {
                temp.type = geqsym;
                strcpy(temp.name, ">=");
                temp.value = geqsym;
                list[lex_index++] = temp;

                i += 2;
                continue;
            }

            temp.type = gtrsym;
            strcpy(temp.name, ">");
            temp.value = gtrsym;
            list[lex_index++] = temp;

            i++;
            continue;
        }

        // elsesym.
       /* if (input[i] == 'e')
        {

            if (input[i + 1] == 'l' && input[i + 2] == 's' && input[i + 3] == 'e' && isalpha(input[i + 4]) == 0 )
            {
                temp.type = elsesym;
                strcpy(temp.name, "else");
                temp.value = elsesym;
                list[lex_index++] = temp;

                i += 4;
                continue;
            }
        }*/

        // thensym.
       /* if (input[i] == 't' && input[i + 1] == 'h' && input[i + 2] == 'e' && input[i + 3] == 'n' && isalpha(input[i + 4]) == 0 )
        {
            temp.type = thensym;
            strcpy(temp.name, "then");
            temp.value = thensym;
            list[lex_index++] = temp;

            i += 4;
            continue;
        }*/

        // elsedo statement.
        if (input[i] == 'e' && input[i + 1] == 'l' && input[i + 2] == 's' && input[i + 3] == 'e' && input[i + 4] == 'd' && input[i + 5] == 'o' && isalpha(input[i + 6]) == 0)
        {
            temp.type = elsedosym;
            strcpy(temp.name, "elsedo");
            temp.value = elsedosym;
            list[lex_index++] = temp;

            i += 6;
            continue;
        }

        if (input[i] == '!' && input[i + 1] == '=')
            {
                temp.type = neqsym;
                strcpy(temp.name, "!=");
                temp.value = neqsym;
                list[lex_index++] = temp;

                i += 2;
                continue;
            }

        // leqsym. neqsym. lesssym.
        if (input[i] == '<')
        {
            if (input[i + 1] == '=')
            {
                temp.type = leqsym;
                strcpy(temp.name, "<=");
                temp.value = leqsym;
                list[lex_index++] = temp;

                i += 2;
                continue;
            }

            temp.type = lsssym;
            strcpy(temp.name, "<");
            temp.value = lsssym;
            list[lex_index++] = temp;

            i++;
            continue;
        }

        // modsym
        if (input[i] == '%')
        {
            temp.type = modsym;
            strcpy(temp.name, "%");
            temp.value = modsym;
            list[lex_index++] = temp;

            i++;
            continue;
        }

        // odsym.
        if (input[i] == 'o' && input[i + 1] == 'd' && isalpha(input[i + 2]) == 0)
        {
            temp.type = odsym;
            strcpy(temp.name, "od");
            temp.value = odsym;
            list[lex_index++] = temp;

            i += 2;
            continue;
        }

        // If we made it here, we have literals.
        // Not sure if we will get upper case but i included them in if statement.

        char idHolder[MAX_IDENT_LEN]; char digHolder[MAX_IDENT_LEN];
        int counter = 0;
		j = 0;

        if ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z'))
        {
			// idHolder initializes to next letter
            idHolder[j] = input[i++];

            while ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z') || isdigit(input[i]) != 0)
            {
                j++;
                if (j > MAX_IDENT_LEN)
				{
                    printlexerror(4);
					exit(4);
				}

                idHolder[j] = input[i];
                i++;
            }

			// took me a long time to remember to add the null terminator to the string. fixed all my string storage problems.
            temp.type = identsym;
			idHolder[j + 1] = '\0';
            strcpy(temp.name, idHolder);
            temp.value = identsym;

            list[lex_index++] = temp;
            continue;
        }

        // digit literal.
        if (isdigit(input[i]) != 0)
        {
            digHolder[counter] = input[i];
            i++;

            while (isdigit(input[i]) != 0)
            {
                counter++;
                if (counter > MAX_NUMBER_LEN)
				{
                    printlexerror(3);
					exit(3);
				}

                digHolder[counter] = input[i];
                i++;
            }

			if (isalpha(input[i]))
			{
				printlexerror(2);
				exit(2);
			}

			// i think we need a \0 here too.
            temp.type = numbersym;
			digHolder[counter + 1] = '\0';
			temp.value = atoi(digHolder);

            list[lex_index++] = temp;
            continue;
        }

        // if we made it here, invalid char input.
        printtokens();
        printlexerror(1);
		exit(1);
    }
    if (printFlag == 1)
        printtokens();

    // returning to caller in driver.c.
	return list;

}


void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "=", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);        // dosym
				break;
			/*case endsym:
				printf("%11s\t%d", "end", endsym);
				break;*/
			case whensym:
				printf("%11s\t%d", "when", whensym);
				break;
			/*case thensym:
				printf("%11s\t%d", "then", thensym);
				break;*/
			/*case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;*/
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case odsym:
				printf("%11s\t%d", "od", odsym);            // odsym
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
            case elsedosym:
                printf("%11s\t%d", "elsedo", elsedosym);
                break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}
