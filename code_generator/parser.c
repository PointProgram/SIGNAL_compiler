#include "hash.h"
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "parser.h"
#include "lexer.h"
#include "log.h"
#pragma warning (disable:4996)
int pos = 0;
int deep = 0, height = 0;
int recursLoop = 0;
struct tokenStruct* TS;
enum outputType streamOut = noneOut;
struct errorHandling* errStore;
struct errorHandling* rootStore;

//<program> --> PROGRAM <procedure-identifier> ; <block>. 
void program();
//<block> -- > <constant - declarations> BEGIN <statements - list> END
void block();
//<constant - declarations> -- > CONST <constant - declarations - list> | <empty>
void constDeclLst(char *lex);
//<constant - declarations - list> -- > <constant - declaration> <constant - declarations - list> | <empty>
void constDecls();
//<constant - declaration> -- > <identifier> = <unsigned - integer>;
int constDecl(char *lex);
//<statements-list> --> <statement> <statements-list> | <empty>
void stmtList(char *message);
//<statement> --> <condition-statement> ENDIF ; | WHILE <conditional-expression> DO <statements-list> ENDWHILE ; 
int statement(char *message);
//<condition-statement> --> <incomplete-condition-statement><alternative-part>
int condStatement();
//<incomplete-condition-statement> --> IF <conditional-expression> THEN <statements-list>
int incmpltCondStatement();
//<alternative-part> --> ELSE<statements-list> | <empty>
void altPart();
//<conditional-expression> --> <expression><comparison-operator> <expression>
void condExpression();
//<comparison-operator> --> < | <= | = | <> | >= | > 
int compOperator();
//<expression> --> <variable-identifier> | <unsigned-integer>
void expression(char* lex);
//<variable-identifier> --> <identifier> 
void varIdentifier(char *lex);
//<procedure-identifier> --> <identifier>
void procIdentifier(char* lex);
//<identifier> --> <letter><string> 
int identifier(char* lex);
//<string> --> <letter><string> | <digit><string> | <empty> 
void string(char* lex, int lexInd);
//<unsigned-integer> --> <digit><digits-string>
void unsgnInteger(char* lex);
//<digits-string> --> <digit><digits-string> | <empty> 
void digString(char* lex, int lexInd);
//<digit> --> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 
int digit(int digChar);
//<letter> --> A | B | C | D | ... | Z 
int letter(int letChar);

char* concat(const char* s1, const char* s2)
{
	const size_t len1 = strlen(s1);
	const size_t len2 = strlen(s2);
	char* result = malloc(len1 + len2 + 1); // +1 for the null-terminator
	// in real code you would check for errors in malloc here
	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
	return result;
}

/*void printtree(node* tree) {
	//printf("\n----------------------------------------------------\n");
	if (tree != NULL) {
		//printf("%-40s %10d %10d\n", tree->lexName, tree->lexCode, tree->depth);
		tree = tree->next;
	}
}*/

node* copyTree(node* tree) {
	node* newTree;
	if (tree == NULL)
		return NULL;
	else {
		newTree = (node*)malloc(sizeof(node));
		newTree->lexName = tree->lexName;
		newTree->lexCode = tree->lexCode;
		newTree->depth = tree->depth;
		newTree->height = tree->height;
		newTree->next = copyTree(tree->next);
		return newTree;
	}
}

void errPush(struct errorHandling** err, short col_, short row_, char* lexCur, char* lexExp)
{
	struct errorHandling* temp = NULL;
	if (!(*err))
	{
		temp = (struct errorHandling*)malloc(sizeof(struct errorHandling));
		temp->nxt = NULL;
		temp->column = col_;
		temp->row = row_;
		temp->lexemCur = lexCur;
		temp->lexemExp = lexExp;
		*err = temp;
		return;
	}

	errPush(&(*err)->nxt, col_, row_, lexCur, lexExp);
}

struct errorHandling* newSt(short col_, short row_, char* lexCur, char* lexExp)
{
	struct errorHandling* tmp = (struct errorHandling*)malloc(sizeof(struct errorHandling));
	tmp->nxt = NULL;
	tmp->column = col_;
	tmp->row = row_;
	tmp->lexemCur = lexCur;
	tmp->lexemExp = lexExp;

	return tmp;
}

struct tokenStruct* SCN(struct tokenStruct** typeTable, int tableSize) {
	if (typeTable[pos] != NULL)
		return typeTable[pos++];
	return NULL;
}

