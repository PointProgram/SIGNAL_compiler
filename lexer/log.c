#include "log.h"

void logF(char* record, enum outputType out) {
	if (out == errorOut) {
		fprintf(stderr, "%s", record);
	}
	else if (out == infoOut) {
		fprintf(stdout, "%s", record);
	}
}

void showError(FILE* f, int col, int row, char* record, enum ErrorType error, enum outputType out) {
	switch (error) {
	case illegalChar:
		fprintf(f, "\"Lexer: Error (line %d, column %d): Illegal charachter '%s' detected\"\n", row, col, record);
		logF(("\"Lexer: Error (line %d, column %d): Illegal charachter '%s' detected\"\n", row, col, record), out);
		break;
	case endFile:
		fprintf(f, "\"Lexer: Error (line %d, column %d): Expected '%s' but end of file has occured\"\n", row, col, record);
		logF(("\"Lexer: Error (line %d, column %d): Expected '%s' but end of file has occured\"\n", row, col, record), out);
		break;
	case tocenCrash:
		fprintf(f, "\"Lexer: Error (line %d, column %d): Got %s symbol error has occured\"\n", row, col, record);
		logF(("\"Lexer: Error (line %d, column %d): Got %s symbol error has occured\"\n", row, col, record), out);
		break;
	}
	
}

void displayTable(struct nlist** htab, int startPoint, FILE* outGen) {
	int i, size;
	if (startPoint == 500) {
		fprintf(outGen, "\nTable of constants:\n");
		size = CONSTIDNSIZE;
	}
	else if (startPoint == 1000) {
		fprintf(outGen, "\nTable of identificators:\n");
		size = IDNSIZE;
	}
	else if (startPoint == 10000) {
		fprintf(outGen, "\nTable of tasks:\n");
		size = TOCENSIZE;
	}
	startPoint++;
	struct nlist* t = lookForCode(startPoint, htab, size);
	while (t != NULL) {
		fprintf(outGen, "%-5d %-10s\n", t->defn, t->name);
		startPoint++;
		t = lookForCode(startPoint, htab, size);
	}
}
