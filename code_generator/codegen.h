#ifndef CODEGEN_H
#define CODEGEN_H
#define IDNREPEATSIZE 255

struct idn_repeat {
    char* name;
    int times;
};

struct idn_repeat* rep[IDNREPEATSIZE];
struct idn_repeat* crash[IDNREPEATSIZE];
struct idn_repeat* label[IDNREPEATSIZE];
char* prog_name;

void generator(node* tree, FILE* out_asm);
void cleanR(struct idn_repeat** htab, unsigned int SIZE);
#endif