
#include "HW2-lexicalAnalyzer.h"

char  *reservedWords[] = {"const", "int", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd"};

const char letters[NUM_LETTERS] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
                    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

const char digits[NUM_DIGITS] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

const char specialSym[NUM_SPECIAL_SYM] = {'+', '-', '*', '/', '(', ')', '=', ',' , '.', '<', '>',  ';' , ':'};

const char assignSym[2] = ":=";

int ssym[256];

int isDone=0;

lexeme lexeme_table[MAX_NAME_TABLE_SIZE];

int analyze(){

    FILE *in;

    char myString[strmax];
    int myStringIndex=0;

    int lexeme_index=0;

    if((in=fopen("lexAnalyzerInput.txt","r"))==NULL) {
        printf("\nError: Cannot open scanner input file for reading. Program will now exit.\n");
        exit(1);
    }

    ssym['+'] = plussym;
    ssym['-'] = minussym;
    ssym['*'] = multsym;
    ssym['/'] = slashsym;
    ssym['('] = lparentsym;
    ssym[')'] = rparentsym;
    ssym['='] = eqlsym;
    ssym[','] = commasym;
    ssym['.'] = periodsym;
    ssym['#'] = neqsym;
    ssym['<'] = lessym;
    ssym['>'] = gtrsym;
    ssym['$'] = leqsym;
    ssym['%'] = geqsym;
    ssym[';'] = semicolonsym;

    //array to store the strng read in with the max size for string + 1 for terminateing char
    char stringIn[strmax+1];

    retVals mainRetVals;
    mainRetVals.wasComment = 0;
    mainRetVals.index = -1;

    char charIn='0';

    printf("Source Program:\n");

    if(fscanf(in, "%c", &charIn) == EOF){
        isDone = 1;
        printf("Error: File is empty. Program will now exit.\n");
        exit(0);
    }
    printf("%c", charIn);

    //Go character by character separating the program into tokens
    while(1){

        //when we reach the special character '.' our program has ended
        if(charIn == '.'){
            mainRetVals = getSpecial(in, charIn, lexeme_index);
            lexeme_index = mainRetVals.index;
            break;
        }

        while((charIn == ' ') || (charIn == '\n') || (charIn == '\t') ){

            if(fscanf(in, "%c", &charIn) == EOF){
                break;
            }
            printf("%c", charIn);

        }

        //if there is no period to indicate end of program then we use feof to break
        //out of the loop
        if(feof(in)){
            break;
        }

        //if the comment was the last sequence in the file then we exit the loop *this isnt really necessary*
        if(isDone){
            break;
        }

        //check if the next character is an identfifer or reserved word
        mainRetVals = getIdentifier(in, charIn, lexeme_index);

        if(lexeme_index == mainRetVals.index){

            mainRetVals = getNumber(in, charIn, lexeme_index);

            if(lexeme_index == mainRetVals.index){

                mainRetVals = getSpecial(in, charIn, lexeme_index);

                if( (lexeme_index == mainRetVals.index) && (!(mainRetVals.wasComment)) ){
                    printf("\n\nError: '%c' symbol not recognized, program will now exit\n", charIn);
                    exit(1);
                }
                else{
                    charIn = mainRetVals.currChar;
                    lexeme_index = mainRetVals.index;
                }

            }
            else{
                charIn = mainRetVals.currChar;
                lexeme_index = mainRetVals.index;
            }

        }
        else{
            charIn = mainRetVals.currChar;
            lexeme_index = mainRetVals.index;
        }


    }

    printf("\n");

    fclose(in);

    return lexeme_index;

}

//returns a 1 if the char passed in is a digit 0-9
int isDigit(char c){
    int i;
    for(i=0; i<NUM_DIGITS; i++){
        if(c == digits[i]){
            return 1;
        }
    }
    return 0;
}

//returns a 1 if the char passed in is a letters a-z or A-Z
int isLetter(char c){
    int i;
    for(i=0; i<NUM_LETTERS; i++){
        if(c == letters[i]){                //printf("c=%d, letters[i]=%d\n", c, letters[i]);
            return 1;
        }
        if(c == toupper(letters[i])){
            return 1;
        }
    }
    return 0;
}

//function that checks in a character is a special symbol
//returns 0 if not a special char, 2 in the special case that it is ':'
//and 1 if it is any other special char
int isSpecialSym(char c){
    int i;
    for(i=0; i<NUM_SPECIAL_SYM; i++){
        if(c == specialSym[i]){
            return 1;
        }
    }
    return 0;
}

//Inserts the given string into the symbol table
void insertIntoST(char* stringIn, int STindex, int type){
    strcpy(lexeme_table[STindex].name, stringIn);
    lexeme_table[STindex].tokenType = type;
    return;
}

int isReservedWord(char* str){
    int i=0;
    for(i=0; i<norw; i++){
        if(strcmp(reservedWords[i], str)==0){
            int rwType;
            rwType = findRWtokenType(i);
            return rwType;
        }
    }
    return -1;
}


int isComment(char ch, FILE *in, int lexeme_index){

    if(ch == '*'){
        while(1){
            if(fscanf(in, "%c", &ch) == EOF){
                isDone = 1;
                break;
            }
            printf("%c", ch);
            if(ch == '*'){
                if(fscanf(in, "%c", &ch) == EOF){
                    isDone = 1;
                    break;
                }
                printf("%c", ch);
                if(ch == '/'){
                    if(fscanf(in, "%c", &ch) == EOF){
                        isDone = 1;
                        break;
                    }
                    printf("%c", ch);
                    if((ch = fgetc(in)) == EOF){
                        isDone=1;
                    }
                    printf("%c", ch);
                    return 1;
                }
            }
        }
    }
    return 0;

}

int findRWtokenType(int rwIndex){
    switch(rwIndex){
        case 0: return 28;
        case 1: return 29;
        case 2: return 30;
        case 3: return 27;
        case 4: return 21;
        case 5: return 22;
        case 6: return 23;
        case 7: return 24;
        case 8: return 33;
        case 9: return 25;
        case 10: return 26;
        case 11: return 32;
        case 12: return 31;
        case 13: return 8;
        default: return -1;
    }
}

//function that verifies that the identifier is less than 11
//characters or that a number has less than 5 digits
//(if typeToCheck=1, we check identifier length,
//  if typeToCheck=2, we check number length)
void checkValidLength(char* str, int typeToCheck, int strIndex){

    if(typeToCheck == 1){
        if(strIndex > cmax){
            printf("Error: Identifier %s is too long, identifiers cannot have more than %d characters\n", str, cmax);
            exit(1);
        }
    }

    else if(typeToCheck == 2){
        if(strIndex > nmax){
            printf("Error: Number %s is too long, numbers cannot have more than %d digits\n", str, nmax);
            exit(1);
        }
    }
    else{
        printf("Error in checkValidLength function, invalid typeToCheck");
        exit(1);
    }

}


retVals getIdentifier(FILE *in, char charIn, int lexeme_index){

   if(isLetter(charIn)){

      char myString[strmax];
      int myStringIndex=0;

      while ((isLetter(charIn)) || (isDigit(charIn))){
          myString[myStringIndex] = charIn;
          myStringIndex++;
          if(fscanf(in, "%c", &charIn) == EOF){
            isDone = 1;
            break;
          }
          printf("%c", charIn);
      }

      myString[myStringIndex] = '\0';

      //check that identifiers have a valid amount of characters
      checkValidLength(myString, 1, myStringIndex);

      int rwType=-1;

      //if it is a reserved word then insert into table with the corresponding token type
      if((rwType = isReservedWord(myString)) > 0){
         insertIntoST(myString, lexeme_index, rwType);
      }

      //if it's not a reserved word, then it is an identifier
      else{
           insertIntoST(myString, lexeme_index, identsym);
       }

       //if we insert into ST then we must move to the next ST index
       //for the next insert
       lexeme_index++;

    }

    retVals valsToReturn;
    valsToReturn.wasComment = 0;
    valsToReturn.index = lexeme_index;
    valsToReturn.currChar = charIn;

    return valsToReturn;

}

retVals getNumber(FILE *in, char charIn, int lexeme_index){

    //If the first character is a digit then we have a number value
    if(isDigit(charIn)){

        char myString[strmax];
        int myStringIndex=0;

        //loop getting the rest of the number
        while (isDigit(charIn)){
            myString[myStringIndex] = charIn;
            myStringIndex++;
            if(fscanf(in, "%c", &charIn) == EOF){
                isDone = 1;
                break;
            }
            if(isLetter(charIn)){
                printf("Error: Invalid identifier\n");
                exit(1);
            }
            printf("%c", charIn);
        }

        //add a terminating character to the string
        myString[myStringIndex] = '\0';

        //Check that number values have a valid amount of digits
        checkValidLength(myString, 2, myStringIndex);

        //insert into symbol table
        insertIntoST(myString, lexeme_index, numbersym);

        lexeme_index++;

    }

    retVals valsToReturn;
    valsToReturn.wasComment = 0;
    valsToReturn.index = lexeme_index;
    valsToReturn.currChar = charIn;

    return valsToReturn;

}

retVals getSpecial(FILE *in, char charIn, int lexeme_index){  //printf("in getSpecial charIn=%c\n", charIn);

    retVals valsToReturn;
    valsToReturn.wasComment = 0;

    //Check if charIn is a special symbol
    if(isSpecialSym(charIn)){

        char myString[strmax];
        int myStringIndex=0;

        myString[myStringIndex] = charIn;
        myStringIndex++;

        //check if we have the assign/becomes symbol
        if(charIn == ':'){
            if(fscanf(in, "%c", &charIn) == EOF){
                isDone = 1;
            }
            printf("%c", charIn);
            if(charIn == '='){
                myString[myStringIndex] = charIn;
                myStringIndex++;
                myString[myStringIndex] = '\0';
                insertIntoST(myString, lexeme_index, becomessym);
                if(fscanf(in, "%c", &charIn) == EOF){
                    isDone = 1;
                }
                printf("%c", charIn);
            }
            else{
                printf("Syntax error: char ':' must be followed by '='\n");
                exit(1);
            }
        }
        //check if we have the greater or equal to
        else if(charIn == '>'){

            if(fscanf(in, "%c", &charIn) == EOF){
                isDone = 1;
            }

            printf("%c", charIn);

            if(charIn == '='){
                myString[myStringIndex] = charIn;
                myStringIndex++;
                myString[myStringIndex] = '\0';
                insertIntoST(myString, lexeme_index, geqsym);
                fscanf(in, "%c", &charIn);
                printf("%c", charIn);
            }
            else{
                myString[myStringIndex] = '\0';
                insertIntoST(myString, lexeme_index, gtrsym);
            }
        }
        //check if we have the less or equal symbol, or the NOT symbol '<>'
        else if(charIn == '<'){

            if(fscanf(in, "%c", &charIn) == EOF){
                isDone = 1;
            }
            printf("%c", charIn);

            if((charIn == '=') || (charIn == '>')){

                myString[myStringIndex] = charIn;
                myStringIndex++;
                myString[myStringIndex] = '\0';
                if(charIn == '='){
                    insertIntoST(myString, lexeme_index, leqsym);
                }
                else{
                    insertIntoST(myString, lexeme_index, neqsym);
                }
                if(fscanf(in, "%c", &charIn) == EOF){
                    isDone = 1;
                }
                printf("%c", charIn);

            }

            else{
                myString[myStringIndex] = '\0';
                insertIntoST(myString, lexeme_index, lessym);
            }

        }
        //if we have a slash symbol we have to check if it is a division or if
        //the symbol indicates the beginning of a comment
        else if(charIn == '/'){

            if(fscanf(in, "%c", &charIn) == EOF){
                isDone = 1;
            }
	    else{
                printf("%c", charIn);
            }
            //if it is not a comment we insert the slash sym into the ST
            if(!(isComment(charIn, in, lexeme_index))){
                myString[myStringIndex] = '\0';
                insertIntoST(myString, lexeme_index, slashsym);
            }
            //if it was a comment we set valsToReturn.wasComment=1 so that in main
            //we know that symbol was recognzied but ignored. Also we move the character forward
            else{
                valsToReturn.wasComment = 1;
                lexeme_index--;
                if(fscanf(in, "%c", &charIn) == EOF){
                    isDone = 1;
                }
		else{
                    printf("%c", charIn);
		}
            }

        }
        //if we reached the program ending character then we insert it
        //into the table and DON'T scan or print another character
        else if(charIn == '.'){
            myString[myStringIndex] = '\0';
            insertIntoST(myString, lexeme_index, ssym[*myString]);
        }
        //otherwise it is one of the 1 character special symbols
        else{
            myString[myStringIndex] = '\0';
            insertIntoST(myString, lexeme_index, ssym[*myString]);
            if(fscanf(in, "%c", &charIn) == EOF){
                isDone = 1;
            }
	    else{
                printf("%c", charIn);
	    }
        }

        lexeme_index++;

    }

    valsToReturn.index = lexeme_index;
    valsToReturn.currChar = charIn;

    return valsToReturn;

}


void printLexAnalyzerOutput(int printToScreen, int lex_index){

    FILE *out;

    if((out=fopen("scanner-output.txt","w"))==NULL) {
        printf("\nError: Cannot open scanner output file for writing. Program will now exit.\n");
        exit(1);
    }

    fprintf(out, "Lexeme Table:\n");
    fprintf(out, "lexeme\t\ttoken type\n");
    if(printToScreen){
        printf("Lexeme Table:\n");
        printf("lexeme\t\ttoken type\n");
    }

    int k=0;
    for(k=0; k < lex_index; k++){
        fprintf(out, "%s\t\t%d\n",lexeme_table[k].name, lexeme_table[k].tokenType);
        if(printToScreen){
            printf("%s\t\t%d\n",lexeme_table[k].name, lexeme_table[k].tokenType);
        }
    }

    fprintf(out, "\nLexeme List:\n");
    if(printToScreen){
        printf("\nLexeme List:\n");
    }

    for(k=0; k < lex_index; k++){
        int tokType = lexeme_table[k].tokenType;
        fprintf(out, "%d ", tokType);
        if(printToScreen){
            printf("%d ", tokType);
        }
        if((tokType == 2) || (tokType == 3)){
            fprintf(out, "%s ", lexeme_table[k].name);
            if(printToScreen){
                printf("%s ", lexeme_table[k].name);
            }
        }
    }

    fprintf(out, "\n\nSymbolic Representation:\n");
    if(printToScreen){
        printf("\n\nSymbolic Representation:\n");
    }

    printSymbolicRep(printToScreen, lex_index, out);

    fprintf(out, "\n");
    if(printToScreen){
        printf("\n");
    }

    fclose(out);

}

void printSymbolicRep(int printToScreen, int lexeme_index, FILE *out){

    int i;
    for(i=0; i<lexeme_index; i++){

        switch(lexeme_table[i].tokenType){

            case 1: fprintf(out, "nulsym "); if(printToScreen) printf("nulsym "); break;
            case 2: fprintf(out, "identsym "); if(printToScreen) printf("identsym "); break;
            case 3: fprintf(out, "numbersym "); if(printToScreen) printf("numbersym "); break;
            case 4: fprintf(out, "plussym "); if(printToScreen) printf("plussym "); break;
            case 5: fprintf(out, "minussym "); if(printToScreen) printf("minussym "); break;
            case 6: fprintf(out, "multsym "); if(printToScreen) printf("multsym "); break;
            case 7: fprintf(out, "slashsym "); if(printToScreen) printf("slashsym "); break;
            case 8: fprintf(out, "oddsym "); if(printToScreen) printf("oddsym "); break;
            case 9: fprintf(out, "eqlsym "); if(printToScreen) printf("eqlsym "); break;
            case 10: fprintf(out, "neqsym "); if(printToScreen) printf("neqsym "); break;
            case 11: fprintf(out, "lessym "); if(printToScreen) printf("lessym "); break;
            case 12: fprintf(out, "leqsym "); if(printToScreen) printf("leqsym "); break;
            case 13: fprintf(out, "gtrsym "); if(printToScreen) printf("gtrsym "); break;
            case 14: fprintf(out, "geqsym "); if(printToScreen) printf("geqsym "); break;
            case 15: fprintf(out, "lparentsym "); if(printToScreen) printf("lparentsym "); break;
            case 16: fprintf(out, "rparentsym "); if(printToScreen) printf("rparentsym "); break;
            case 17: fprintf(out, "commasym "); if(printToScreen) printf("commasym "); break;
            case 18: fprintf(out, "semicolonsym "); if(printToScreen) printf("semicolonsym "); break;
            case 19: fprintf(out, "periodsym "); if(printToScreen) printf("periodsym "); break;
            case 20: fprintf(out, "becomessym "); if(printToScreen) printf("becomessym "); break;
            case 21: fprintf(out, "beginsym "); if(printToScreen) printf("beginsym "); break;
            case 22: fprintf(out, "endsym "); if(printToScreen) printf("endsym "); break;
            case 23: fprintf(out, "ifsym "); if(printToScreen) printf("ifsym "); break;
            case 24: fprintf(out, "thensym "); if(printToScreen) printf("thensym "); break;
            case 25: fprintf(out, "whilesym "); if(printToScreen) printf("whilesym "); break;
            case 26: fprintf(out, "dosym "); if(printToScreen) printf("dosym "); break;
            case 27: fprintf(out, "callsym "); if(printToScreen) printf("callsym "); break;
            case 28: fprintf(out, "constsym "); if(printToScreen) printf("constsym "); break;
            case 29: fprintf(out, "intsym "); if(printToScreen) printf("intsym "); break;
            case 30: fprintf(out, "procsym "); if(printToScreen) printf("procsym "); break;
            case 31: fprintf(out, "writesym "); if(printToScreen) printf("writesym "); break;
            case 32: fprintf(out, "readsym "); if(printToScreen) printf("readsym "); break;
            case 33: fprintf(out, "elsesym "); if(printToScreen) printf("elsesym "); break;

            default:   printf("Error in printSymbolicRep(), unrecognized symbol!.");
                       exit(1);
                       break;

        }

    }


}

