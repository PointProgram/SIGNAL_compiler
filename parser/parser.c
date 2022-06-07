#include "hash.h"
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "parser.h"
#include "lexer.h"
#include "log.h"

int pos = 0;
int deep = 0;
struct tokenStruct* TS;
enum outputType streamOut = noneOut;
struct errorHandling* errStore;
struct errorHandling* rootStore;

//<program> --> PROGRAM <procedure-identifier> ; <block>. 
void program();
//<block> -- > <constant - declarations> BEGIN <statements - list> END
void block();
//<constant - declarations> -- > CONST <constant - declarations - list> | <empty>
void constDeclLst();
//<constant - declarations - list> -- > <constant - declaration> <constant - declarations - list> | <empty>
void constDecls();
//<constant - declaration> -- > <identifier> = <unsigned - integer>;
int constDecl();
//<statements-list> --> <statement> <statements-list> | <empty>
void stmtList();
//<statement> --> <condition-statement> ENDIF ; | WHILE <conditional-expression> DO <statements-list> ENDWHILE ; 
void statement();
//<condition-statement> --> <incomplete-condition-statement><alternative-part>
void condStatement();
//<incomplete-condition-statement> --> IF <conditional-expression> THEN <statements-list>
void incmpltCondStatement();
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
void identifier(char* lex);
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

void sigProgram(FILE * generatedCode){
	short check = checkEmpty(lexRow, LEXSIZE);
	if(!check)
		return;
	pos = 0;
	TS = SCN(lexRow, LEXSIZE);
	root = newNode("signal-program", -1);
	errStore = newSt(0, 0, "init", "init");
	rootStore = errStore;
	curNode = root;
	deep++;
	program();
	deep--;
	printTree(root, rootStore, generatedCode, 0);
}

void program() {
	insert(&curNode, -1, "program", deep, next);
	deep++;
	struct nlist* tableVal = lookForName("PROGRAM", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		insert(&curNode, -2, "error", deep, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "PROGRAM");
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, next);
		TS = SCN(lexRow, LEXSIZE);
		procIdentifier(TS->lexem);
	}
	char divC = ';';
	TS = SCN(lexRow, LEXSIZE);
	if (TS->key != divC){
		insert(&curNode, -2, "error", deep, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, ";");
	}
	else {
		insert(&curNode, divC, ";", deep, next);
		TS = SCN(lexRow, LEXSIZE);
		block();
	}
	TS = SCN(lexRow, LEXSIZE);
	divC = '.';
	if(TS->key != divC) {
		insert(&curNode, -2, "error", deep, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, ".");
	}
	else {
		insert(&curNode, divC, ".", deep, next);
		struct tokenStruct* lexVal;
		TS = SCN(lexRow, LEXSIZE);
		if (TS != NULL) {
			insert(&curNode, -2, "error", deep, next);
			errPush(&errStore, TS->col, TS->row, TS->lexem, "EOF");
		}
	}
	deep--;
}

void block() {
	insert(&curNode, -1, "block", deep, next);
	deep++;
	constDecls();
	struct nlist* tableVal = lookForName("BEGIN", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		insert(&curNode, -2, "error", deep, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "BEGIN");
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, next);
		TS = SCN(lexRow, LEXSIZE);
		stmtList();
	}
	tableVal = lookForName("END", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		insert(&curNode, -2, "error", deep, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "END");
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, next);
	}
	deep--;
}


void constDecls() {
	insert(&curNode, -1, "constant-declarations", deep, next);
	deep++;
	struct nlist* tableVal = lookForName("CONST", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		insert(&curNode, -1, "empty", deep, next);
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, next);
		TS = SCN(lexRow, LEXSIZE);
		constDeclLst();
	}
	deep--;
}

void constDeclLst() {
	insert(&curNode, -1, "constant-declarations-list", deep, next);
	deep++;
	int check = constDecl();
	
	if(!check)
		constDeclLst();
	else
		insert(&curNode, -1, "empty", deep, next);
	deep--;
}

