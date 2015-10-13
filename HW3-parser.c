
#include "HW3-parser.h"
#include "HW2-lexicalAnalyzer.h"

int symTableIndex=0;

int addressM = 4;

const char splSym[NUM_SPECIAL_SYM] = {'+', '-', '*', '/', '(', ')', '=', ',' , '.', '<', '>',  ';' , ':'};

symbol symbol_table[MAX_NAME_TABLE_SIZE];

void checkProgram(){

    int lexTableIndex = 0;

    lexTableIndex = checkBlock(0, lexTableIndex);

    if(lexeme_table[lexTableIndex].tokenType != periodsym){
        int s;
        for(s=0; s<NUM_SPECIAL_SYM; s++){
            if(strcmp(lexeme_table[lexTableIndex].name, &splSym[s]) == 0){
                printf("***Error 8: Incorrect symbol after statement part in block\n");
                exit(1);
            }
        }
        printf("***Error 9: Period expected at the end of the program\n");
        exit(1);
    }

}

int checkBlock(int level, int lexTableIndex){

    char ident[strmax];
    char number[strmax];

    if(lexeme_table[lexTableIndex].tokenType == constsym){

        do{
            lexTableIndex++;

            if(lexeme_table[lexTableIndex].tokenType != identsym){
                printf("***Error 4: \"const\" must be followed by an identifier\n");
                exit(1);
            }

            checkValidAssign(lexTableIndex);
            strcpy(ident, lexeme_table[lexTableIndex].name);
            lexTableIndex++;

            if(lexeme_table[lexTableIndex].tokenType != eqlsym){
                printf("***Error 3: In const-declaration, identifier must be followed by a '='\n");
                exit(1);
            }
            lexTableIndex++;

            if(lexeme_table[lexTableIndex].tokenType != numbersym){
                printf("***Error 2: In const-declaration, '=' must be followed by a number\n");
                exit(1);
            }
            strcpy(number, lexeme_table[lexTableIndex].name);


            enterIntoSymTable(1, ident, number, -1, -1);

            lexTableIndex++;


        } while(lexeme_table[lexTableIndex].tokenType == commasym);

        if(lexeme_table[lexTableIndex++].tokenType != semicolonsym){
            printf("***Error 5: In const-declaration, semicolon/comma missing\n");
            exit(1);
        }

    }

    if(lexeme_table[lexTableIndex].tokenType == intsym){

        do{
            lexTableIndex++;

            if(lexeme_table[lexTableIndex].tokenType != identsym){
                printf("***Error 4: \"int\" must be followed by an identifier\n");
                exit(1);
            }
            strcpy(number, lexeme_table[lexTableIndex].name);

            lexTableIndex++;

            enterIntoSymTable(2, number, "", level, addressM);
            addressM ++;

        } while(lexeme_table[lexTableIndex].tokenType == commasym);

        if(lexeme_table[lexTableIndex++].tokenType != semicolonsym){
            printf("***Error 5: In int-declaration, semicolon/comma missing\n");
            exit(1);
        }

    }

    if(lexeme_table[lexTableIndex].tokenType == procsym){

        do{
            lexTableIndex++;

            if(lexeme_table[lexTableIndex].tokenType != identsym){
                printf("***Error 4: \"procedure\" must be followed by an identifier\n");
                exit(1);
            }
            strcpy(ident, lexeme_table[lexTableIndex].name);

            enterIntoSymTable(3, ident, "", level, -2);

            lexTableIndex++;

            if(lexeme_table[lexTableIndex].tokenType != semicolonsym){
                printf("***Error 5: In proc-declaration, semicolon/comma missing\n");
                exit(1);
            }
            lexTableIndex++;

            lexTableIndex = checkBlock(level+1, lexTableIndex);

             if(lexeme_table[lexTableIndex].tokenType != semicolonsym){
                printf("***Error 5: In proc-declaration, semicolon/comma missing\n");
                exit(1);
            }
            lexTableIndex++;


        } while(lexeme_table[lexTableIndex].tokenType == procsym);


    }

    int oldLexIndex = lexTableIndex;
    lexTableIndex = checkStatement(lexTableIndex);

    //if the index didnt change then we didnt get a statement
    if(lexTableIndex == oldLexIndex){
        printf("***Error 7: Statement expected\n");
        exit(1);
    }

    return lexTableIndex;

}

