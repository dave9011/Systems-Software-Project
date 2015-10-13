#include <stdio.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

typedef struct node {
    int op;
    int l;
    int m;
} instruction;

int base(int l, int base);
void fetch(int progCounter);
void execute();

//Initialize the stack, the code section, and the 4 registers
int STACK[MAX_STACK_HEIGHT];
instruction code[MAX_CODE_LENGTH];
int SP=0, BP=1, PC=0;
instruction IR;

//variables to keep track of the levels; this will allow us to separate
//the activation records correctly when we print out
int lvl1=0, lvl2=0, lvl3=0;

FILE *in, *ofp;

//array with names of each opcode
const char* opNames[] = {"LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SIO", "SIO"};

int main (){

    int readOp=0, readL=0, readM=0;
    int instNum=0;

    IR.op = 0;
    IR.l = 0;
    IR.m = 0;

    //initialize the stack
    int s;
    for(s=0; s<100; s++){
        STACK[s] = 0;
    }

    //open the input file and check for null; if null, print error message and exit
    if((in=fopen("Pmachine-input.txt","r"))==NULL) {
        printf("\nError: Cannot open projectInput.txt for reading. Program will now exit.\n");
        exit(1);
    }

    //open output file and check for null
    if ((ofp = fopen("Pmachine-output.txt", "w")) == NULL) {
        printf("\nError: Cannot open PMoutput.txt for writing. Program will now exit.\n");
        exit(1);
    }

    //print the header
    fprintf(ofp,"Line\tOP\tL\tM\n");
    fprintf(ofp, "--------------------------\n");

    while(fscanf(in,"%d %d %d",&readOp,&readL, &readM)!=EOF) {

        instruction inst;
        inst.op = readOp;
        inst.l = readL;
        inst.m = readM;
        code[PC++] = inst;
        fprintf(ofp, "%d\t%s\t%d\t%d\n",instNum++,opNames[inst.op-1],inst.l, inst.m);

        if(PC > MAX_CODE_LENGTH)
            break;

    }

    fprintf(ofp, "--------------------------\n\n");
    fprintf(ofp, "----------------------------------------------------------------------------------------------\n");
    fprintf(ofp, "\t\t\t\tpc\tbp\tsp\tstack\n");
    fprintf(ofp, "Initial Values\t\t\t0\t1\t0\t0\n");
    fprintf(ofp, "----------------------------------------------------------------------------------------------\n");

    PC = 0;

    while(BP > 0) {
        fetch(PC);
        execute();
    }

    fclose(in);
    fclose(ofp);

    system("pause");
    return 0;

}

// l stand for L in the instruction format
int base(int l, int base) {

  if(l > MAX_LEXI_LEVELS){
    printf("The given L = %d exceedes the maximum allowed L value of 3", l);
    exit(1);
  }

  int b1; //find base L levels down
  b1 = base;
  while (l > 0)
  {
    b1 = STACK[b1 + 1];
    l--;
  }

  return b1;
}

void fetch(int progCounter){
    IR = code[progCounter];
    fprintf(ofp,"%d\t%s\t%d\t%d",progCounter,opNames[IR.op-1],IR.l, IR.m);
    PC++;
}