int sigProgram(FILE * generatedCode){
	deep = 0;
	short check = checkEmpty(lexRow, LEXSIZE);
	if(!check)
		return -1;
	pos = 0;
	TS = SCN(lexRow, LEXSIZE);
	curNode = newNode("<signal-program>", -1, 0, height++);
	errStore = newSt(0, 0, "init", "init");
	rootStore = errStore;
	deep++;
	program();
	deep--;
	int pr_int = 0;
	printTree(curNode, rootStore, generatedCode, 0, &pr_int);
	return pr_int;
}

void program() {
	insert(&curNode, -1, "<program>", deep, height++, next);
	deep++;
	struct nlist* tableVal = lookForName("PROGRAM", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		insert(&curNode, -2, "<error>", deep, height++, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "PROGRAM");
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
		TS = SCN(lexRow, LEXSIZE);
		procIdentifier(TS->lexem);
	}
	char divC = ';';
	TS = SCN(lexRow, LEXSIZE);
	if (TS->key != divC){
		insert(&curNode, -2, "<error>", deep, height++, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, ";");
	}
	else {
		insert(&curNode, divC, ";", deep, height++, next);
		TS = SCN(lexRow, LEXSIZE);
		block();
	}
	TS = SCN(lexRow, LEXSIZE);
	divC = '.';
	if(TS->key != divC) {
		insert(&curNode, -2, "<error>", deep, height++, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, ".");
	}
	else {
		insert(&curNode, divC, ".", deep, height++, next);
		struct tokenStruct* lexVal;
		TS = SCN(lexRow, LEXSIZE);
		if (TS != NULL) {
			insert(&curNode, -2, "<error>", deep, height++, next);
			errPush(&errStore, TS->col, TS->row, TS->lexem, "EOF");
		}
	}
	deep--;
}

void block() {
	insert(&curNode, -1, "<block>", deep, height++, next);
	deep++;
	constDecls();
	struct nlist* tableVal = lookForName("BEGIN", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		insert(&curNode, -2, "<error>", deep, height++, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "BEGIN");
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
		TS = SCN(lexRow, LEXSIZE);
		stmtList("");
	}
	tableVal = lookForName("END", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		insert(&curNode, -2, "<error>", deep, height++, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "END");
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
	}
	deep--;
}


void constDecls() {
	insert(&curNode, -1, "<constant-declarations>", deep, height++, next);
	deep++;
	struct nlist* tableVal = lookForName("CONST", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		insert(&curNode, -1, "<empty>", deep, height++, next);
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
		TS = SCN(lexRow, LEXSIZE);
		constDeclLst("<constant-declaration>");
	}
	deep--;
}

void constDeclLst(char *lex) {
	insert(&curNode, -1, "<constant-declarations-list>", deep, height++, next);
	deep++;
	int check = constDecl(lex);
	if (!check) {
		lex = concat(lex, "<constant-declaration>");
		constDeclLst(lex);
	}
	else {
		if (lex == "<constant-declaration>")
			lex = "<empty>";
		else
			lex = concat(lex, "<empty>");
		insert(&curNode, -1, lex, deep, height++, next);
	}
	deep--;
}

int constDecl(char *lex) {
	insert(&curNode, -1, lex, deep, height++, next);
	deep++;
	int constIf = 0;

	int identState = identifier(TS->lexem);
	TS = SCN(lexRow, LEXSIZE);
	char divC = '=';
	if (TS->key != divC) {
		insert(&curNode, -2, "<error>", deep, height++, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, ";");
	}
	else {
		insert(&curNode, divC, "=", deep, height++, next);
		TS = SCN(lexRow, LEXSIZE);
	}
	unsgnInteger(TS->lexem);
	TS = SCN(lexRow, LEXSIZE);
	divC = ';';
	if (TS->key != divC) {
		insert(&curNode, -2, "<error>", deep, height++, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, ";");
	}
	else {
		insert(&curNode, divC, ";", deep, height++, next);
		TS = SCN(lexRow, LEXSIZE);
	}
	struct nlist* tableVal = lookForName("BEGIN", keytab, DIVKEYSIZE);
	deep--;
	if (TS->key == tableVal->defn)
		return 1;
	return 0;
}

