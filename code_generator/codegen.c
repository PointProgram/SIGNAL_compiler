#include "tree.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hash.h"
#include "log.h"
#include "parser.h"
#include "codegen.h"
#pragma warning (disable:4996)

//free the memory of allocated arrays
void cleanR(struct idn_repeat** htab, unsigned int SIZE) {
	for (int i = 0;i < SIZE;i++) {
		if (htab[i] != NULL) {
			free(htab[i]->name);
			free(htab[i]);
			htab[i] = NULL;
		}
	}
}

//convert integer number to dynamic char array
char* convertIntegerToChar(int N) {
	int m = N;
	int digit = 0;
	while (m) {
		digit++;
		m /= 10;
	}
	char* arr, *arr1;
	arr1 = (char*)malloc(digit * sizeof(char) + 1);
	arr = (char*)malloc(digit);
	int index = 0;
	while (N) {
		arr1[++index] = N % 10 + '0';
		N /= 10;
	}
	int i;
	for (i = 0; i < index; i++) {
		arr[i] = arr1[index - i];
	}
	arr[i] = '\0';
	return (char*)arr;
}

//generate unique label name
struct idn_repeat* generate_lab() {
	int i = 0, last_num;
	char* buf = "?L", *bf;
	if (label[0] == NULL) {
		last_num = 1;
	}
	while (label[i] != NULL) {
		last_num = label[i]->times + 1;
		i++;
	}
	bf = convertIntegerToChar(last_num);
	buf = concat(buf, bf);
	struct idn_repeat* rp = (struct idn_repeat*)malloc(sizeof(struct idn_repeat));
	rp->name = strDup(buf);
	rp->times = last_num;
	label[i] = rp;
	return rp;
}

//add new element to the dtruct idn_repeat array
int push_(char* name, struct idn_repeat** rep_, unsigned int SIZE) {
	struct idn_repeat* np = (struct idn_repeat*)malloc(sizeof(struct idn_repeat));
	int sz = 0;
	while (rep_[sz] != NULL && sz < SIZE) {
		if (strcmp(rep_[sz]->name, name) == 0) {
			rep_[sz]->times += 1;
			return sz;
		}
		sz++;
	}
	np->name = strDup(name);
	np->times = 1;
	rep_[sz] = np;
	return -1;
}

//find sequence of neibourhour leaves
int* form_(node* tree, int depth) {
	node* leaves = tree;
	int* out = (int*)malloc(sizeof(int)), a = 2;
	while (leaves->depth >= depth) {
		if ((leaves->depth == depth) && (strstr(leaves->lexName, "<"))) {
			out = realloc(out, a * sizeof(int));
			out[a - 2] = leaves->height;
			a++;
		}
		leaves = leaves->next;
	}
	return out;
}

//get required tree node
node get_leaves(node* tree, int height) {
	while (tree->height != height) {
		tree = tree->next;
	}
	return tree->next[0];
}

//find appropriate instance of lexem
struct tokenStruct* search(struct tokenStruct** typeTable, int tableSize, int times, char* word) {
	int i = 0, cnt = 1;
	while (typeTable[i] != NULL) {
		if (strcmp(typeTable[i++]->lexem, word) == 0) {
			if (cnt == times)
				return typeTable[i-1];
			else
				cnt++;
		}
	}
	return typeTable[i];
}

//check if varible is allowed to use in program
int find_variables(FILE* out_asm, char* variable, short type) {
	int i = 0;
	//struct nlist* consttab[CONSTIDNSIZE];
	//struct nlist* idntab[IDNSIZE];
	while (rep[i] != NULL) {
		if (strcmp(rep[i]->name, variable) == 0) {
			if (type == 1) {
				if (lookForName(variable, consttab, CONSTIDNSIZE) != NULL) {
					return 1;
				}
			}
			else if (type == 2) {
				if (lookForName(variable, idntab, IDNSIZE) != NULL) {
					return 1;
				}
			}
		}
		i++;
	}
	if (lookForName(variable, consttab, CONSTIDNSIZE) == NULL || type == 2) {
		int push_val = push_(variable, crash, IDNREPEATSIZE);
		if (push_val != -1) {
			struct tokenStruct* upd = search(lexRow, LEXSIZE, crash[push_val]->times, crash[push_val]->name);
			if(type == 1)
				showError(out_asm, upd->col, upd->row, variable, onlyNum, noneOut, NULL);
			else if (type == 2)
				showError(out_asm, upd->col, upd->row, variable, onlyIdn, noneOut, NULL);
		}
		else {
			int j = 0;
			while (crash[j] != NULL)
				j++;
			struct tokenStruct* upd = search(lexRow, LEXSIZE, crash[j - 1]->times, crash[j - 1]->name);
			if (type == 1)
				showError(out_asm, upd->col, upd->row, variable, onlyNum, noneOut, NULL);
			else if (type == 2)
				showError(out_asm, upd->col, upd->row, variable, onlyIdn, noneOut, NULL);
		}
	}
	else 
		return 1;


	return 0;
}