int constDecl() {
	insert(&curNode, -1, "constant-declaration", deep, next);
	deep++;
	int constIf = 0;

	identifier(TS->lexem);
	TS = SCN(lexRow, LEXSIZE);
	char divC = '=';
	if (TS->key != divC) {
		insert(&curNode, -2, "error", deep, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, ";");
	}
	else {
		insert(&curNode, divC, "=", deep, next);
		TS = SCN(lexRow, LEXSIZE);
	}
	unsgnInteger(TS->lexem);
	TS = SCN(lexRow, LEXSIZE);
	divC = ';';
	if (TS->key != divC) {
		insert(&curNode, -2, "error", deep, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, ";");
	}
	else {
		insert(&curNode, divC, ";", deep, next);
		TS = SCN(lexRow, LEXSIZE);
	}
	struct nlist* tableVal = lookForName("BEGIN", keytab, DIVKEYSIZE);
	deep--;
	if (TS->key == tableVal->defn)
		return 1;
	return 0;
}

void stmtList() {
	insert(&curNode, -1, "statement-list", deep, next);
	deep++;
	struct nlist* tableVal = lookForName("WHILE", keytab, DIVKEYSIZE);
	if (TS->key == tableVal->defn) {
		statement();
		stmtList();
	}
	else {
		tableVal = lookForName("IF", keytab, DIVKEYSIZE);
		if (TS->key == tableVal->defn) {
			statement();
			stmtList();
		}
		else {
			insert(&curNode, -1, "empty", deep, next);
		}
	}
	deep--;
}

void statement() {
	insert(&curNode, -1, "statement", deep, next);
	deep++;
	struct nlist* tableVal = lookForName("WHILE", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		condStatement();
		tableVal = lookForName("ENDIF", keytab, DIVKEYSIZE);
		if (TS->key != tableVal->defn) {
			insert(&curNode, -2, "error", deep, next);
			errPush(&errStore, TS->col, TS->row, TS->lexem, "ENDIF");
		}
		else {
			insert(&curNode, tableVal->defn, tableVal->name, deep, next);
			TS = SCN(lexRow, LEXSIZE);
			char divC = ';';
			if (TS->key != divC) {
				insert(&curNode, -2, "error", deep, next);
				errPush(&errStore, TS->col, TS->row, TS->lexem, ";");
			}
			else {
				insert(&curNode, divC, ";", deep, next);
				TS = SCN(lexRow, LEXSIZE);
			}
		}
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, next);
		condExpression();
		tableVal = lookForName("DO", keytab, DIVKEYSIZE);
		TS = SCN(lexRow, LEXSIZE);
		if (TS->key != tableVal->defn) {
			insert(&curNode, -2, "error", deep, next);
			errPush(&errStore, TS->col, TS->row, TS->lexem, "DO");
		}
		else {
			insert(&curNode, tableVal->defn, tableVal->name, deep, next);
			TS = SCN(lexRow, LEXSIZE);
			stmtList();
			tableVal = lookForName("ENDWHILE", keytab, DIVKEYSIZE);
			if (TS->key != tableVal->defn) {
				insert(&curNode, -2, "error", deep, next);
				errPush(&errStore, TS->col, TS->row, TS->lexem, "DO");
			}
			else {
				insert(&curNode, tableVal->defn, tableVal->name, deep, next);
				TS = SCN(lexRow, LEXSIZE);
				char divC = ';';
				if (TS->key != divC) {
					insert(&curNode, -2, "error", deep, next);
					errPush(&errStore, TS->col, TS->row, TS->lexem, ";");
				}
				else {
					insert(&curNode, divC, ";", deep, next);
					TS = SCN(lexRow, LEXSIZE);
				}
			}
		}
	}
	deep--;
}

void condStatement() {
	insert(&curNode, -1, "condition-statement", deep, next);
	deep++;
	incmpltCondStatement();
	altPart();
	deep--;
}

void incmpltCondStatement() {
	insert(&curNode, -1, "incomplete-condition-statement", deep, next);
	deep++;
	struct nlist* tableVal = lookForName("IF", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		insert(&curNode, -2, "error", deep, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "IF");
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, next);
		condExpression();
		TS = SCN(lexRow, LEXSIZE);
		tableVal = lookForName("THEN", keytab, DIVKEYSIZE);
		if (TS->key != tableVal->defn) {
			insert(&curNode, -2, "error", deep, next);
			errPush(&errStore, TS->col, TS->row, TS->lexem, "THEN");
		}
		else {
			insert(&curNode, tableVal->defn, tableVal->name, deep, next);
			TS = SCN(lexRow, LEXSIZE);
			stmtList();
		}
	}
	deep--;
}

