#include "lexer.h"
#include "log.h"
#include "tree.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#pragma warning (disable:4996)
int lexSize = 999;

//function for forming a strings
char* add_char(char* str, char s) {
	size_t len;
	if (str == NULL)
		len = 0;
	else
		len = strlen(str);
	char* str2 = malloc(len + 1 + 1);
	if (str != NULL)
		strcpy(str2, str);
	str2[len] = s;
	str2[len + 1] = '\0';
	return str2;
}

//position update
void updateRecAttr(int* line, int* column, char fileElem) {
	if (fileElem == '\n') {
		*column = 1;
		*line += 1;
	}
	else {
		*column += 1;
	}
}

//main lexical analyzator program
void Lexer(FILE* readFile, FILE* outputToken) {
	char nextCharachter = fgetc(readFile);
	struct symbol sumb;
	char* buf;
	int lexCode;
	bool SupressOutput = false;
	enum outputType streamOutput = noneOut;
	int col = 1, row = 1, beg_col;
	do {
		sumb.value = nextCharachter;
		sumb.attr = SymbolCategories[(int)nextCharachter];
		buf = NULL;
		lexCode = 0;
		SupressOutput = false;
		//whitespace processing 
		if (sumb.attr == 0) {
			while (nextCharachter != EOF) {
				updateRecAttr(&row, &col, nextCharachter);
				nextCharachter = fgetc(readFile);
				sumb.value = nextCharachter;
				sumb.attr = SymbolCategories[(int)nextCharachter];
				if (sumb.attr > 0 || sumb.attr < 0)
					break;
			}
			SupressOutput = true;
		}
		//digit constants processing
		else if (sumb.attr == 1) {
			beg_col = col;
			while (nextCharachter != EOF && sumb.attr == 1) {
				buf = add_char(buf, sumb.value);
				updateRecAttr(&row, &col, nextCharachter);
				nextCharachter = fgetc(readFile);
				sumb.value = nextCharachter;
				sumb.attr = SymbolCategories[(int)nextCharachter];
			}

			int codeOfLexTab = getByName(buf, consttab, CONSTIDNSIZE);
			if (codeOfLexTab != 0) {
				lexCode = codeOfLexTab;
			}
			else {
				lexCode = showLastLexCode(consttab, constWord, CONSTIDNSIZE) + 1;
				pushBack(buf, lexCode, consttab, CONSTIDNSIZE);
			}
		}
		//identificators and keyword processing
		else if (sumb.attr == 2) {
			beg_col = col;
			while ((nextCharachter != EOF) && (sumb.attr == 2 || sumb.attr == 1)) {
				buf = add_char(buf, sumb.value);
				updateRecAttr(&row, &col, nextCharachter);
				nextCharachter = fgetc(readFile);
				sumb.value = nextCharachter;
				sumb.attr = SymbolCategories[(int)nextCharachter];
			}
			int codeOfLexTab = getByName(buf, keytab, DIVKEYSIZE);
			if (codeOfLexTab != 0) {
				lexCode = codeOfLexTab;
			}
			else {
				codeOfLexTab = getByName(buf, idntab, IDNSIZE);
				if (codeOfLexTab != 0) {
					lexCode = codeOfLexTab;
				}
				else {
					lexCode = showLastLexCode(idntab, idnWord, IDNSIZE) + 1;
					pushBack(buf, lexCode, idntab, IDNSIZE);
				}
			}
		}
		//one-symbol dividers processing
		else if (sumb.attr == 3) {
			beg_col = col;
			lexCode = sumb.value;
			buf = add_char(buf, sumb.value);
			updateRecAttr(&row, &col, nextCharachter);
			nextCharachter = fgetc(readFile);
			sumb.value = nextCharachter;
			sumb.attr = SymbolCategories[(int)nextCharachter];
		}
		//processing of multi-symbolic dividers <= and <>
		else if (sumb.attr == 41) {
			beg_col = col;
			buf = add_char(buf, sumb.value);
			char previousVal = sumb.value;
			updateRecAttr(&row, &col, nextCharachter);
			nextCharachter = fgetc(readFile);
			sumb.value = nextCharachter;
			sumb.attr = SymbolCategories[(int)nextCharachter];
			if (nextCharachter == '=') {
				buf = add_char(buf, sumb.value);
				lexCode = getByName(buf, divtab, DIVKEYSIZE);
				pushBack(buf, lexCode, divtab, DIVKEYSIZE);
				updateRecAttr(&row, &col, nextCharachter);
				nextCharachter = fgetc(readFile);
				sumb.value = nextCharachter;
				sumb.attr = SymbolCategories[(int)nextCharachter];
			}
			else if (nextCharachter == '>') {
				buf = add_char(buf, sumb.value);
				lexCode = getByName(buf, divtab, DIVKEYSIZE);
				pushBack(buf, lexCode, divtab, DIVKEYSIZE);
				updateRecAttr(&row, &col, nextCharachter);
				nextCharachter = fgetc(readFile);
				sumb.value = nextCharachter;
				sumb.attr = SymbolCategories[(int)nextCharachter];
			}
			else {
				lexCode = previousVal;
			}
		}
		//processing of multi-symbolic divider >=
		else if (sumb.attr == 42) {
			beg_col = col;
			buf = add_char(buf, sumb.value);
			char previousVal = sumb.value;
			updateRecAttr(&row, &col, nextCharachter);
			nextCharachter = fgetc(readFile);
			sumb.value = nextCharachter;
			sumb.attr = SymbolCategories[(int)nextCharachter];
			if (nextCharachter == '=') {
				buf = add_char(buf, sumb.value);
				updateRecAttr(&row, &col, nextCharachter);
				nextCharachter = fgetc(readFile);
				lexCode = getByName(buf, divtab, DIVKEYSIZE);
				pushBack(buf, lexCode, divtab, DIVKEYSIZE);
				sumb.value = nextCharachter;
				sumb.attr = SymbolCategories[(int)nextCharachter];
			}
			else {
				lexCode = previousVal;
			}
		}
		//processing of multi-symbolic comment divider (*
		else if (sumb.attr == 5) {
			if (nextCharachter == EOF) {
				lexCode = nextCharachter;
			}
			else {
				short int tmp_row = row, tmp_col = col;
				char nextCh = nextCharachter;
				updateRecAttr(&row, &col, nextCharachter);
				nextCharachter = fgetc(readFile);
				sumb.value = nextCharachter;
				sumb.attr = SymbolCategories[(int)nextCharachter];
				bool ifError = false;
				if (sumb.value == '*') {
					updateRecAttr(&row, &col, nextCharachter);
					nextCharachter = fgetc(readFile);
					if (nextCharachter == EOF) {
						showError(outputToken, col, row, "*)", endFile, streamOutput, NULL);
						SupressOutput = true;
					}
					else {
						sumb.value = nextCharachter;
						sumb.attr = SymbolCategories[(int)nextCharachter];
						bool symbolTriger = true;
						char tmp_value;
						do {
							tmp_value = sumb.value;
							while (nextCharachter != EOF && !((sumb.value != '*') ^ symbolTriger)) {
								updateRecAttr(&row, &col, nextCharachter);
								nextCharachter = fgetc(readFile);
								sumb.value = nextCharachter;
								sumb.attr = SymbolCategories[(int)nextCharachter];
							}
							if (nextCharachter == EOF) {
								showError(outputToken, col, row, "*)", endFile, streamOutput, NULL);
								ifError = true;
								SupressOutput = true;
								break;
							}
							else { // ���� '*' � ���� ���� �����
								if (symbolTriger)
									symbolTriger = false;
								else
									symbolTriger = true;
								if (tmp_value == '*' && sumb.value == ')' && symbolTriger)
									break;
								updateRecAttr(&row, &col, nextCharachter);
								nextCharachter = fgetc(readFile);
								sumb.value = nextCharachter;
								sumb.attr = SymbolCategories[(int)nextCharachter];
							}
						} while (sumb.value != ')' || symbolTriger);
						if (sumb.value == ')') {
							SupressOutput = true;
						}
						if (nextCharachter != EOF && !ifError) {
							updateRecAttr(&row, &col, nextCharachter);
							nextCharachter = fgetc(readFile);
							sumb.value = nextCharachter;
							sumb.attr = SymbolCategories[(int)nextCharachter];
						}
					}
				}
				else {
					buf = add_char(buf, nextCh);
					showError(outputToken, tmp_col, tmp_row, buf, illegalChar, streamOutput, NULL);
					lexCode = nextCharachter;
					SupressOutput = true;
				}
			}
		}
		//processing of forbidden symbols
		else if (sumb.attr == 6) {
			buf = add_char(buf, nextCharachter);
			showError(outputToken, col, row, buf, illegalChar, streamOutput, NULL);
			updateRecAttr(&row, &col, nextCharachter);
			nextCharachter = fgetc(readFile);
			sumb.value = nextCharachter;
			sumb.attr = SymbolCategories[(int)nextCharachter];
			SupressOutput = true;
		}
		else if (sumb.attr == 7) {
			beg_col = col;
			buf = add_char(buf, sumb.value);
			char previousVal = sumb.value;
			updateRecAttr(&row, &col, nextCharachter);
			nextCharachter = fgetc(readFile);
			sumb.value = nextCharachter;
			sumb.attr = SymbolCategories[(int)nextCharachter];
			if (nextCharachter >= '0' && nextCharachter <= '9') {
				while (nextCharachter >= '0' && nextCharachter <= '9') {
					buf = add_char(buf, sumb.value);
					updateRecAttr(&row, &col, nextCharachter);
					nextCharachter = fgetc(readFile);
					sumb.value = nextCharachter;
					sumb.attr = SymbolCategories[(int)nextCharachter];
				}
				if (nextCharachter == '-') {
					buf = add_char(buf, sumb.value);
					updateRecAttr(&row, &col, nextCharachter);
					nextCharachter = fgetc(readFile);
					sumb.value = nextCharachter;
					sumb.attr = SymbolCategories[(int)nextCharachter];
					if (nextCharachter >= '0' && nextCharachter <= '9') {
						while (nextCharachter >= '0' && nextCharachter <= '9') {
							buf = add_char(buf, sumb.value);
							updateRecAttr(&row, &col, nextCharachter);
							nextCharachter = fgetc(readFile);
							sumb.value = nextCharachter;
							sumb.attr = SymbolCategories[(int)nextCharachter];
						}
						if (nextCharachter == '-') {
							buf = add_char(buf, sumb.value);
							updateRecAttr(&row, &col, nextCharachter);
							nextCharachter = fgetc(readFile);
							sumb.value = nextCharachter;
							sumb.attr = SymbolCategories[(int)nextCharachter];
							if (nextCharachter >= '0' && nextCharachter <= '9') {
								while (nextCharachter >= '0' && nextCharachter <= '9') {
									buf = add_char(buf, sumb.value);
									updateRecAttr(&row, &col, nextCharachter);
									nextCharachter = fgetc(readFile);
									sumb.value = nextCharachter;
									sumb.attr = SymbolCategories[(int)nextCharachter];
								}
							}
							int codeOfLexTab = getByName(buf, tokentab, TOKENSIZE);
							if (codeOfLexTab != 0) {
								lexCode = codeOfLexTab;
							}
							else {
								lexCode = showLastLexCode(tokentab, tknWord, TOKENSIZE) + 1;
								pushBack(buf, lexCode, tokentab, TOKENSIZE);
							}
						}
					}
					else {
						showError(outputToken, col, row, buf, tokenCrash, streamOutput, NULL);
						SupressOutput = true;
					}
				}
				else {
					showError(outputToken, col, row, buf, tokenCrash, streamOutput, NULL);
					SupressOutput = true;
				}
			}
			else {
				showError(outputToken, col, row, buf, tokenCrash, streamOutput, NULL);
				SupressOutput = true;
			}
		}
		if (SupressOutput == false) {
			fprintf(outputToken, "%-3d %-3d %-8d %-10s\n", row, beg_col, lexCode, buf);
			pushLexRow(beg_col, row, lexCode, buf, lexRow, lexSize);
		}
		free(buf);
	} while (nextCharachter != EOF);
}

