#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include "hash.h"

enum ErrorType { illegalChar = 0, endFile, exceptionCase, tocenCrash };
enum outputType { noneOut = 0, errorOut, infoOut };

void logF(char* record, enum outputType out);
void showError(FILE* f, int col, int row, char* record, enum ErrorType error, enum outputType out);
void displayTable(struct nlist** htab, int startPoint, FILE* outGen);

#endif
