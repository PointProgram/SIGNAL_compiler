#include<stdlib.h>
#include<stdio.h>
#include "lexer.h"
#include "tree.h"

//initializing root node
node* newNode(char* nameLex, int valLex) {
    node* tmp = (node*)malloc(sizeof(node));
    tmp->lexCode = valLex;
    tmp->lexName = nameLex;
    tmp->depth = 0;
    tmp->next = NULL;
    return tmp;
}

//adding data elements to the node
void insert(node** tree, int valLex, char* nameLex, int deepLevel, enum branch direction)
{
    node* temp = NULL;
    if (!(*tree))
    {
        temp = (node*)malloc(sizeof(node));
        temp->next = NULL;
        temp->lexName = nameLex;
        temp->lexCode = valLex;
        temp->depth = deepLevel;
        *tree = temp;
        return;
    }

    if (direction == next)
        insert(&(*tree)->next, valLex, nameLex, deepLevel, direction);
}

//free allocated memory of the tree
void deltree(node* tree) {
    if (tree) {
        deltree(tree->next);
        free(tree);
    }
}

