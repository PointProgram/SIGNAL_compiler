#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include "hash.h"
#include "tree.h"
#include "parser.h"

enum ErrorType { illegalChar = 0, endFile, exceptionCase, tokenCrash, cmpErrorF1, cmpErrorF2, wrongWord, endExp};
enum outputType { noneOut = 0, errorOut, infoOut };


void showError(FILE* f, int col, int row, char* record, enum ErrorType error, enum outputType out, char* backReck);
void displayTable(struct nlist** htab, int startPoint, FILE* outGen);
void compareSymbols(char* argvParameter, FILE* expected, FILE* generated, FILE* crashLog);
void displayLexRow(struct tokenStruct** ltab);
void printTree(node* tree, struct errorHandling* erH, FILE* outParse, short depth);

#endif
