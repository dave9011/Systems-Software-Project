#ifndef HW3-PARSER_H_INCLUDED
#define HW3-PARSER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_TABLE_SIZE 100

typedef struct{
	int kind; 		// const = 1, int = 2, proc = 3
	char name[10];	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
} symbol;

extern symbol symbol_table[MAX_NAME_TABLE_SIZE];

void foundError(int code);
void checkProgram();
int checkBlock(int level, int lexTableIndex);
int checkStatement(int lexTableIndex);
int checkExpression(int lexTableIndex);
int checkTerm(int lexTableIndex);
int checkFactor(int lexTableIndex);
int checkCondition(int lexTableIndex);
void enterIntoSymTable(int kind, char* name, char* value, int level, int address);
void printParserOutput(int printToScreen);
void checkCall(int index);
void checkDeclaredIdentifier(int index);
void checkValidAssign(int lexTableIndex);

#endif // HW3-PARSER_H_INCLUDED