int checkStatement(int lexTableIndex){

    if(lexeme_table[lexTableIndex].tokenType == identsym){

        checkValidAssign(lexTableIndex);

        lexTableIndex++;

        if(lexeme_table[lexTableIndex].tokenType != becomessym){

                if(lexeme_table[lexTableIndex].tokenType == eqlsym){
                    printf("***Error 1: In statement, used '=' instead of ':='\n");
                }
                else{
                    printf("***Error 13: Assignment operator expected\n");
                }
                exit(1);
        }

        lexTableIndex++;

        lexTableIndex = checkExpression(lexTableIndex);

    }
    else if(lexeme_table[lexTableIndex].tokenType == callsym){

        lexTableIndex++;

        if(lexeme_table[lexTableIndex].tokenType != identsym){
            printf("***Error 14: \"Call\" must be followed by an identifier\n");
            exit(1);
        }

        checkCall(lexTableIndex);

        lexTableIndex++;

    }
    else if(lexeme_table[lexTableIndex].tokenType == beginsym){

        do{
            lexTableIndex++;
            lexTableIndex = checkStatement(lexTableIndex);

        }while(lexeme_table[lexTableIndex].tokenType == semicolonsym);

        if(lexeme_table[lexTableIndex].tokenType != endsym){
            printf("error!-STATEMENT-BEGIN-not endsym\n");
            exit(1);
        }
        lexTableIndex++;


    }
    else if(lexeme_table[lexTableIndex].tokenType == ifsym){

        lexTableIndex++;
        lexTableIndex = checkCondition(lexTableIndex);

        if(lexeme_table[lexTableIndex].tokenType != thensym){
            printf("***Error 16: \"then\" expected\n");
            exit(1);
        }

        lexTableIndex++;
        int oldLexIndex = lexTableIndex;
        lexTableIndex = checkStatement(lexTableIndex);
        if(lexTableIndex == oldLexIndex){
            printf("***Error 7: Statement expected\n");
            exit(1);
        }

    }
    else if(lexeme_table[lexTableIndex].tokenType == whilesym){

        lexTableIndex++;
        lexTableIndex = checkCondition(lexTableIndex);

        if(lexeme_table[lexTableIndex].tokenType != dosym){
            printf("***Error 18: \"do\" expected\n");
            exit(1);
        }

        lexTableIndex++;
        int oldLexIndex = lexTableIndex;
        lexTableIndex = checkStatement(lexTableIndex);
        if(lexTableIndex == oldLexIndex){
            printf("***Error 7: Statement expected\n");
            exit(1);
        }

    }

    return lexTableIndex;

}

int checkExpression(int lexTableIndex){

    int s;
    for(s=0; s<NUM_SPECIAL_SYM; s++){
        if(strcmp(lexeme_table[lexTableIndex].name, &splSym[s]) == 0){
            if((lexeme_table[lexTableIndex].tokenType == plussym) || (lexeme_table[lexTableIndex].tokenType == minussym)){
                lexTableIndex++;
                break;
            }
            else{
                printf("***Error 24: An expression cannot begin with this symbol: %s\n", lexeme_table[lexTableIndex].name);
                exit(1);
            }
        }
    }

    lexTableIndex = checkTerm(lexTableIndex);   //printf("return from term() in expression: current=%s\n", lexeme_table[lexTableIndex].name);

    while((lexeme_table[lexTableIndex].tokenType == plussym) || (lexeme_table[lexTableIndex].tokenType == minussym)){

        //printf("in exp while loop\n");

        lexTableIndex++;
        lexTableIndex = checkTerm(lexTableIndex);
    }

    return lexTableIndex;

}

int checkTerm(int lexTableIndex){   //printf("check term for: %s\n", lexeme_table[lexTableIndex].name);

    lexTableIndex = checkFactor(lexTableIndex);

    while((lexeme_table[lexTableIndex].tokenType == multsym) || (lexeme_table[lexTableIndex].tokenType == slashsym)){
        lexTableIndex++;
        lexTableIndex = checkFactor(lexTableIndex);
    }

    return lexTableIndex;

}

int checkFactor(int lexTableIndex){    //printf("check factor for: %s\n", lexeme_table[lexTableIndex].name);

    if(lexeme_table[lexTableIndex].tokenType == identsym){
        checkDeclaredIdentifier(lexTableIndex);   //printf("in factor we have ident %s\n", lexeme_table[lexTableIndex].name);
        lexTableIndex++;
    }
    else if(lexeme_table[lexTableIndex].tokenType == numbersym){
        lexTableIndex++;
    }
    else if(lexeme_table[lexTableIndex].tokenType == lparentsym){

        lexTableIndex++;
        lexTableIndex = checkExpression(lexTableIndex);
        if(lexeme_table[lexTableIndex].tokenType != rparentsym){
            printf("***Error 22: Right parenthesis missing\n");
            exit(1);
        }
        lexTableIndex++;

    }
    else{
        printf("***Error 23: The preceding factor cannot begin with this symbol: %s\n", lexeme_table[lexTableIndex].name);
        exit(1);
    }

    return lexTableIndex;

}