void execute(){

    int oldBP1, readInValue;

    //LIT
    if(IR.op == 1){
        SP++;
        STACK[SP] = IR.m;
    }

    //OPR
    else if(IR.op == 2){

        switch(IR.m){

            //return
            case 0: SP = BP-1;
                    PC = STACK[SP + 4];
                    BP = STACK[SP + 3];

                    //delete the latest level tracker
                    if(lvl3>0)
                        lvl3=0;
                    else if(lvl2>0){
                        lvl2=0;
                    }
                    else
                        lvl1=0;

                    break;

            //negative
            case 1: STACK[SP] = -STACK[SP];
                    break;

            //addition
            case 2: SP--;
                    STACK[SP] = STACK[SP] + STACK[SP+1];
                    break;

            //subtraction
            case 3: SP--;
                    STACK[SP] = STACK[SP] - STACK[SP+1];
                    break;

            //multiplication
            case 4: SP--;
                    STACK[SP] = STACK[SP] * STACK[SP+1];
                    break;

            //division
            case 5: SP--;
                    STACK[SP] = STACK[SP] / STACK[SP+1];
                    break;

            //odd
            case 6: STACK[SP] = STACK[SP] % 2;
                    break;

            //mod
            case 7: SP--;
                    STACK[SP] = STACK[SP] % STACK[SP+1];
                    break;

            //equal
            case 8: SP--;
                    if(STACK[SP] == STACK[SP+1]){
                        STACK[SP] = 1;
                    }
                    else{
                        STACK[SP] = 0;
                    }
                    break;

            //not equal
            case 9: SP--;
                    if(STACK[SP] != STACK[SP+1]){
                        STACK[SP] = 1;
                    }
                    else{
                        STACK[SP] = 0;
                    }
                    break;

            //less than
            case 10: SP--;
                     if(STACK[SP] < STACK[SP+1]){
                            STACK[SP] = 1;
                     }
                     else{
                        STACK[SP] = 0;
                     }
                     break;

            //less or equal than
            case 11: SP--;
                     if(STACK[SP] <= STACK[SP+1]){
                            STACK[SP] = 1;
                     }
                     else{
                        STACK[SP] = 0;
                     }
                     break;

            //greater than
            case 12: SP--;
                     if(STACK[SP] > STACK[SP+1]){
                            STACK[SP] = 1;
                     }
                     else{
                        STACK[SP] = 0;
                     }
                     break;

            //greater or equal then
            case 13: SP--;
                     if(STACK[SP] >= STACK[SP+1]){
                            STACK[SP] = 1;
                     }
                     else{
                        STACK[SP] = 0;
                     }
                     break;

            default: printf("Error: Invalid \"M\" value in OPR(IR.op =2)\n");
                     break;

        }

    }

    //LOD
    else if(IR.op == 3){
        SP++;
        STACK[SP] = STACK[base(IR.l, BP) + IR.m];
    }

    //STO
    else if(IR.op == 4){
        STACK[base(IR.l, BP) + IR.m] = STACK[SP];
        SP--;
    }

    //CAL
    else if(IR.op == 5){

        STACK[SP + 1]  = 0;			        // space to return value
        STACK[SP + 2] = base(IR.l, BP); 	// static link (SL)
        STACK[SP + 3]  = BP;			    // dynamic link (DL)
        STACK[SP + 4]  = PC;	 		    // return address (RA)
        oldBP1 = BP;
        BP = SP + 1;
	    PC = IR.m;

	    if(lvl1==0)
            lvl1 = BP;
        else if(lvl2==0)
            lvl2 = BP;
        else
            lvl3 = BP;

    }

    //INC
    else if(IR.op == 6){
        SP = SP + IR.m;
    }

    //JMP
    else if(IR.op == 7){
        PC = IR.m;
    }

    //JPC
    else if(IR.op == 8){
        if(STACK[SP] == 0){
            PC = IR.m;
        }
    }

    //SIO
    else if(IR.op == 9){
        SP--;
        printf("%d\n", STACK[SP]);
        SP--;
    }

    //SIO
    else if(IR.op == 10){
        SP++;
        scanf("%d", &readInValue);
    }

    else{
        //fprintf(ofp,"Ins not support!");
    }

    fprintf(ofp,"\t%d\t%d\t%d", PC, BP, SP);

    //print the stack contents when the SP is greater than 0, i.e. it has contents
    if(SP > 0){

        int s;
        fprintf(ofp,"\t");

        for(s=1; s<=SP; s++){

            fprintf(ofp,"%d ", STACK[s]);

            //if we have any activation records
            if(lvl1>0 || lvl2>0 || lvl3>0) {

                if(lvl1>0){
                    if(s == (lvl1-1)){
                        fprintf(ofp, "| ");
                    }

                }
                if(lvl2>0){
                    if(s == (lvl2-1)){
                        fprintf(ofp, "| ");
                    }

                }
                if(lvl2>0){
                    if(s == (lvl3-1)){
                        fprintf(ofp, "| ");
                    }

                }

            }

        }

        if(IR.op == 5){

            //fprintf(ofp, "| ");

            for(s=SP+1; s<=SP+4; s++){
                fprintf(ofp,"%d ", STACK[s]);
            }
        }

        fprintf(ofp,"\n");

    }

    else{
        fprintf(ofp,"\n");
    }

}



