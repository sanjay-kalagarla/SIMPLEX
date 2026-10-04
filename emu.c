/*
Name: Sanjay Kalagarla
UserID: 2401CS15
CS2206 SIMPLEX Emulator

Declaration
-----------
I confirm that the submitted work is entirely my own and has been
implemented according to the requirements specified in the
CS2206 MiniProject specification.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMSIZE 65536
#define MAX_STEPS 100000

int memory[MEMSIZE];

int A = 0;
int B = 0;
int PC = 0;
int SP = 0;

FILE *logfile;

/* Initialize memory */
void init_memory(void)
{
    int i;

    for(i=0;i<MEMSIZE;i++)
        memory[i] = 0;
}
/* Load object program */
void load_program(char *filename)
{
    FILE *fp;
    int i = 0;

    fp = fopen(filename,"rb");

    if(!fp)
    {
        printf("Error: cannot open object file\n");
        exit(1);
    }

    while(i < MEMSIZE && fread(&memory[i],sizeof(int),1,fp)==1)
        i++;

    fclose(fp);

    fprintf(logfile,"Program loaded: %d words\n",i);
}
/* Safe memory read */
int read_mem(int addr)
{
    if(addr < 0 || addr >= MEMSIZE)
    {
        fprintf(logfile,"ERROR: memory read out of bounds (%d)\n",addr);
        printf("Memory read error\n");
        exit(1);
    }

    return memory[addr];
}
/* Safe memory write */
void write_mem(int addr,int value)
{
    if(addr < 0 || addr >= MEMSIZE)
    {
        fprintf(logfile,"ERROR: memory write out of bounds (%d)\n",addr);
        printf("Memory write error\n");
        exit(1);
    }

    memory[addr] = value;
}
/* Memory dump with addresses */
void dump_memory(int size)
{
    int i;

    fprintf(logfile,"\nMemory Dump\n");

    for(i = 0; i < size; i += 4)
    {
        fprintf(logfile,"0x%08X ", i);

        if(i < size)
            fprintf(logfile,"0x%08X ", memory[i]);

        if(i+1 < size)
            fprintf(logfile,"0x%08X ", memory[i+1]);

        if(i+2 < size)
            fprintf(logfile,"0x%08X ", memory[i+2]);

        if(i+3 < size)
            fprintf(logfile,"0x%08X ", memory[i+3]);

        fprintf(logfile,"\n");
    }
}
/* Execute program */
void execute(void)
{
    int instr;
    int opcode;
    int operand;

    int running = 1;
    int steps = 0;

    fprintf(logfile,"\nExecution Start\n");

    while(running)
    {

        if(steps++ > MAX_STEPS)
        {
            fprintf(logfile,"ERROR: possible infinite loop\n");
            printf("Program stopped: infinite loop\n");
            break;
        }

        if(PC < 0 || PC >= MEMSIZE)
        {
            fprintf(logfile,"ERROR: PC out of bounds (%d)\n",PC);
            printf("PC out of bounds\n");
            break;
        }

        instr = memory[PC];
        PC++;

        opcode = instr & 0xFF;

        /* take the upper 24 bits without relying on signed shifts,
           then sign extend them */
        operand = (int)(((unsigned int)instr >> 8) & 0xFFFFFF);
        if(operand & 0x800000)
            operand -= 0x1000000;

        fprintf(logfile,
        "PC=%d A=%d B=%d SP=%d OPCODE=%d OPERAND=%d\n",
        PC-1,A,B,SP,opcode,operand);


        switch(opcode)
        {

        case 0: /* ldc */
            B = A;
            A = operand;
            break;

        case 1: /* adc */
            A = A + operand;
            break;

        case 2: /* ldl */
            B = A;
            A = read_mem(SP + operand);
            break;

        case 3: /* stl */
            write_mem(SP + operand,A);
            A = B;
            break;

        case 4: /* ldnl */
            A = read_mem(A + operand);
            break;

        case 5: /* stnl */
            write_mem(A + operand,B);
            break;

        case 6: /* add */
            A = B + A;
            break;

        case 7: /* sub */
            A = B - A;
            break;

        case 8: /* shl */
            if(A < 0 || A > 31)
            {
                fprintf(logfile,"ERROR: bad shift count %d\n",A);
                printf("Illegal shift\n");
                running = 0;
                break;
            }
            A = (int)((unsigned int)B << A);
            break;

        case 9: /* shr */
            if(A < 0 || A > 31)
            {
                fprintf(logfile,"ERROR: bad shift count %d\n",A);
                printf("Illegal shift\n");
                running = 0;
                break;
            }
            A = B >> A;
            break;

        case 10: /* adj */
            SP = SP + operand;
            break;

        case 11: /* a2sp */
            SP = A;
            A = B;
            break;

        case 12: /* sp2a */
            B = A;
            A = SP;
            break;

        case 13: /* call */
            B = A;
            A = PC;
            PC = PC + operand;
            break;

        case 14: /* return */
            PC = A;
            A = B;
            break;

        case 15: /* brz */
            if(A == 0)
                PC = PC + operand;
            break;

        case 16: /* brlz */
            if(A < 0)
                PC = PC + operand;
            break;

        case 17: /* br */
            PC = PC + operand;
            break;

        case 18: /* HALT */
            fprintf(logfile,"HALT instruction encountered\n");
            running = 0;
            break;

        default:
            fprintf(logfile,"ERROR: illegal opcode %d\n",opcode);
            printf("Illegal instruction\n");
            running = 0;
        }
    }

    fprintf(logfile,"\nExecution Finished\n");
}

int main(int argc,char *argv[])
{
    char logname[300];
    char base[256];
    char *dot;

    int dump_size = 64;

    if(argc!=2 && argc!=3)
    {
        printf("Usage: emu file.obj [words_to_dump]\n");
        return 1;
    }

    if(argc==3)
        dump_size = atoi(argv[2]);

    if(dump_size < 0 || dump_size > MEMSIZE)
        dump_size = 64;

    strcpy(base,argv[1]);

    dot = strrchr(base,'.');
    if(dot) *dot = '\0';

    sprintf(logname,"%s.log",base);

    logfile = fopen(logname,"w");

    if(!logfile)
    {
        printf("Cannot create log file\n");
        return 1;
    }

    init_memory();

    load_program(argv[1]);

    execute();

    fprintf(logfile,"\nFinal Registers\n");
    fprintf(logfile,"A=%d B=%d PC=%d SP=%d\n",A,B,PC,SP);

    dump_memory(dump_size);

    fclose(logfile);

    printf("Execution finished. Log written to %s\n",logname);

    return 0;
}