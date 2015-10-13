
#include "HW2-lexicalAnalyzer.h"
#include "HW3-parser.h"

#include <stdlib.h>
#include <string.h>

void mergeOut();

int main(int argc, char *argv[]){

    int lexemeSize, scannerToScreen = 0, parserToScreen=0;

    lexemeSize = analyze();

    if(argc < 2){
	printf("In -\n");
    }

    int i=0;
    for(i=1; i<argc; i++){
	if(strcmp(argv[i], "-l") == 0){
            scannerToScreen = 1;
            break;
        }
    }
    printLexAnalyzerOutput(scannerToScreen, lexemeSize);

    checkProgram();

    for(i=1; i<argc; i++){
	if(strcmp(argv[i], "-a") == 0){
            parserToScreen = 1;
            break;
        }
    }
    printParserOutput(parserToScreen);

    if(argc < 2){
	printf("\n- Out\n");
    }

    mergeOut();

    return 0;

}


void mergeOut(){

   FILE *fs1, *fs2, *ft;

   char ch;

   fs1 = fopen("scanner-output.txt", "r");
   fs2 = fopen("parser-output.txt", "r");

   if( fs1 == NULL || fs2 == NULL )
   {
      printf("Error merging the two output files\n");
      exit(1);
   }

   ft = fopen("finalOutput.txt", "w");

   if( ft == NULL )
   {
      printf("Error opening finalOutput file\n");
      exit(1);
   }

   while( ( ch = fgetc(fs1) ) != EOF )
      fputc(ch,ft);

   while( ( ch = fgetc(fs2) ) != EOF )
      fputc(ch,ft);

   //printf("Two files were merged successfully.\n");

   fclose(fs1);
   fclose(fs2);
   fclose(ft);

}






