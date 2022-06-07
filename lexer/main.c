#include "lexer.h"
#include "hash.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv) {
	FILE* inputProgram, *outputProgram;
	char *locationIn = NULL, *locationOut = NULL;
	int pos = 1;
	locationIn = malloc(strlen(argv[pos]) + 10);
	strcpy(locationIn, argv[pos]);
	strcat(locationIn, "/input.sig");
	if ((inputProgram = fopen(locationIn, "r")) == NULL) {
		fprintf(stderr, "The input file doesn`t exists");
		exit(1);
	}
	locationOut = malloc(strlen(argv[pos]) + 14);
	strcpy(locationOut, argv[pos]);
	strcat(locationOut, "/generated.txt");
	if ((outputProgram = fopen(locationOut, "w")) == NULL) {
		fprintf(stderr, "The input file doesn`t exists");
		exit(1);
	}
	//Initialization of all needed tables
	initializeTables();
	//generating of token table by reading data from inputProgram to outputProgram
	Lexer(inputProgram, outputProgram);
	//output constant table in file generated.txt
	displayTable(consttab, constWord, outputProgram);
	//output identificator table in file generated.txt
	displayTable(idntab, idnWord, outputProgram);
	displayTable(tocentab, tcnWord, outputProgram);
	//free allocated memory
	destroyTables();
	//close files
	free(locationIn);
	free(locationOut);
	fclose(inputProgram);
	fclose(outputProgram);
    return 0;
}
