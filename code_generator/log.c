#include "log.h"
#include "lexer.h"
//print the information with stderr or stdout


//print an information about error to the file
void showError(FILE* f, int col, int row, char* record, enum ErrorType error, enum outputType out, char* backReck) {
	switch (error) {
	case illegalChar:
		fprintf(f, "\"Lexer: Error (line %d, column %d): Illegal charachter '%s' detected\"\n", row, col, record);
		if (out == errorOut)
			fprintf(stderr, "\"Lexer: Error (line %d, column %d): Illegal charachter '%s' detected\"\n", row, col, record);
		else if (out == infoOut)
			fprintf(stdout, "\"Lexer: Error (line %d, column %d): Illegal charachter '%s' detected\"\n", row, col, record);
		break;
	case endFile:
		fprintf(f, "\"Lexer: Error (line %d, column %d): Expected '%s' but end of file has occured\"\n", row, col, record);
		if (out == errorOut)
			fprintf(stderr, "\"Lexer: Error (line %d, column %d): Expected '%s' but end of file has occured\"\n", row, col, record);
		else if (out == infoOut)
			fprintf(stdout, "\"Lexer: Error (line %d, column %d): Expected '%s' but end of file has occured\"\n", row, col, record);
		break;
	case tokenCrash:
		fprintf(f, "\"Lexer: Error (line %d, column %d): Got %s symbol error has occured\"\n", row, col, record);
		if (out == errorOut)
			fprintf(stderr, "\"Lexer: Error (line %d, column %d): Got %s symbol error has occured\"\n", row, col, record);
		else if (out == infoOut)
			fprintf(stdout, "\"Lexer: Error (line %d, column %d): Got %s symbol error has occured\"\n", row, col, record);
		break;
	case cmpErrorF1:
		fprintf(f, "==>>\"compareFile: Error (line %d, column %d): In expected file '%s' symbol has occured\n", row, col, record);
		if (out == errorOut)
			fprintf(stderr, "compareFile: Error (line %d, column %d): In expected file '%s' symbol has occured\"\n", row, col, record);
		else if (out == infoOut)
			fprintf(stdout, "compareFile: Error (line %d, column %d): In expected file '%s' symbol has occured\"\n", row, col, record);
		break;
	case cmpErrorF2:
		fprintf(f, "----(line %d, column %d): while in generated file '%s' symbol has occured\"\n", row, col, record);
		if (out == errorOut)
			fprintf(stderr, "(line %d, column %d): while in generated file '%s' symbol has occured\"\n", row, col, record);
		else if (out == infoOut)
			fprintf(stdout, "(line %d, column %d): while in generated file '%s' symbol has occured\"\n", row, col, record);
		break;
	case wrongWord:
		fprintf(f, "\"Parser: Error (line %d, column %d): '%s' expected  but '%s' found\"\n", row, col, backReck, record);
		if (out == errorOut)
			fprintf(stderr, "\"Parser: Error (line %d, column %d): '%s' expected  but '%s' found\"\n", row, col, backReck, record);
		else if (out == infoOut)
			fprintf(stdout, "\"Parser: Error (line %d, column %d): '%s' expected  but '%s' found\"\n", row, col, backReck, record);
		break;
	case repVar:
		fprintf(f, "\"Code Generator: Error (line %d, column %d): multiple var '%s' declaration\"\n", row, col, record);
		if (out == errorOut)
			fprintf(stderr, "\"Code Generator: Error (line %d, column %d): multiple var '%s' declaration\"\n", row, col, record);
		else if (out == infoOut)
			fprintf(stdout, "\"Code Generator: Error (line %d, column %d): multiple var '%s' declaration\"\n", row, col, record);
		break;
	case repInit:
		fprintf(f, "\"Code Generator: Error (line %d, column %d): var '%s' is undefined\"\n", row, col, record);
		if (out == errorOut)
			fprintf(stderr, "\"Code Generator: Error (line %d, column %d): var '%s' is undefined\"\n", row, col, record);
		else if (out == infoOut)
			fprintf(stdout, "\"Code Generator: Error (line %d, column %d): var '%s' is undefined\"\n", row, col, record);
		break;
	case repWord:
		fprintf(f, "\"Code Generator: Error (line %d, column %d): var '%s' is already used in program\"\n", row, col, record);
		if (out == errorOut)
			fprintf(stderr, "\"Code Generator: Error (line %d, column %d): var '%s' is already used in program\"\n", row, col, record);
		else if (out == infoOut)
			fprintf(stdout, "\"Code Generator: Error (line %d, column %d): var '%s' is already used in program\"\n", row, col, record);
		break;
	case onlyNum:
		fprintf(f, "\"Code Generator: Error (line %d, column %d): expected only digital constant values but identificator '%s' is found\"\n", row, col, record);
		if (out == errorOut)
			fprintf(stderr, "\"Code Generator: Error (line %d, column %d): expected only digital constant values but identificator '%s' is found\"\n", row, col, record);
		else if (out == infoOut)
			fprintf(stdout, "\"Code Generator: Error (line %d, column %d): expected only digital constant values but identificator '%s' is found\"\n", row, col, record);
		break;
	case onlyIdn:
		fprintf(f, "\"Code Generator: Error (line %d, column %d): expected only identificator values but digital constant value '%s' is found\"\n", row, col, record);
		if (out == errorOut)
			fprintf(stderr, "\"Code Generator: Error (line %d, column %d): expected only identificator values but digital constant value '%s' is found\"\n", row, col, record);
		else if (out == infoOut)
			fprintf(stdout, "\"Code Generator: Error (line %d, column %d): expected only identificator values but digital constant value '%s' is found\"\n", row, col, record);
		break;
	}
	
}
	


