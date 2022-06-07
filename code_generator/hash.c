#include "hash.h"
#include <string.h>
#include <stdlib.h>
#pragma warning (disable:4996)
//initialize tables with NULL
void initHashTab(struct nlist** htab, int SIZE) {
    for (int i = 0;i < SIZE;i++)
        htab[i] = NULL;
}

//initialize table of lexicla row with NULL
void initLexRow(struct tokenStruct** ltab, int SIZE) {
    for (int i = 0;i < SIZE;i++)
        ltab[i] = NULL;
}

//forming of hash value for string strInput
unsigned hash(const char* strInput, unsigned int SIZE) {
    const unsigned char* tmp;
    unsigned hashval = 0;
    for (tmp = (const unsigned char*)strInput; *tmp; tmp++)
        hashval = *tmp + 31u * hashval;
    return hashval % SIZE;
}

//look for name value in hash table
struct nlist* lookForName(char* name, struct nlist** htab, unsigned int SIZE) {
    int hashA = hash(name, SIZE);
    while (htab[hashA] != NULL || htab[hashA] != 0) {
        if (strcmp(name, htab[hashA]->name) == 0)
            return htab[hashA];
        ++hashA;
        hashA %= SIZE;
    }
    return NULL; 
}

//look for code value in hash table
struct nlist* lookForCode(int code, struct nlist** htab, unsigned int SIZE)
{
    for (int j = 0; j < SIZE; j++)
        if (htab[j] != NULL) {
            if (code == htab[j]->defn) {
                return htab[j]; 
            }
        }
    return NULL; 
}

//check if table is empty
short checkEmpty(struct tokenStruct** ltab, unsigned int SIZE)
{
    for (int j = 0; j < SIZE; j++)
        if (ltab[j] != NULL) {
                return 1;
        }
    return 0;
}


//duplicating of string value
char* strDup(char* s) {
    char* p;
    p = (char*)malloc((strlen(s) + 1)*sizeof(char));
    if (p != NULL)
        strcpy(p, s);
    return p;
}

//insert values to the table
void pushBack(char* name, int defn, struct nlist** htab, unsigned int SIZE) {
    struct nlist* np = (struct nlist*)malloc(sizeof(struct nlist));
    int hashval = hash(name, SIZE);
    while (htab[hashval] != NULL) {
        ++hashval;
        hashval %= SIZE;
    }
    np->name = strDup(name);
    np->defn = defn;
    htab[hashval] = np;
}

//insert values to the lexical row 
void pushLexRow(int colPush, int rowPush, int keyPush, char* lexemPush, struct tokenStruct** ltab, unsigned int SIZE) {
    struct tokenStruct* np = (struct tokenStruct*)malloc(sizeof(struct tokenStruct));
    int indVal = 0;
    while (ltab[indVal] != NULL) {
        ++indVal;
    }
    np->col = colPush;
    np->row = rowPush;
    np->key = keyPush;
    np->lexem = strDup(lexemPush);
    ltab[indVal] = np;
}

//get the required lex code by name searching 
int getByName(char* name, struct nlist** htab, unsigned int SIZE) {
    struct nlist* n = lookForName(name, htab, SIZE);
    if (n == NULL)
        return 0;
    else
        return n->defn;
}

//return the position of last element in the table
short showLastLexCode(struct nlist** htab, short typeTab, unsigned int SIZE) {
    int i;
    struct nlist* t;
    short lastCode = 0;
    for (i = 0;i < SIZE;i++) {
        if (htab[i] != NULL) {
            t = htab[i];
            if (t->defn > lastCode) {
                lastCode = t->defn;
            }
        }
    }
    if (lastCode == 0)
        lastCode = typeTab;
    return lastCode;
}

//free the allocated memory of the tables
void cleanAlloc(struct nlist** htab, unsigned int SIZE) {
    for (int i = 0;i < SIZE;i++) {
        if (htab[i] != NULL) {
            free(htab[i]->name);
            free(htab[i]);
            htab[i] = NULL;
        }
    }
}

//free the allocated memory of lexical row
void cleanLex(struct tokenStruct** ltab, unsigned int SIZE) {
    for (int i = 0;i < SIZE;i++) {
        if (ltab[i] != NULL) {
            free(ltab[i]->lexem);
            free(ltab[i]);
            ltab[i] = NULL;
        }
    }
}
