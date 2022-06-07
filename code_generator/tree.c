#include<stdlib.h>
#include<stdio.h>
#include "lexer.h"
#include "tree.h"

//initializing root node
node* newNode(char* nameLex, int valLex, int depth, int height) {
    node* tmp = (node*)malloc(sizeof(node));
    tmp->lexCode = valLex;
    tmp->lexName = nameLex;
    tmp->depth = depth;
    tmp->height = height;
    tmp->next = NULL;
    return tmp;
}

//adding data elements to the node
void insert(node** tree, int valLex, char* nameLex, int deepLevel, int height, enum branch direction)
{
    node* temp = NULL;
    if (!(*tree))
    {
        *tree = newNode(nameLex, valLex, deepLevel, height);
        return;
    }

     insert(&(*tree)->next, valLex, nameLex, deepLevel, height, direction);
}

void printLeaves(node* tree) {
    while (tree != NULL) {
        printf("%-40s  %10d %10d %10d\n", tree->lexName, tree->lexCode, tree->depth, tree->height);
        tree = tree->next;
    }
}

//free allocated memory of the tree
void deltree(node* tree) {
    if (tree) {
        deltree(tree->next);
        free(tree);
        tree->next = NULL;
        tree = NULL;
    }
}

