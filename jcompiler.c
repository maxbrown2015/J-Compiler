/*
 * jcompiler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctype.h"
#include "token.h"
#include "tokenWriter.h"




int main(int argc, char** argv) {
	FILE *inputFile;
	FILE *outputFile;
	token *theToken;

	char *filename = malloc(strlen(argv[1]) + sizeof(char) * 2);
	char *jFileName = malloc(250 * sizeof(char));
	char *asmFileEnd = malloc(5 * sizeof(char));
	strcpy(asmFileEnd , ".asm");
	strcpy(jFileName , argv[1]);
	strcpy(filename , "");

	filename = strncpy(filename , jFileName,  strlen(jFileName) - 2);
	strcat(filename , asmFileEnd);


	inputFile = fopen(argv[1], "r");


	if(inputFile == NULL) {
		printf("input File is null -> ERROR\n");
		free(filename);
		free(jFileName);
		free(asmFileEnd);
		return 0;
	}

	outputFile = fopen(filename , "w");
	
	char *line = malloc(250 * sizeof(char));
	char *tokens;
	int count = 0;
	while(!feof(inputFile)) 
	{	
		

		fgets(line , 250, inputFile);

		if(line == NULL) {
			break;
		}

		if (strcmp(line, "\n") == 0 || line[0] == ';')
      		continue;


		tokens = strtok(line , "\n\t ");	

		while(tokens != NULL) {
			if(tokens[0] == ';') {
				printf("\n");
				break;
			}

			theToken = malloc(sizeof(*theToken));
			if(initializeToken(theToken, tokens) == 1) {
				printf("ERROR initializing token\n");
				free(filename);
				free(jFileName);
				free(asmFileEnd);
				free(line);
				free(theToken);
				return 0;
			}

			if(writeToken(theToken , outputFile) == 1) {
				printf("ERROR writing token\n");
				free(filename);
				free(jFileName);
				free(asmFileEnd);
				free(line);
				free(theToken);
				return 0;
			}

			free(theToken);
			tokens = strtok(NULL, "\n\t ");

		}
		

	}

	free(line);
	free(jFileName);
	free(filename);
	free(asmFileEnd);

	fclose(outputFile);
	fclose(inputFile);

	return 0;


}