//initializing of single symbol dividers
void initializeOneDivider() {
	for (int i = 0; i < 255; i++) {
		//0-whitespace
		if ((i >= 8 && i <= 13) || i == 32)
			SymbolCategories[i] = 0;
		//1-digits
		else if (i >= 48 && i <= 57)
			SymbolCategories[i] = 1;
		//2-identificators
		else if (i >= 65 && i <= 90)
			SymbolCategories[i] = 2;
		//3-one symbol divider
		else if (i == 46 || i == 59 || i == 61)
			SymbolCategories[i] = 3;
		//4-one symbol divider
		else if (i == 60)
			SymbolCategories[i] = 41;
		else if (i == 62)
			SymbolCategories[i] = 42;
		//5-one symbol divider comment
		else if (i == 40)
			SymbolCategories[i] = 5;
		//6-error
		else if ((i >= 0 && i <= 8) || i == 127 || i == 41)
			SymbolCategories[i] = 6;
		else if (i == 35) {
			SymbolCategories[i] = 7;
		}
		else
			SymbolCategories[i] = 6;
	}
}


//initializing keyword table
void KeyWordTab(struct nlist** htab) {
	pushBack("PROGRAM", 401, htab, DIVKEYSIZE);
	pushBack("BEGIN", 402, htab, DIVKEYSIZE);
	pushBack("END", 403, htab, DIVKEYSIZE);
	pushBack("ENDIF", 404, htab, DIVKEYSIZE);
	pushBack("WHILE", 405, htab, DIVKEYSIZE);
	pushBack("DO", 406, htab, DIVKEYSIZE);
	pushBack("ENDWHILE", 407, htab, DIVKEYSIZE);
	pushBack("IF", 408, htab, DIVKEYSIZE);
	pushBack("THEN", 409, htab, DIVKEYSIZE);
	pushBack("ELSE", 410, htab, DIVKEYSIZE);
	pushBack("CONST", 411, htab, DIVKEYSIZE);
}

//initializing multi divider table
void multiDivWordTab(struct nlist** htab) {
	pushBack("<=", 301, htab, DIVKEYSIZE);
	pushBack("<>", 302, htab, DIVKEYSIZE);
	pushBack(">=", 303, htab, DIVKEYSIZE);
}

//initialize tables with initial values
void initializeTables() {
	KeyWordTab(keytab);
	multiDivWordTab(divtab);
	initializeOneDivider();
	initLexRow(lexRow, lexSize);
}

//free the memory of allocated tables
void destroyTables() {
	cleanAlloc(consttab, CONSTIDNSIZE);
	cleanAlloc(keytab, DIVKEYSIZE);
	cleanAlloc(idntab, IDNSIZE);
	cleanAlloc(divtab, DIVKEYSIZE);
	cleanAlloc(tokentab, TOKENSIZE);
	cleanLex(lexRow, lexSize);
	deltree(root);
}
