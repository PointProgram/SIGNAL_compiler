#include "lexer.h"
#include "log.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
void Lexer(FILE* readFile, FILE* outputTocen) {
	char getNextCharachter = fgetc(readFile);
	struct symbol sumb;
	char* buf;
	int lexCode;
	bool SupressOutput = false;
	enum outputType streamOutput = noneOut;
	int col = 1, row = 1, beg_col;
	do {
		sumb.value = getNextCharachter;
		sumb.attr = SymbolCategories[(int)getNextCharachter];
		buf = NULL;
		lexCode = 0;
		SupressOutput = false;
		//whitespace processing 
		if (sumb.attr == 0) {
			while (getNextCharachter != EOF) {
				updateRecAttr(&row, &col, getNextCharachter);
				getNextCharachter = fgetc(readFile);
				sumb.value = getNextCharachter;
				sumb.attr = SymbolCategories[(int)getNextCharachter];
				if (sumb.attr > 0 || sumb.attr < 0)
					break;
			}
			SupressOutput = true;
		}
		//digit constants processing
		else if (sumb.attr == 1) {
			beg_col = col;
			while (getNextCharachter != EOF && sumb.attr == 1) {
				buf = add_char(buf, sumb.value);
				updateRecAttr(&row, &col, getNextCharachter);
				getNextCharachter = fgetc(readFile);
				sumb.value = getNextCharachter;
				sumb.attr = SymbolCategories[(int)getNextCharachter];
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
			while ((getNextCharachter != EOF) && (sumb.attr == 2 || sumb.attr == 1)) {
				buf = add_char(buf, sumb.value);
				updateRecAttr(&row, &col, getNextCharachter);
				getNextCharachter = fgetc(readFile);
				sumb.value = getNextCharachter;
				sumb.attr = SymbolCategories[(int)getNextCharachter];
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
			updateRecAttr(&row, &col, getNextCharachter);
			getNextCharachter = fgetc(readFile);
			sumb.value = getNextCharachter;
			sumb.attr = SymbolCategories[(int)getNextCharachter];
		}
		//processing of multi-symbolic dividers <= and <>
		else if (sumb.attr == 41) {
			beg_col = col;
			buf = add_char(buf, sumb.value);
			char previousVal = sumb.value;
			updateRecAttr(&row, &col, getNextCharachter);
			getNextCharachter = fgetc(readFile);
			sumb.value = getNextCharachter;
			sumb.attr = SymbolCategories[(int)getNextCharachter];
			if (getNextCharachter == '=') {
				buf = add_char(buf, sumb.value);
				lexCode = getByName(buf, divtab, DIVKEYSIZE);
				pushBack(buf, lexCode, divtab, DIVKEYSIZE);
				updateRecAttr(&row, &col, getNextCharachter);
				getNextCharachter = fgetc(readFile);
				sumb.value = getNextCharachter;
				sumb.attr = SymbolCategories[(int)getNextCharachter];
			}
			else if (getNextCharachter == '>') {
				buf = add_char(buf, sumb.value);
				lexCode = getByName(buf, divtab, DIVKEYSIZE);
				pushBack(buf, lexCode, divtab, DIVKEYSIZE);
				updateRecAttr(&row, &col, getNextCharachter);
				getNextCharachter = fgetc(readFile);
				sumb.value = getNextCharachter;
				sumb.attr = SymbolCategories[(int)getNextCharachter];
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
			updateRecAttr(&row, &col, getNextCharachter);
			getNextCharachter = fgetc(readFile);
			sumb.value = getNextCharachter;
			sumb.attr = SymbolCategories[(int)getNextCharachter];
			if (getNextCharachter == '=') {
				buf = add_char(buf, sumb.value);
				updateRecAttr(&row, &col, getNextCharachter);
				getNextCharachter = fgetc(readFile);
				lexCode = getByName(buf, divtab, DIVKEYSIZE);
				pushBack(buf, lexCode, divtab, DIVKEYSIZE);
				sumb.value = getNextCharachter;
				sumb.attr = SymbolCategories[(int)getNextCharachter];
			}
			else {
				lexCode = previousVal;
			}
		}
		//processing of multi-symbolic comment divider (*
		else if (sumb.attr == 5) {
			if (getNextCharachter == EOF) {
				lexCode = getNextCharachter;
			}
			else {
				short int tmp_row = row, tmp_col = col;
				char nextCh = getNextCharachter;
				updateRecAttr(&row, &col, getNextCharachter);
				getNextCharachter = fgetc(readFile);
				sumb.value = getNextCharachter;
				sumb.attr = SymbolCategories[(int)getNextCharachter];
				bool ifError = false;
				if (sumb.value == '*') {
					updateRecAttr(&row, &col, getNextCharachter);
					getNextCharachter = fgetc(readFile);
					if (getNextCharachter == EOF) {
						showError(outputTocen, col, row, "*)", endFile, streamOutput);
						SupressOutput = true;
					}
					else {
						sumb.value = getNextCharachter;
						sumb.attr = SymbolCategories[(int)getNextCharachter];
						bool symbolTriger = true;
						char tmp_value;
						do {
							tmp_value = sumb.value;
							while (getNextCharachter != EOF && !((sumb.value != '*') ^ symbolTriger)) {
								updateRecAttr(&row, &col, getNextCharachter);
								getNextCharachter = fgetc(readFile);
								sumb.value = getNextCharachter;
								sumb.attr = SymbolCategories[(int)getNextCharachter];
							}
							if (getNextCharachter == EOF) {
								showError(outputTocen, col, row, "*)", endFile, streamOutput);
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
								updateRecAttr(&row, &col, getNextCharachter);
								getNextCharachter = fgetc(readFile);
								sumb.value = getNextCharachter;
								sumb.attr = SymbolCategories[(int)getNextCharachter];
							}
						} while (sumb.value != ')' || symbolTriger);
						if (sumb.value == ')') {
							SupressOutput = true;
						}
						if (getNextCharachter != EOF && !ifError) {
							updateRecAttr(&row, &col, getNextCharachter);
							getNextCharachter = fgetc(readFile);
							sumb.value = getNextCharachter;
							sumb.attr = SymbolCategories[(int)getNextCharachter];
						}
					}
				}
				else {
					buf = add_char(buf, nextCh);
					showError(outputTocen, tmp_col, tmp_row, buf, illegalChar, streamOutput);
					lexCode = getNextCharachter;
					SupressOutput = true;
				}
			}
		}
		//processing of forbidden symbols
		else if (sumb.attr == 6) {
			buf = add_char(buf, getNextCharachter);
			showError(outputTocen, col, row, buf, illegalChar, streamOutput);
			updateRecAttr(&row, &col, getNextCharachter);
			getNextCharachter = fgetc(readFile);
			sumb.value = getNextCharachter;
			sumb.attr = SymbolCategories[(int)getNextCharachter];
			SupressOutput = true;
		}
		else if (sumb.attr == 7) {
			beg_col = col;
			buf = add_char(buf, sumb.value);
			char previousVal = sumb.value;
			updateRecAttr(&row, &col, getNextCharachter);
			getNextCharachter = fgetc(readFile);
			sumb.value = getNextCharachter;
			sumb.attr = SymbolCategories[(int)getNextCharachter];
			if (getNextCharachter >= '0' && getNextCharachter <= '9') {
				while (getNextCharachter >= '0' && getNextCharachter <= '9') {
					buf = add_char(buf, sumb.value);
					updateRecAttr(&row, &col, getNextCharachter);
					getNextCharachter = fgetc(readFile);
					sumb.value = getNextCharachter;
					sumb.attr = SymbolCategories[(int)getNextCharachter];
				}
				if (getNextCharachter == '-') {
					buf = add_char(buf, sumb.value);
					updateRecAttr(&row, &col, getNextCharachter);
					getNextCharachter = fgetc(readFile);
					sumb.value = getNextCharachter;
					sumb.attr = SymbolCategories[(int)getNextCharachter];
					if (getNextCharachter >= '0' && getNextCharachter <= '9') {
						while (getNextCharachter >= '0' && getNextCharachter <= '9') {
							buf = add_char(buf, sumb.value);
							updateRecAttr(&row, &col, getNextCharachter);
							getNextCharachter = fgetc(readFile);
							sumb.value = getNextCharachter;
							sumb.attr = SymbolCategories[(int)getNextCharachter];
						}
						if (getNextCharachter == '-') {
							buf = add_char(buf, sumb.value);
							updateRecAttr(&row, &col, getNextCharachter);
							getNextCharachter = fgetc(readFile);
							sumb.value = getNextCharachter;
							sumb.attr = SymbolCategories[(int)getNextCharachter];
							if (getNextCharachter >= '0' && getNextCharachter <= '9') {
								while (getNextCharachter >= '0' && getNextCharachter <= '9') {
									buf = add_char(buf, sumb.value);
									updateRecAttr(&row, &col, getNextCharachter);
									getNextCharachter = fgetc(readFile);
									sumb.value = getNextCharachter;
									sumb.attr = SymbolCategories[(int)getNextCharachter];
								}
							}
							int codeOfLexTab = getByName(buf, tocentab, TOCENSIZE);
							if (codeOfLexTab != 0) {
								lexCode = codeOfLexTab;
							}
							else {
								lexCode = showLastLexCode(tocentab, tcnWord, TOCENSIZE) + 1;
								pushBack(buf, lexCode, tocentab, TOCENSIZE);
							}
						}
					}
					else {
						showError(outputTocen, col, row, buf, tocenCrash, streamOutput);
						SupressOutput = true;
					}
				}
				else {
					showError(outputTocen, col, row, buf, tocenCrash, streamOutput);
					SupressOutput = true;
				}
			}
			else {
				showError(outputTocen, col, row, buf, tocenCrash, streamOutput);
				SupressOutput = true;
			}
		}
		if (SupressOutput == false) {
			fprintf(outputTocen, "%-3d %-3d %-8d %-10s\n", row, beg_col, lexCode, buf);
		}
		free(buf);
	} while (getNextCharachter != EOF);
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
}

//initializing multi divider table
void multiDivWordTab(struct nlist** htab) {
	pushBack("<=", 301, htab, DIVKEYSIZE);
	pushBack("<>", 302, htab, DIVKEYSIZE);
	pushBack(">=", 303, htab, DIVKEYSIZE);
}


void initializeTables() {
	KeyWordTab(keytab);
	multiDivWordTab(divtab);
	initializeOneDivider();
}

void destroyTables() {
	cleanAlloc(consttab, CONSTIDNSIZE);
	cleanAlloc(keytab, DIVKEYSIZE);
	cleanAlloc(idntab, IDNSIZE);
	cleanAlloc(divtab, DIVKEYSIZE);
	cleanAlloc(tocentab, TOCENSIZE);
}