void stmtList(char *message) {
	insert(&curNode, -1, "<statement-list>", deep, height++, next);
	deep++;
	node* leafTree = copyTree(curNode);
	int height_ = height;
	int statementState = statement(concat(message, "<statement>"));
	if (statementState == 0) {
		message = concat(message, "<statement>");
		stmtList(message);
	}
	else if (statementState == 1) {
		deltree(curNode);
		curNode = copyTree(leafTree);
		height = height_;
		if (message == "<statement>")
			message = "<empty>";
		else
			message = concat(message, "<empty>");
		insert(&curNode, -1, message, deep, height++, next);
	}
	deep--;
}

int statement(char *message) {
	insert(&curNode, -1, message, deep, height++, next);
	deep++;
	struct nlist* tableVal = lookForName("WHILE", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		int condStatementVar = condStatement();
		if (condStatementVar == 0) {
			deep--;
			return 1;
		}
		tableVal = lookForName("ENDIF", keytab, DIVKEYSIZE);
		if (TS->key != tableVal->defn) {
			insert(&curNode, -2, "<error>", deep, height++, next);
			errPush(&errStore, TS->col, TS->row, TS->lexem, "ENDIF");
			deep--;
			return 2;
		}
		else {
			insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
			TS = SCN(lexRow, LEXSIZE);
			char divC = ';';
			if (TS->key != divC) {
				insert(&curNode, -2, "<error>", deep, height++, next);
				errPush(&errStore, TS->col, TS->row, TS->lexem, ";");
				deep--;
				return 2;
			}
			else {
				insert(&curNode, divC, ";", deep, height++, next);
				TS = SCN(lexRow, LEXSIZE);
			}
		}
		deep--;
		return 0;
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
		condExpression();
		tableVal = lookForName("DO", keytab, DIVKEYSIZE);
		TS = SCN(lexRow, LEXSIZE);
		if (TS->key != tableVal->defn) {
			insert(&curNode, -2, "<error>", deep, height++, next);
			errPush(&errStore, TS->col, TS->row, TS->lexem, "DO");
			deep--;
			return 2;
		}
		else {
			insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
			TS = SCN(lexRow, LEXSIZE);
			stmtList("");
			tableVal = lookForName("ENDWHILE", keytab, DIVKEYSIZE);
			if (TS->key != tableVal->defn) {
				insert(&curNode, -2, "<error>", deep, height++, next);
				errPush(&errStore, TS->col, TS->row, TS->lexem, "DO");
				deep--;
				return 2;
			}
			else {
				insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
				TS = SCN(lexRow, LEXSIZE);
				char divC = ';';
				if (TS->key != divC) {
					insert(&curNode, -2, "<error>", deep, height++, next);
					errPush(&errStore, TS->col, TS->row, TS->lexem, ";");
					deep--;
					return 2;
				}
				else {
					insert(&curNode, divC, ";", deep, height++, next);
					TS = SCN(lexRow, LEXSIZE);
				}
			}
		}
		deep--;
		return 0;
	}
}

int condStatement() {
	insert(&curNode, -1, "<condition-statement>", deep, height++, next);
	deep++;
	int incmpltCondStat = incmpltCondStatement();
	if (incmpltCondStat == 0) {
		deep--;
		return 0;
	}
	altPart();
	deep--;
	return 1;
}

int incmpltCondStatement() {
	insert(&curNode, -1, "<incomplete-condition-statement>", deep, height++, next);
	deep++;
	struct nlist* tableVal = lookForName("IF", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		//insert(&curNode, -2, "<error>", deep, height++, next);
		//errPush(&errStore, TS->col, TS->row, TS->lexem, "IF");
		deep--;
		return 0;
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
		condExpression();
		TS = SCN(lexRow, LEXSIZE);
		tableVal = lookForName("THEN", keytab, DIVKEYSIZE);
		if (TS->key != tableVal->defn) {
			insert(&curNode, -2, "<error>", deep, height++, next);
			errPush(&errStore, TS->col, TS->row, TS->lexem, "THEN");
		}
		else {
			insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
			TS = SCN(lexRow, LEXSIZE);
			stmtList("");
		}
	}
	deep--;
	return 1;
}

void altPart() {
	insert(&curNode, -1, "<alternative-part>", deep, height++, next);
	deep++;
	struct nlist* tableVal = lookForName("ELSE", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		insert(&curNode, -1, "<empty>", deep, height++, next);
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
		TS = SCN(lexRow, LEXSIZE);
		stmtList("");
	}
	deep--;
}

void condExpression() {
	insert(&curNode, -1, "<conditional-expression>", deep, height++, next);
	deep++;
	TS = SCN(lexRow, LEXSIZE);
	expression(TS->lexem);
	TS = SCN(lexRow, LEXSIZE);
	compOperator();
	TS = SCN(lexRow, LEXSIZE);
	expression(TS->lexem);
	deep--;
}

