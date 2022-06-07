#ifndef HASH_H
#define HASH_H

struct nlist { 
    char* name; 
    int defn;
};

#define DIVKEYSIZE 99
#define CONSTIDNSIZE 499
#define IDNSIZE 9999
#define TOCENSIZE 99


char SymbolCategories[255];
struct nlist* consttab[CONSTIDNSIZE];
struct nlist* keytab[DIVKEYSIZE];
struct nlist* idntab[IDNSIZE];
struct nlist* divtab[DIVKEYSIZE];
struct nlist* tocentab[TOCENSIZE];


void initHashTab(struct nlist** htab, int SIZE);
struct nlist* lookForName(char* name, struct nlist** htab, unsigned int SIZE);
struct nlist* lookForCode(int code, struct nlist** htab, unsigned int SIZE);
void pushBack(char* name, int defn, struct nlist** htab, unsigned int SIZE);
int getByName(char* name, struct nlist** htab, unsigned int SIZE);
short showLastLexCode(struct nlist** htab, short typeTab, unsigned int SIZE);
void cleanAlloc(struct nlist** htab, unsigned int SIZE);

#endif // !HASH_H

