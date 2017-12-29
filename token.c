/*
 * token.c
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ctype.h"
#include "token.h"



int initializeToken(token *theToken , char* stringTok) 
{

	int i;
	int length = strlen(stringTok);

	if(!strcmp(stringTok , "defun")) {
		theToken->type = 0;
		return 0;
	}
	else if(!strcmp(stringTok , "return")) {
		theToken->type = 2;
		return 0;	
	}

	else if(!strcmp(stringTok , "+")) {
		theToken->type = 3;
		return 0;
	}
	else if(!strcmp(stringTok , "-")) {
		theToken->type = 4;
		return 0;
	}
	else if(!strcmp(stringTok , "*")) {
		theToken->type = 5;
		return 0;
	}
	else if(!strcmp(stringTok , "/")) {
		theToken->type = 6;
		return 0;
	}
	else if(!strcmp(stringTok , "%")) {
		theToken->type = 7;
		return 0;
	}
	else if(!strcmp(stringTok , "and")) {
		theToken->type = 8;
		return 0;
	}
	else if(!strcmp(stringTok , "or")) {
		theToken->type = 9;
		return 0;
	}
	else if(!strcmp(stringTok , "not")) {
		theToken->type = 10;
		return 0;
	}
	else if(!strcmp(stringTok , "lt")) {
		theToken->type = 11;
		return 0;
	}
	else if(!strcmp(stringTok , "le")) {
		theToken->type = 12;
		return 0;
	}
	else if(!strcmp(stringTok , "eq")) {
		theToken->type = 13;
		return 0;
	}
	else if(!strcmp(stringTok , "ge")) {
		theToken->type = 14;
		return 0;
	}
	else if(!strcmp(stringTok , "gt")) {
		theToken->type = 15;
		return 0;
	}
	else if(!strcmp(stringTok , "if")) {
		theToken->type = 16;
		return 0;
	}
	else if(!strcmp(stringTok , "else")) {
		theToken->type = 17;
		return 0;
	}
	else if(!strcmp(stringTok , "endif")) {
		theToken->type = 18;
		return 0;
	}
	else if(!strcmp(stringTok , "drop")) {
		theToken->type = 19;
		return 0;
	}
	else if(!strcmp(stringTok , "dup")) {
		theToken->type = 20;
		return 0;
	}
	else if(!strcmp(stringTok , "swap")) {
		theToken->type = 21;
		return 0;
	}
	else if(!strcmp(stringTok , "rot")) {
		theToken->type = 22;
		return 0;
	}

	//arg is either length 4 or 5
	if(length > 3  && length < 6){
		if(stringTok[0] == 'a' && stringTok[1] == 'r' && stringTok[2] == 'g') {
			theToken->type = 23;
			if(length == 4 && isdigit(stringTok[3])) {
				theToken->arg_no = (int) stringTok[3] - 48;
				return 0;
			}

			else if(length == 5 && isdigit(stringTok[3]) && isdigit(stringTok[4])) {
				char number[2] = {stringTok[3] , stringTok[4]};
				int arg_number = atoi(number);
				if(arg_number > 20) {
					printf("argument number out of bounds\n");
					return 1;
				}
			

				theToken->arg_no = arg_number;
				return 0;
			}
		}
	}

	//hex literal
	if(stringTok[0] == '0' && stringTok[1] == 'x') {
		for(i = 2; i < length; i++) {
			if(!isxdigit(stringTok[i]))
				printf("broken token!\n");
		}

		theToken->type = 24;
		theToken->literal_value = strtol(stringTok , NULL , 16);
		return 0;
	}
	//decimal
	else if(stringTok[0] == '-' || isdigit(stringTok[0])) 
	{
		
		for(i = 1; i < length; i++) {
			if(!isdigit(stringTok[i])) {
				printf("illegl number\n");
				return 1;
			}
		}

		theToken->type = 24;
		theToken->literal_value = atoi(stringTok);
		return 0;
		
	
	}

	else 
	{
		if(!isalpha(stringTok[0])) {
			printf("illegal leading char\n");
			return 1;
		}

		for(i = 1; i < length; i++) {
			if(!(isalnum(stringTok[i]) || stringTok[i] == '_')) {
				printf("Illegal name\n");
				return 1;
			}
		}
		theToken->type = 1;
		strcpy(theToken->str , stringTok);


		return 0;
	}

	//if no value has been returned yet, it is an illegal token
	return 1;
}

