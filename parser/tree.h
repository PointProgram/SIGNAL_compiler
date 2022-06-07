#ifndef TREE_H
#define TREE_H

struct bin_tree {
    int lexCode;
    char* lexName;
    int depth;
    struct bin_tree* next;
};
typedef struct bin_tree node;

enum branch {left = 0, right, next};

node* root;
node* curNode;

node* newNode(char* nameLex, int valLex);
void insert(node** tree, int valLex, char* nameLex, int deepLevel, enum branch direction);
void deltree(node* tree);

#endif
