#include "lexer.h"
#include "hash.h"
#include "log.h"
#include "parser.h"
#include "codegen.h"
#include <stdlib.h>
#include <string.h>

#pragma warning (disable:4996)

int main(int argc, char** argv) {
	FILE* inputProgram = NULL, *outputProgram = NULL, *comparedProgram = NULL, *crashLog = NULL;
	int pos = 1;
	
	//generating of the file that stores info about compared expected and generated files
	if ((crashLog = fopen("tests/crashLog/crashLog.txt", "w")) == NULL) {
		fprintf(stderr, "The 'crashLog.txt' file doesn`t exists");
		exit(1);
	}

	while (pos < argc) {
		char* locationeExp = NULL, * locationeGen = NULL, * locationIn = NULL;
		locationeGen = malloc(strlen(argv[pos]) + 15);
		strcpy(locationeGen, argv[pos]);
		strcat(locationeGen, "/generated.txt");

		locationeExp = malloc(strlen(argv[pos]) + 14);
		strcpy(locationeExp, argv[pos]);
		strcat(locationeExp, "/expected.txt");

		locationIn = malloc(strlen(argv[pos]) + 11);
		strcpy(locationIn, argv[pos]);
		strcat(locationIn, "/input.sig");
		
		if ((inputProgram = fopen(locationIn, "r")) == NULL) {
			fprintf(stderr, "The 'input.sig' file doesn`t exists");
			exit(1);
		}
		if ((outputProgram = fopen(locationeGen, "w+")) == NULL) {
			fprintf(stderr, "The 'generated.txt' file doesn`t exists");
			exit(1);
		}
		if ((comparedProgram = fopen(locationeExp, "r")) == NULL) {
			fprintf(stderr, "The 'expected.txt' file doesn`t exists");
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
		//output task table in file generated.txt
		displayTable(tokentab, tknWord, outputProgram);
		//displayLexRow(lexRow);
		//tree parse forming
		if(!sigProgram(outputProgram))
			generator(curNode, outputProgram);
		//printLeaves(curNode);
		//free allocated memory
		destroyTables();
		//comparing of the file print arguments like that
		//./a.out tests/test01 tests/test02 tests/test03 tests/test04 tests/test05 tests/test06 tests/test07 tests/test08
		//and get the results in crashLog.txt
		compareSymbols(argv[pos], comparedProgram, outputProgram, crashLog);
		pos++;
		free(locationeExp); free(locationeGen); free(locationIn);
		
		//break;
	}
	//close files
	fclose(inputProgram);
	fclose(outputProgram);
	fclose(comparedProgram);
	fclose(crashLog);
    return 0;
}
