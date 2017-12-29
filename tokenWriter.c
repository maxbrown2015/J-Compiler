/*
 * tokenWriter.c
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ctype.h"
#include "token.h"
#include "tokenWriter.h"
#include "stack.h"


//boolean value that is true if defun is last token read
int isDefunLabel = 0;
//increasing variable which differentiaes different branchs from compare
int branchCounter = 0;

int ifCounter = 0;
int endifCounter = 0;
int currIf = 0;

char routineName[1000] = {0};

stack *theStack;

int writeToken (token *theToken, FILE *theFile) {

	char outputString[4000]  = {0};
	//pops two elements of the stack (SP = R6) and stores them in R1 and R2
	char *pop_two = "\tLDR R1 R6 #0\n\tADD R6 R6 #1\n\tLDR R2 R6 #0\n\tADD R6 R6 #1\n";
	char *pop_one = "\tLDR R1 R6 #0\n\tADD R6 R6 #1\n";
	char *pop_three = "\tLDR R1 R6 #0\n\tADD R6 R6 #1\n\tLDR R2 R6 #0\n\tADD R6 R6 #1\n\tLDR R3 R6 #0\n\tADD R6 R6 #1\n";
	char *push_literal = "\tCONST R3 #";
	char *push_literalHICONST = "\tHICONST R3 #";
	char *addOut = "\tADD R1 R1 R2\n";
	char *subOut = "\tSUB R1 R1 R2\n";
	char *mulOut = "\tMUL R1 R1 R2\n";
	char *divOut = "\tDIV R1 R1 R2\n";
	char *modOut = "\tMOD R1 R1 R2\n";
	char *andOut = "\tAND R1 R1 R2\n";
	char *orOut = "\tOR R1 R1 R2\n";
	char *notOut = "\tNOT R1 R1 R2\n";
	char *ltOut = "\tBRn ";
	char *leOut = "\tBRnz ";
	char *eqOut = "\tBRz ";
	char *geOut = "\tBRzp ";
	char *gtOut = "\tBRp ";
	//moves stackPointer down, removing top element
	char *dropOut = "\tADD R6 R6 #1\n";
	//loads top of the stack to register, stores in SP -1, and decrements SP--;
	char *dupOut = "\tLDR R1 R6 #0\n\tSTR R1 R6 #-1\n\tADD R6 R6 #-1\n";
	//after popping top two elements, store them in opposite order from their popping
	char *swapOut = "\tSTR R1 R6 #-1\n\tADD R6 R6 #-1\n\tSTR R2 R6 #-1\n\tADD R6 R6 #-1\n";
	//pop 3 elements, and then rotate them 
	char *rotOut = "\tSTR R2 R6 #-1\n\tADD R6 R6 #-1\n\tSTR R1 R6 #-1\n\tADD R6 R6 #-1\n\tSTR R3 R6 #-1\n\tADD R6 R6 #-1\n";

	//if not a function-header, function call
	char *ident = "\tJSR ";
	//shifts SP 
	char *postCall = "\tADD R6 R6 #-1\n";

	char *argOut = "\tLDR R3 R5 #";
	//stores a local variable to the top of the stack and decrements SP
 	char *storeVariable = "\tSTR R3 R6 #-1\n\tADD R6 R6 #-1\n";
 	//stores output back on the stack
 	char *storeOutput = "\tSTR R1 R6 #-1\n\tADD R6 R6 #-1\n";

 	char *CMP = "\tCMP R1 R2\n";

 	//prologue: allocate three spots on the stack, save the Return address, save the FP, and set FP to SP
 	char *prologue = "\tADD R6 R6 #-3\n\tSTR R7 R6 #1\n\tSTR R5 R6 #0\n\tADD R5 R6 #0\n";
 	//epilogue: save the return value, update SP, update FP, return RA, free three stack places, jump to R7 (RA)
 	char *epilogue = "\tLDR R7 R6 #0\n\tSTR R7 R5 #2\n\tADD R6 R5 #0\n\tLDR R5 R6 #0\n\tLDR R7 R6 #1\n\tADD R6 R6 #3\n\tJMPR R7\n";


	char *put0 = "\tCONST R0 #0\n";
	char *put1  = "\tCONST R0 #1\n";
	char *storeBoolean = "\tSTR R0 R6 #-1\n\tADD R6 R6 #-1\n";
	char *JMPEnd = "\tJMP END_";

	char *branchNumber = malloc(250 * sizeof(char));
	char *branchLabel = malloc(250 * sizeof(char));
	char *endLabel = malloc(250 * sizeof(char));

	char *ifOut = "IF_";
	char *elseOut = "ELSE_";
	char *endif = "END_IF_";
	char *ifCompare = "\tCMPI R1 #0\n";

	char *ifNumberString = malloc(100 * sizeof(char));

	char *argNumberToString = malloc(1000 * sizeof(char));
	char  *argOutFull = malloc(1000 * sizeof(char));
	//value for hex / decimal literals
	int value;
	//arg number + 2 to offset from FP
	int argNumberOffset;
	switch(theToken->type) 
	{
		//defun
		case 0: 
			//defun
			isDefunLabel = 1;
			break;
		case 1: 
			//ident
			if(isDefunLabel == 1) {

				//function header 
				strcat(outputString , "\t\t .CODE\n");
				strcat(outputString , "\t\t .FALIGN\n");
				strcat(outputString , theToken->str);
				strcat(outputString , "\n");
				//handlle prologue
				strcat(outputString , "\t;;;;prologue;;;;\n");
				strcat(outputString , prologue);
				isDefunLabel = 0;
				strcat(outputString , "\t;;;function body;;;;\n");
				strcpy(routineName , theToken->str);
			}

			else {
			//function call
			strcat(outputString , ident);			
			strcat(outputString , theToken->str);
			strcat(outputString , "\n");
			strcat(outputString , postCall);
			}
			break;
		case 2: 
			//return
			strcat(outputString , "\t;;;;epilogue;;;;\n");
			strcat(outputString , epilogue);
			break;
		case 3: 
			//add
			strcat(outputString , pop_two);
			strcat(outputString , addOut);
			strcat(outputString , storeOutput);
			break;	
		case 4: 
			//sub
			strcat(outputString , pop_two);
			strcat(outputString , subOut);
			strcat(outputString , storeOutput);		

			break;
		case 5: 
			//mul
			strcat(outputString , pop_two);
			strcat(outputString , mulOut);	
			strcat(outputString , storeOutput);		
			break;				
		case 6: 
			//div
			strcat(outputString , pop_two);
			strcat(outputString , divOut);
			strcat(outputString , storeOutput);		
			break;
		case 7: 
			//mod
			strcat(outputString , pop_two);
			strcat(outputString , modOut);
			strcat(outputString , storeOutput);		
			break;
		case 8: 
			//and
			strcat(outputString , pop_two);
			strcat(outputString , andOut);	
			strcat(outputString , storeOutput);				
			break;		
		case 9: 
			//or
			strcat(outputString , pop_two);
			strcat(outputString , orOut);	
			strcat(outputString , storeOutput);		
			break;
		case 10: 
			//not
			strcat(outputString , pop_two);
			strcat(outputString , notOut);	
			strcat(outputString , storeOutput);		
			break;
		case 11: 
			//lt
			strcat(outputString , pop_two);
			strcat(outputString , CMP);
			strcat(outputString , ltOut);

			sprintf(branchNumber , "%d" , branchCounter);
			branchCounter++;
			strcpy(branchLabel , "lt_");
			strcat(branchLabel , branchNumber);

			strcat(outputString , branchLabel);
			strcat(outputString , "\n");

			strcat(outputString , put0);
			strcat(outputString , storeBoolean);
			strcat(outputString , JMPEnd);
			strcat(outputString, branchLabel);
			strcat(outputString , "\n");

			strcat(outputString, branchLabel);
			strcat(outputString , "\n");
			strcat(outputString , put1);
			strcat(outputString , storeBoolean);

			strcpy(endLabel , "END_");
			strcat(endLabel , "lt_");
			strcat(endLabel , branchNumber);
			strcat(endLabel , "\n");
			strcat(outputString , endLabel);

			break;
		case 12: 
			//le
			strcat(outputString , pop_two);
			strcat(outputString , CMP);
			strcat(outputString , leOut);

			sprintf(branchNumber , "%d" , branchCounter);
			branchCounter++;
			strcpy(branchLabel , "le_");
			strcat(branchLabel , branchNumber);

			strcat(outputString , branchLabel);
			strcat(outputString , "\n");

			strcat(outputString , put0);
			strcat(outputString , storeBoolean);
			strcat(outputString , JMPEnd);
			strcat(outputString, branchLabel);
			strcat(outputString , "\n");

			strcat(outputString, branchLabel);
			strcat(outputString , "\n");
			strcat(outputString , put1);
			strcat(outputString , storeBoolean);

			strcpy(endLabel , "END_");
			strcat(endLabel , "le_");
			strcat(endLabel , branchNumber);
			strcat(endLabel , "\n");
			strcat(outputString , endLabel);
			break;
		case 13: 
			//eq
			strcat(outputString , pop_two);
			strcat(outputString , CMP);
			strcat(outputString , eqOut);

			sprintf(branchNumber , "%d" , branchCounter);
			branchCounter++;
			strcpy(branchLabel , "eq_");
			strcat(branchLabel , branchNumber);

			strcat(outputString , branchLabel);
			strcat(outputString , "\n");

			strcat(outputString , put0);
			strcat(outputString , storeBoolean);
			strcat(outputString , JMPEnd);
			strcat(outputString, branchLabel);
			strcat(outputString , "\n");

			strcat(outputString, branchLabel);
			strcat(outputString , "\n");
			strcat(outputString , put1);
			strcat(outputString , storeBoolean);

			strcpy(endLabel , "END_");
			strcat(endLabel , "eq_");
			strcat(endLabel , branchNumber);
			strcat(endLabel , "\n");
			strcat(outputString , endLabel);
			break;
		case 14: 
			//ge
			strcat(outputString , pop_two);
			strcat(outputString , CMP);
			strcat(outputString , geOut);

			sprintf(branchNumber , "%d" , branchCounter);
			branchCounter++;
			strcpy(branchLabel , "ge_");
			strcat(branchLabel , branchNumber);

			strcat(outputString , branchLabel);
			strcat(outputString , "\n");

			strcat(outputString , put0);
			strcat(outputString , storeBoolean);
			strcat(outputString , JMPEnd);
			strcat(outputString, branchLabel);
			strcat(outputString , "\n");

			strcat(outputString, branchLabel);
			strcat(outputString , "\n");
			strcat(outputString , put1);
			strcat(outputString , storeBoolean);

			strcpy(endLabel , "END_");
			strcat(endLabel , "ge_");
			strcat(endLabel , branchNumber);
			strcat(endLabel , "\n");
			strcat(outputString , endLabel);

			break;
		case 15: 
			//gt
			strcat(outputString , pop_two);
			strcat(outputString , CMP);
			strcat(outputString , gtOut);

			sprintf(branchNumber , "%d" , branchCounter);
			branchCounter++;
			strcpy(branchLabel , "gt_");
			strcat(branchLabel , branchNumber);

			strcat(outputString , branchLabel);
			strcat(outputString , "\n");

			strcat(outputString , put0);
			strcat(outputString , storeBoolean);
			strcat(outputString , JMPEnd);
			strcat(outputString, branchLabel);
			strcat(outputString , "\n");

			strcat(outputString, branchLabel);
			strcat(outputString , "\n");
			strcat(outputString , put1);
			strcat(outputString , storeBoolean);

			strcpy(endLabel , "END_");
			strcat(endLabel , "gt_");
			strcat(endLabel , branchNumber);
			strcat(endLabel , "\n");
			strcat(outputString , endLabel);

			break;
		//see README.txt for if-else implementation
		case 16: 
			//if
			ifCounter++;
			theStack = push(theStack , ifCounter);
			sprintf(ifNumberString , "%d" , theStack->if_count);
			strcat(outputString , ifOut);
			strcat(outputString , routineName);
			strcat(outputString  ,ifNumberString);
			strcat(outputString , "\n");
			strcat(outputString, pop_one);
			strcat(outputString , ifCompare);
			strcat(outputString , "\tBRnz "  );
			strcat(outputString , elseOut);
			strcat(outputString , routineName);
			strcat(outputString  ,ifNumberString);
			strcat(outputString , "\n");

			break;
		case 17: 
			//else
			sprintf(ifNumberString , "%d" , theStack->if_count);
			strcat(outputString , "\tBRnzp ");
			strcat(outputString , endif);
			strcat(outputString , routineName);
			strcat(outputString  ,ifNumberString);
			strcat(outputString , "\n");
			strcat(outputString , elseOut);
			strcat(outputString , routineName);
			strcat(outputString  ,ifNumberString);
			strcat(outputString , "\n");
			break;
		case 18: 
			//endif
			sprintf(ifNumberString , "%d" , theStack->if_count);
			theStack = pop(theStack);
			strcat(outputString , endif);
			strcat(outputString , routineName);
			strcat(outputString  ,ifNumberString);
			strcat(outputString , "\n");
			break;
		case 19: 
			//drop
			strcat(outputString , dropOut);
			break;
		case 20: 
			//dup
			strcat(outputString , dupOut);
			break;	
		case 21: 
			//swap
			strcat(outputString , pop_two);
			strcat(outputString , swapOut);
			break;	
		case 22: 
			//rot
			strcat(outputString , pop_three);
			strcat(outputString , rotOut);
			break;
		case 23: 
			//arb
			argNumberOffset = theToken->arg_no + 2;
			sprintf(argNumberToString ,"%d" ,argNumberOffset);
			strcat(argOutFull , argOut);
			strcat(argOutFull , argNumberToString);
			strcat(outputString , argOutFull);
			strcat(outputString , "\n");
			strcat(outputString , storeVariable);
			break;
		case 24: 
			
		 	value = theToken->literal_value;
			char* value_str_lower = malloc(1000 * sizeof(char));
			char* value_str_upper = malloc(1000 * sizeof(char));
			char* value_str = malloc(1000 * sizeof(char));

			if(value > 256 || value < -256) {
				//handle
				int lower8bits = value & 0xFF;
				int upper8bits = (value >> 8) & 0xFF;
				sprintf(value_str_upper , "%d" , upper8bits);
				sprintf(value_str_lower , "%d" , lower8bits);
				strcat(outputString , push_literal);
				strcat(outputString , value_str_lower);
				strcat(outputString , "\n");
				strcat(outputString , push_literalHICONST);
				strcat(outputString , value_str_upper);
				strcat(outputString , "\n");
				strcat(outputString , storeVariable);
			}
			else {
		
				
				sprintf(value_str , "%d" , value);
				strcat(outputString , push_literal);
				strcat(outputString , value_str);
				strcat(outputString , "\n");
				strcat(outputString , storeVariable);

			}
			free(value_str);
			free(value_str_upper);
			free(value_str_lower);
			
			break;
		case 25: 
			//broken token case
			return 1;
			break;
		default:
			printf("what happened\n");
			return 1;
	}

	free(argNumberToString);
	free(argOutFull);

	free(branchLabel);
	free(endLabel);
	free(branchNumber);
	free(ifNumberString);


	fwrite(outputString , sizeof(char), strlen(outputString) , theFile);

	return 0;

}