int checkCondition(int lexTableIndex){

    if(lexeme_table[lexTableIndex].tokenType == oddsym){
        lexTableIndex++;
        lexTableIndex = checkExpression(lexTableIndex);
    }
    else{
        lexTableIndex = checkExpression(lexTableIndex);
        int relationCheck = lexeme_table[lexTableIndex].tokenType;
        if((relationCheck < 9) || (relationCheck > 14)){
            printf("***Error 20: Relational operator expected\n");
            exit(1);
        }
        lexTableIndex++;
        lexTableIndex = checkExpression(lexTableIndex);
    }

    return lexTableIndex;

}

void enterIntoSymTable(int kind, char* name, char* value, int level, int address){

    //for constants
    if(kind == 1){
        symbol_table[symTableIndex].kind = kind;
        strcpy(symbol_table[symTableIndex].name, name);
        symbol_table[symTableIndex].val = strToInt(value);

    }
    //variables
    if(kind == 2){
        symbol_table[symTableIndex].kind = kind;
        strcpy(symbol_table[symTableIndex].name, name);
        symbol_table[symTableIndex].level = level;
        symbol_table[symTableIndex].addr = address;
    }
    //procedures
    if(kind == 3){
        symbol_table[symTableIndex].kind = kind;
        strcpy(symbol_table[symTableIndex].name, name);
        symbol_table[symTableIndex].level = level;
        symbol_table[symTableIndex].addr = address;
    }

    symTableIndex++;

}

int strToInt(char* str){

    int c = 0;
    int value=0;
    while(str[c] != '\0'){
        value = 10 * value + str[c] - '0';
        c++;
    }

    return value;

}

//Check to see if the program tries to make a call to a non-procedure identifier
//(i.e. a constant or variable); this is a meaningless call so print error message
void checkCall(int index){

    checkDeclaredIdentifier(index);

    int r=0;
    while(r < symTableIndex){
        if(strcmp(lexeme_table[index].name, symbol_table[r].name) == 0){
            if(symbol_table[r].kind != 3){
                printf("***Error 15: Call of ");
                if(symbol_table[r].kind == 1){
                    printf("constant %s is meaningless\n", lexeme_table[index].name);
                }
                if(symbol_table[r].kind == 2){
                    printf("variable %s is meaningless\n", lexeme_table[index].name);
                }
                exit(1);
            }
            break;
        }
        r++;
    }

}

void checkDeclaredIdentifier(int index){

    int r=0, check=0;
    while(r < symTableIndex){
        if(strcmp(lexeme_table[index].name, symbol_table[r].name) == 0){
            check = 1;
            break;
        }
        r++;
    }

    if(check == 0){
        printf("***Error 11: Undeclared identifier %s\n", lexeme_table[index].name);
        exit(1);
    }


}

void printParserOutput(int printToScreen){

    FILE *out;

    if((out=fopen("parser-output.txt","w"))==NULL) {
        printf("\nError: Cannot open parser output file for writing. Program will now exit.\n");
        exit(1);
    }

    fprintf(out, "\n\nResult: No errors, program is syntactically correct\n");
    fprintf(out, "\nSymbol Table:\n");
    if(printToScreen){
        printf("\n\nResult: No errors, program is syntactically correct\n");
        printf("\nSymbol Table:\n");
    }

    int i=0;
    for(i=0; i < symTableIndex; i++){

        fprintf(out, "%d|\t%s", i, symbol_table[i].name);
        if(printToScreen){
            printf("%d|\t%s", i, symbol_table[i].name);
        }

        if(symbol_table[i].kind == 1){
            fprintf(out, "\t%d", symbol_table[i].val);
            if(printToScreen){
                printf("\t%d", symbol_table[i].val);
            }
        }


        fprintf(out, "\t%d", symbol_table[i].addr);
        fprintf(out, "\n");
        if(printToScreen){
            printf("\t%d", symbol_table[i].addr);
            printf("\n");
        }

    }

    fclose(out);

}

void checkValidAssign(int lexTableIndex){

    if(lexeme_table[lexTableIndex].tokenType != 2){
        printf("***Error 12: Assignment to constant or procedure is not allowed\n");
        exit(1);
    }

}