//display lexem row 
char* get_row(int* beg_key, char* compare) {
	char* buffer = ";";
	while (strcmp(lexRow[(*beg_key)]->lexem, compare) != 0) {
		buffer = concat(buffer, lexRow[*beg_key]->lexem);
		buffer = concat(buffer, " ");
		(*beg_key)++;
	}
	buffer = concat(buffer, lexRow[*beg_key]->lexem);
	buffer = concat(buffer, " ");
	(*beg_key)++;
	return buffer;
}

//check multiple variable initialization and use
int check_repeat(FILE* out_asm, char * var_check, short const_) {
	short check = 0;
	if (const_) {
		int i = 0;
		while (rep[i] != NULL) {
			if (strcmp(rep[i]->name, var_check) == 0) {
				check = 1;
				break;
			}
			i++;
		}
	}
	if (strcmp(prog_name, var_check) == 0 || check) {
		int push_val = push_(var_check, crash, IDNREPEATSIZE);
		if (push_val != -1) {
			struct tokenStruct* upd = search(lexRow, LEXSIZE, crash[push_val]->times, crash[push_val]->name);
			showError(out_asm, upd->col, upd->row, var_check, repWord, noneOut, NULL);
		}
		else {
			int j = 0;
			while (crash[j] != NULL)
				j++;
			struct tokenStruct* upd = search(lexRow, LEXSIZE, crash[j-1]->times, crash[j-1]->name);
			showError(out_asm, upd->col, upd->row, var_check, repWord, noneOut, NULL);
		}
		return 0;
	}
	return 1;
}

//initialization of the constant values
int get_constant(node* tree, int height, FILE* out_asm, int *bg) {
	int* ot;
	node one_node, other_node;
	if ((strstr(tree->lexName, "<constant-declaration>")) && (!strstr(tree->next->lexName, "<empty>"))) {
		fprintf(out_asm, "%s\n", get_row(&(*bg), ";"));
		ot = form_(tree->next, tree->next->depth);
		one_node = get_leaves(tree->next, ot[1]);
		other_node = get_leaves(tree->next, ot[0]);
			
		int x = atoi(one_node.lexName);
		char* buf = NULL;
		if (x <= 255)
			buf = "DB";
		else if (x <= 65535)
			buf = "DW";
		else if (x <= 4294967295)
			buf = "DD";
		int push_val = push_(other_node.lexName, rep, IDNREPEATSIZE);
		if (push_val != -1) {
			struct tokenStruct* upd = search(lexRow, LEXSIZE, rep[push_val]->times, rep[push_val]->name);
			showError(out_asm, upd->col, upd->row, other_node.lexName, repVar, noneOut, NULL);
		}
		else {
			if (check_repeat(out_asm, other_node.lexName, 0))
				fprintf(out_asm, "\t%s %s %s\n", other_node.lexName, buf, one_node.lexName);
		}
		return 1;
	}
	return 0;
}

//condition assembler code forming
node* cond_exp(node* tree, int height, FILE* out_asm, char* _label_, short type_) {
	int *ot;
	int depth = tree->next->depth;
	ot = form_(tree->next, tree->next->depth);
	node one_node = get_leaves(tree->next, ot[0]);

	if (strcmp(one_node.lexName, "<unsigned-integer>") == 0 || strcmp(one_node.lexName, "<variable-identifier>") == 0) {
		while (strstr(tree->lexName, "<") || tree->height < ot[0]) {
			tree = tree->next;
		}
		if(find_variables(out_asm, tree->lexName, type_))
			fprintf(out_asm, "\tMOV AX, %s\n", tree->lexName);
	}
	while (tree->height != ot[1] + 1) {
		tree = tree->next;
	}
	char* buf = tree->lexName;
	if (strcmp(buf, "<") == 0)
		buf = "JGE";
	else if (strcmp(buf, "<=") == 0)
		buf = "JG";
	else if (strcmp(buf, "=") == 0)
		buf = "JNE";
	else if (strcmp(buf, "<>") == 0)
		buf = "JE";
	else if (strcmp(buf, ">=") == 0)
		buf = "JL";
	else if (strcmp(buf, ">") == 0)
		buf = "JLE";
	one_node = get_leaves(tree->next, ot[2]);
	if (strcmp(one_node.lexName, "<unsigned-integer>") == 0 || strcmp(one_node.lexName, "<variable-identifier>") == 0) {
		while (strstr(tree->lexName, "<") || tree->height < ot[2]) {
			tree = tree->next;
		}
		if (find_variables(out_asm, tree->lexName, type_))
				fprintf(out_asm, "\tMOV BX, %s\n", tree->lexName);
	}
	fprintf(out_asm, "\tCMP AX, BX\n");
	
	fprintf(out_asm, "\t%s %s\n", buf, _label_);
	return tree->next;
}

