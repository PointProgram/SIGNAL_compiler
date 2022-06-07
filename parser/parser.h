#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>

struct errorHandling
{
	short column;
	short row;
	char* lexemCur;
	char* lexemExp;
	struct errorHandling* nxt;
};

void sigProgram(FILE* generatedCode);

#endif