#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>

struct symbol {
	char value;
	short attr;
};

enum lexem { multiDiv = 300, keyWord = 400, constWord = 500, idnWord = 1000, tknWord = 10000};

void Lexer(FILE* readFile, FILE* outputTocen);
void initializeTables();
void destroyTables();
char* add_char(char* str, char s);
void updateRecAttr(int* line, int* column, char fileElem);

#endif



#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>

struct symbol {
	char value;
	short attr;
};

enum lexem { multiDiv = 300, keyWord = 400, constWord = 500, idnWord = 1000, tknWord = 10000};

void Lexer(FILE* readFile, FILE* outputTocen);
void initializeTables();
void destroyTables();
char* add_char(char* str, char s);
void updateRecAttr(int* line, int* column, char fileElem);

#endif



