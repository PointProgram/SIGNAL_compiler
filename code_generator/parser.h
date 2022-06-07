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

int sigProgram(FILE* generatedCode);
void printtree(node* tree);
char* concat(const char* s1, const char* s2);

#endif