//analysing and forming of loop and conditional operators assembler code
void statement_analyse(node** tree, int height, FILE* out_asm) {
	fprintf(out_asm, "\n");
	(*tree) = (*tree)->next;
	if (strcmp((*tree)->next->lexName, "WHILE") == 0) {
		
		struct idn_repeat* generate = generate_lab();
		struct idn_repeat *gen_2 = generate_lab();
		fprintf(out_asm, "%s: NOP\n", generate->name);
		*tree = cond_exp((*tree)->next->next, (*tree)->next->next->depth, out_asm, gen_2->name, 2);
		if (!strstr((*tree)->next->next->lexName,"<empty>"))
			statement_analyse(&(*tree)->next, (*tree)->next->next->depth, out_asm);
		else {
			*tree = (*tree)->next->next;
			fprintf(out_asm, "\tNOP\n");

		}
		fprintf(out_asm, "\tJMP %s\n", generate->name);
		fprintf(out_asm, "%s: NOP\n", gen_2->name);
		while ((*tree)->depth != height)
			*tree = (*tree)->next;
		if (!strstr((*tree)->next->lexName, "<empty>"))
			statement_analyse(&(*tree), (*tree)->next->depth, out_asm);
		else {
			*tree = (*tree)->next->next;
			fprintf(out_asm, "\tNOP\n");
		}
	}
	else if (strcmp((*tree)->next->lexName, "<condition-statement>") == 0) {
		int else_branch = (*tree)->next->next->depth;
		*tree = (*tree)->next->next->next;
		struct idn_repeat* generate = generate_lab();
		*tree = cond_exp((*tree)->next, (*tree)->next->depth, out_asm, generate->name, 1);
		if (!strstr((*tree)->next->next->lexName, "<empty>"))
			statement_analyse(&(*tree)->next, (*tree)->next->next->depth, out_asm);
		else {
			*tree = (*tree)->next->next;
			fprintf(out_asm, "\tNOP\n");
		}
		while ((*tree)->depth != else_branch)
			(*tree) = (*tree)->next;
		
		if (strstr((*tree)->next->lexName, "ELSE")) {
			(*tree) = (*tree)->next->next;
			struct idn_repeat* gen_2 = generate_lab();
			fprintf(out_asm, "\tJMP %s\n", gen_2->name);
			fprintf(out_asm, "%s: NOP\n", generate->name);
			if (!strstr((*tree)->next->lexName, "<empty>"))
				statement_analyse(&(*tree), (*tree)->next->depth, out_asm);
			else {
				*tree = (*tree)->next->next->next;
				fprintf(out_asm, "\tNOP\n");
			}
			fprintf(out_asm, "%s: NOP\n", gen_2->name);
		}
		else {
			*tree = (*tree)->next->next;
			fprintf(out_asm, "%s: NOP\n", generate->name);
		}
		while ((*tree)->depth != height)
			*tree = (*tree)->next;
		if (!strstr((*tree)->next->lexName, "<empty>"))
			statement_analyse(&(*tree), (*tree)->next->depth, out_asm);
		else {
			*tree = (*tree)->next->next;
			fprintf(out_asm, "\tNOP\n");
		}
	}
}

//main generator code program 
void generator(node* tree, FILE* out_asm) {
	fprintf(out_asm, "\n\n");
	node* proc_c = tree;
	if(strcmp(proc_c->next->next->lexName, "PROGRAM") == 0) {
		proc_c = proc_c->next->next->next->next->next;
		fprintf(out_asm, ";%s\n", proc_c->lexName);
		prog_name = proc_c->lexName;
		fprintf(out_asm, "DATA SEGMENT\n");
		if (strcmp(proc_c->next->next->next->next->lexName, "CONST") == 0) {
			fprintf(out_asm, ";CONST\n");
			proc_c = proc_c->next->next->next->next-> next->next;
			int b_k = 4;
			while (get_constant(proc_c, proc_c->height, out_asm, &b_k)) {
				proc_c = proc_c->next->next->next->next->next->next->next->next;
			}
		}
		else {
			proc_c = proc_c->next->next->next->next->next;
			fprintf(out_asm, "\tNOP\n");
		}

	}
	
	fprintf(out_asm, "DATA ENDS\n\n");
	fprintf(out_asm, "CODE SEGMENT\n\tASSUME DS: DATA CS:CODE\n");
	fprintf(out_asm, "START:\n\tMOV AX, DATA\n\tMOV DS, AX\n\tXOR AX, AX\nCALL %s\n\n", prog_name);

	if (strcmp(proc_c->lexName, "BEGIN") == 0) {
		proc_c = proc_c->next;
		if (strcmp(proc_c->lexName, "<statement-list>") == 0) {
			if (!strstr(proc_c->next->lexName, "<empty>"))
				statement_analyse(&proc_c, proc_c->next->depth, out_asm);
			else
				fprintf(out_asm, "NOP\n");
		}
	}
	fprintf(out_asm, "RET\nMOV AX, 4C00H\nINT 21H\n");
	fprintf(out_asm, "CODE ENDS\nEND START");

}