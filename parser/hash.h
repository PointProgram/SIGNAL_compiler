#ifndef HASH_H
#define HASH_H
#include <stdlib.h>
struct nlist { 
    char* name; 
    int defn;
};

struct tokenStruct {
    int col;
    int row;
    int key;
    char* lexem;
};
#define DIVKEYSIZE 99
#define CONSTIDNSIZE 499
#define IDNSIZE 9999
#define TOKENSIZE 99
#define LEXSIZE 9999
#define ONEDIVSIZE 255


char SymbolCategories[ONEDIVSIZE];
struct nlist* consttab[CONSTIDNSIZE];
struct nlist* keytab[DIVKEYSIZE];
struct nlist* idntab[IDNSIZE];
struct nlist* divtab[DIVKEYSIZE];
struct nlist* tokentab[TOKENSIZE];
struct tokenStruct* lexRow[LEXSIZE];

void initHashTab(struct nlist** htab, int SIZE);
struct nlist* lookForName(char* name, struct nlist** htab, unsigned int SIZE);
struct nlist* lookForCode(int code, struct nlist** htab, unsigned int SIZE);
void pushBack(char* name, int defn, struct nlist** htab, unsigned int SIZE);
void pushLexRow(int colPush, int rowPush, int keyPush, char* lexemPush, struct tokenStruct** ltab, unsigned int SIZE);
int getByName(char* name, struct nlist** htab, unsigned int SIZE);
short showLastLexCode(struct nlist** htab, short typeTab, unsigned int SIZE);
void cleanAlloc(struct nlist** htab, unsigned int SIZE);
void initLexRow(struct tokenStruct** ltab, int SIZE);
void cleanLex(struct tokenStruct** ltab, unsigned int SIZE);
short checkEmpty(struct tokenStruct** ltab, unsigned int SIZE);

#endif // !HASH_H