//displaying of tables
void displayTable(struct nlist** htab, int startPoint, FILE* outGen) {
	int i, size;
	char* bufWord = NULL;
	if (startPoint == 500) {
		bufWord = "\nTable of constants:\n";
		size = CONSTIDNSIZE;
	}
	else if (startPoint == 1000) {
		bufWord = "\nTable of identificators:\n";
		size = IDNSIZE;
	}
	else if (startPoint == 10000) {
		bufWord = "\nTable of tasks:\n";
		size = TOKENSIZE;
	}
	startPoint++;
	struct nlist* t = lookForCode(startPoint, htab, size);
	short fullness = 0;
	while (t != NULL) {
		if (fullness == 0)
			fprintf(outGen, "%s", bufWord);
		fprintf(outGen, "%-5d %-10s\n", t->defn, t->name);
		startPoint++;
		t = lookForCode(startPoint, htab, size);
		fullness++;
	}
}

//displaying of lexical row
void displayLexRow(struct tokenStruct** ltab) {
	int i = 0, size = LEXSIZE;
	while (ltab[i] != NULL) {
		printf("%-3d %-3d %-8d %-10s\n", ltab[i]->col, ltab[i]->row, ltab[i]->key, ltab[i]->lexem);
		i++;
	}
}

//comparing of expected and generated files symbol by symbol
void compareSymbols(char* argvParameter, FILE* expected, FILE* generated, FILE* crashLog) {
	char expCh, genCh;
	int col_exp = 0, row_exp = 0, col_gen = 0, row_gen = 0;
	char* buf;
	unsigned ifEqual = 0, expEOF = 0, genEOF = 0;
	rewind(generated);
	fprintf(crashLog, "#->Reading data on the '%s' location...\n", argvParameter);
	do {
		expCh = fgetc(expected);
		if (expCh == EOF)
			expEOF = 1;
		if (!expEOF)
			updateRecAttr(&row_exp, &col_exp, expCh);
		genCh = fgetc(generated);
		if (genCh == EOF)
			genEOF = 1;
		if (!genEOF)
			updateRecAttr(&row_gen, &col_gen, genCh);
		if (expCh != genCh) {
			buf = NULL;
			if (expEOF) {
				buf = add_char(buf, (char)'E');
				buf = add_char(buf, (char)'O');
				buf = add_char(buf, (char)'F');
			}
			else
				buf = add_char(buf, expCh);
			showError(crashLog, col_exp, row_exp, buf, cmpErrorF1, noneOut, NULL);
			free(buf);
			buf = NULL;
			if (genEOF) {
				buf = add_char(buf, (char)'E');
				buf = add_char(buf, (char)'O');
				buf = add_char(buf, (char)'F');
			}
			else
				buf = add_char(buf, genCh);
			showError(crashLog, col_gen, row_gen, buf, cmpErrorF2, noneOut, NULL);
			free(buf);
			ifEqual++;
		}
	} while (expCh != EOF || genCh != EOF);
	if (ifEqual != 0)
		fprintf(crashLog, "<<(Files comparison ended with %d differences)>>\n", ifEqual);
	else
		fprintf(crashLog, "<<(Compared files are equal)>>\n");
}

//print the parse tree to the file
void printTree(node* tree, struct errorHandling* erH, FILE* outParse, short depth, int* error_handler)
{
	if (tree == NULL)
		return;
	if (depth == 0) 
		fprintf(outParse, "\nParse tree:\n");
	char* buf = NULL;
	for (int i = 0; i < tree->depth; i++) {
		buf = add_char(buf, (char)'.');
		buf = add_char(buf, (char)'.');
	}
	if (buf != NULL) {
		fprintf(outParse, "%s", buf);
	}
	free(buf);
	if (tree)
	{
		if (tree->lexCode == -2) {
			fprintf(outParse, "%s\n", tree->lexName);
			erH = erH->nxt;
			showError(outParse, erH->column, erH->row, erH->lexemCur, wrongWord, noneOut, erH->lexemExp);
			*error_handler = 1;
			return;
		}
		else if (tree->lexCode == -1) {
			fprintf(outParse, "%s\n", tree->lexName);
		}
		else {
			fprintf(outParse, "%d %s\n", tree->lexCode, tree->lexName);
		}
		printTree(tree->next, erH, outParse, ++depth, &(*error_handler));

	}
}