int compOperator() {
	insert(&curNode, -1, "<comparassion operator>", deep, height++, next);
	deep++;
	struct nlist* tableVal = lookForName("<=", divtab, DIVKEYSIZE);
	int divCode = tableVal->defn;
	if (TS->key == divCode) {
		insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
	}
	else {
		tableVal = lookForName("<>", divtab, DIVKEYSIZE);
		divCode = tableVal->defn;
		if (TS->key == divCode) {
			insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
		}
		else {
			tableVal = lookForName(">=", divtab, DIVKEYSIZE);
			divCode = tableVal->defn;
			if (TS->key == divCode) {
				insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);	
			}
			else {
				divCode = '<';
				if(TS->key == divCode) {
					insert(&curNode, divCode, "<", deep, height++, next);
				}
				else {
					divCode = '=';
					if (TS->key == divCode) {
						insert(&curNode, divCode, "=", deep, height++, next);
					}
					else {
						divCode = '>';
						if (TS->key == divCode) {
							insert(&curNode, divCode, ">", deep, height++, next);
						}
						else {
							insert(&curNode, -2, "<error>", deep, height++, next);
							errPush(&errStore, TS->col, TS->row, TS->lexem, "<|<=|=|<>|>=|>");
							deep--;
							return 0;
						}
					}
				}
			}
		}
	}
	deep--;
	return 1;
}

void expression(char* lex) {
	insert(&curNode, -1, "<expression>", deep, height++, next);
	deep++;
	if (lex[0] >= '0' && lex[0] <= '9') {
		unsgnInteger(lex);
	}
	else if (lex[0] >= 'A' && lex[0] <= 'Z') {	
		varIdentifier(lex);
	}
	else {
		insert(&curNode, -2, "<error>", deep, height++, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "illegal-start-symbol");
	}
	deep--;
}

void varIdentifier(char *lex) {
	insert(&curNode, -1, "<variable-identifier>", deep, height++, next);
	deep++;
	identifier(lex);
	deep--;
}

void procIdentifier(char* lex) {
	insert(&curNode, -1, "<procedure-identifier>", deep, height++, next);
	deep++;
	identifier(lex);
	deep--;
}

int identifier(char *lex) {
	insert(&curNode, -1, "<identifier>", deep, height++, next);
	deep++;
	int lexPos = 0;
	if (letter(lex[lexPos])) {
		lexPos++;
		string(lex, lexPos);
	}
	else {
		insert(&curNode, -2, "<error>", deep, height++, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "identifier");
		return 1;
	}
	deep--;
	return 0;
}

void string(char* lex, int lexInd) {
	if (letter(lex[lexInd])) {
		string(lex, ++lexInd);
	}
	else if (digit(lex[lexInd])) {
		string(lex, ++lexInd);
	}
	else {
		struct nlist* tableVal = lookForName(lex, idntab, IDNSIZE);
		if (tableVal == NULL) {
			insert(&curNode, -2, "<error>", deep, height++, next);
			errPush(&errStore, TS->col, TS->row, TS->lexem, "string");
		}
		else
			insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
	}
}

void unsgnInteger(char* lex) {
	insert(&curNode, -1, "<unsigned-integer>", deep, height++, next);
	deep++;
	int lexPos = 0;
	if (digit(lex[lexPos])) {
		lexPos++;
		digString(lex, lexPos);
	}
	else {
		insert(&curNode, -2, "<error>", deep, height++, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "unsigned-integer");
	}
	deep--;
}

void digString(char* lex, int lexInd) {
	if (!digit(lex[0])) {
		insert(&curNode, -1, "<empty>", deep, height++, next);
	}
	else if (digit(lex[lexInd])) {
		digString(lex, ++lexInd);
	}
	else if (lex[lexInd] == '\0') {
		struct nlist* tableVal = lookForName(lex, consttab, CONSTIDNSIZE);
		insert(&curNode, tableVal->defn, tableVal->name, deep, height++, next);
	}
	else {
		insert(&curNode, -2, "<error>", deep, height++, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "digital-string");
	}
}

int digit(int digChar) {
	if (digChar >= '0' && digChar <= '9')
		return 1;
	else
		return 0;
}

int letter(int letChar) {
	if (letChar >= 'A' && letChar <= 'Z')
		return 1;
	else 
		return 0;
}
