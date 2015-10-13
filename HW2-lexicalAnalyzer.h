#ifndef HW2-LEXICALANALYZER_H_INCLUDED
#define HW2-LEXICALANALYZER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define norw      14         /* number of reserved words */
#define nmax   5       /* maximum integer length */
#define cmax      11         /* maximum number of chars for idents */
#define nestmax    5         /* maximum depth of block nesting */
#define strmax   256         /* maximum length of strings */
#define NUM_LETTERS 26
#define NUM_DIGITS 10
#define NUM_SPECIAL_SYM 13
#define MAX_NAME_TABLE_SIZE 100

//struct that holds each lexeme's type and name
typedef struct {
	int tokenType;
	char name[strmax];
} lexeme;

//need to return the current ST index and character from
//the getIdentifier, getNumber, and getSpecial functions;
//we will do so through this struct
typedef struct{
    int index;
    char currChar;
    int wasComment;
} retVals;

typedef enum {nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,  slashsym = 7,
    oddsym = 8,  eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14,
    lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20,
    beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, dosym = 26, callsym = 27,
    constsym = 28, intsym = 29, procsym = 30, writesym = 31, readsym = 32, elsesym = 33} token_type;

extern lexeme lexeme_table[MAX_NAME_TABLE_SIZE];

//Functions declarations:
int  analyze();
int isDigit(char c);
int isLetter(char c);
int isSpecialSym(char c);
void insertIntoST(char* stringIn, int STindex, int type);
int isReservedWord(char* str);
int isComment(char ch, FILE *in, int lexeme_index);
int findRWtokenType(int rwIndex);
void checkValidLength(char* str, int typeToCheck, int strIndex);
retVals getIdentifier(FILE *in, char charIn, int lexeme_index);
retVals getNumber(FILE *in, char charIn, int lexeme_index);
retVals getSpecial(FILE *in, char charIn, int lexeme_index);
void printLexAnalyzerOutput(int printToScreen, int lex_index);
void printSymbolicRep(int printToScreen, int lexeme_index, FILE *out);

#endif // HW2-LEXICALANALYZER_H_INCLUDED
