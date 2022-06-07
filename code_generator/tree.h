#ifndef TREE_H
#define TREE_H

struct bin_tree {
    int lexCode;
    char* lexName;
    int depth;
    int height;
    struct bin_tree* next;
};
typedef struct bin_tree node;

enum branch {left = 0, right, next};

node* curNode;

node* newNode(char* nameLex, int valLex, int depth, int height);
void insert(node** tree, int valLex, char* nameLex, int deepLevel, int height, enum branch direction);
void deltree(node* tree);
void printLeaves(node* tree);

#endif