void altPart() {
	insert(&curNode, -1, "alternative-part", deep, next);
	deep++;
	struct nlist* tableVal = lookForName("ELSE", keytab, DIVKEYSIZE);
	if (TS->key != tableVal->defn) {
		insert(&curNode, -1, "empty", deep, next);
	}
	else {
		insert(&curNode, tableVal->defn, tableVal->name, deep, next);
		TS = SCN(lexRow, LEXSIZE);
		stmtList();
	}
	deep--;
}

void condExpression() {
	insert(&curNode, -1, "conditional-expression", deep, next);
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
	insert(&curNode, -1, "comparassion operator", deep, next);
	deep++;
	struct nlist* tableVal = lookForName("<=", divtab, DIVKEYSIZE);
	int divCode = tableVal->defn;
	if (TS->key == divCode) {
		insert(&curNode, tableVal->defn, tableVal->name, deep, next);
	}
	else {
		tableVal = lookForName("<>", divtab, DIVKEYSIZE);
		divCode = tableVal->defn;
		if (TS->key == divCode) {
			insert(&curNode, tableVal->defn, tableVal->name, deep, next);
		}
		else {
			tableVal = lookForName(">=", divtab, DIVKEYSIZE);
			divCode = tableVal->defn;
			if (TS->key == divCode) {
				insert(&curNode, tableVal->defn, tableVal->name, deep, next);	
			}
			else {
				divCode = '<';
				if(TS->key == divCode) {
					insert(&curNode, divCode, "<", deep, next);
				}
				else {
					divCode = '=';
					if (TS->key == divCode) {
						insert(&curNode, divCode, "=", deep, next);
					}
					else {
						divCode = '>';
						if (TS->key == divCode) {
							insert(&curNode, divCode, ">", deep, next);
						}
						else {
							insert(&curNode, -2, "error", deep, next);
							errPush(&errStore, TS->col, TS->row, TS->lexem, "<|<=|=|<>|>=|>");
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
	insert(&curNode, -1, "expression", deep, next);
	deep++;
	if (lex[0] >= '0' && lex[0] <= '9') {
		unsgnInteger(lex);
	}
	else if (lex[0] >= 'A' && lex[0] <= 'Z') {	
		varIdentifier(lex);
	}
	else {
		insert(&curNode, -2, "error", deep, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "illegal-start-symbol");
	}
	deep--;
}

void varIdentifier(char *lex) {
	insert(&curNode, -1, "variable-identifier", deep, next);
	deep++;
	identifier(lex);
	deep--;
}

void procIdentifier(char* lex) {
	insert(&curNode, -1, "procedure-identifier", deep, next);
	deep++;
	identifier(lex);
	deep--;
}

void identifier(char *lex) {
	insert(&curNode, -1, "identifier", deep, next);
	deep++;
	int lexPos = 0;
	if (letter(lex[lexPos])) {
		lexPos++;
		string(lex, lexPos);
	}
	else {
		insert(&curNode, -2, "error", deep, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "identifier");
	}
	deep--;
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
			insert(&curNode, -2, "error", deep, next);
			errPush(&errStore, TS->col, TS->row, TS->lexem, "string");
		}
		else
			insert(&curNode, tableVal->defn, tableVal->name, deep, next);
	}
}

void unsgnInteger(char* lex) {
	insert(&curNode, -1, "unsigned-integer", deep, next);
	deep++;
	int lexPos = 0;
	if (digit(lex[lexPos])) {
		lexPos++;
		digString(lex, lexPos);
	}
	else {
		insert(&curNode, -2, "error", deep, next);
		errPush(&errStore, TS->col, TS->row, TS->lexem, "unsigned-integer");
	}
	deep--;
}

void digString(char* lex, int lexInd) {
	if (!digit(lex[0])) {
		insert(&curNode, -1, "empty", deep, next);
	}
	else if (digit(lex[lexInd])) {
		digString(lex, ++lexInd);
	}
	else if (lex[lexInd] == '\0') {
		struct nlist* tableVal = lookForName(lex, consttab, CONSTIDNSIZE);
		insert(&curNode, tableVal->defn, tableVal->name, deep, next);
	}
	else {
		insert(&curNode, -2, "error", deep, next);
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
