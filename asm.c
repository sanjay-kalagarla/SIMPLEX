/*
Name: <Sanjay Kalagarla>
UserID: <2401CS15>
CS2206 SIMPLEX Two Pass Assembler

Declaration
-----------
I confirm that the submitted work is entirely my own and has been
implemented according to the requirements specified in the
CS2206 MiniProject specification.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256
#define MAX_LABEL 64
#define MAX_SYMBOLS 2048
#define UNDEF_SYMBOL -999999

FILE *logfp=NULL;
int error_count=0;
int line_no=0;

typedef struct{
    char name[16];
    int opcode;
    int has_operand;
    int is_branch;
}Instruction;

typedef struct{
    char name[MAX_LABEL];
    int value;
    int used;
}Symbol;

Instruction inst_table[]={
{"ldc",0,1,0},{"adc",1,1,0},{"ldl",2,1,0},{"stl",3,1,0},
{"ldnl",4,1,0},{"stnl",5,1,0},{"add",6,0,0},{"sub",7,0,0},
{"shl",8,0,0},{"shr",9,0,0},{"adj",10,1,0},{"a2sp",11,0,0},
{"sp2a",12,0,0},{"call",13,1,1},{"return",14,0,0},
{"brz",15,1,1},{"brlz",16,1,1},{"br",17,1,1},{"HALT",18,0,0},
{"data",-1,1,0},{"SET",-2,1,0},{"",-99,0,0}
};

Symbol symtab[MAX_SYMBOLS];
int symcount=0;
int pc=0;

void log_error(const char *msg,const char *detail)
{
    if(logfp)
    {
        if(detail)
            fprintf(logfp,"Line %d: Error: %s %s\n",line_no,msg,detail);
        else
            fprintf(logfp,"Line %d: Error: %s\n",line_no,msg);
    }
    error_count++;
}
/* Read one source line. If the line is longer than the buffer the rest of
   it is discarded (so it is never parsed as a second line) and *truncated
   is set to 1. Returns 0 at end of file. */
int read_line(char *line,FILE *fp,int *truncated)
{
    int c;

    *truncated=0;

    if(!fgets(line,MAX_LINE,fp))
        return 0;

    if(strchr(line,'\n')==NULL && !feof(fp))
    {
        *truncated=1;

        while((c=getc(fp))!='\n' && c!=EOF)
            ;
    }

    return 1;
}
void remove_comment(char *line)
{
    char *p=strchr(line,';');
    if(p) *p='\0';
}
int valid_label(char *s)
{
    int i;

    if(!isalpha((unsigned char)s[0]))
        return 0;

    for(i=1;s[i];i++)
        if(!isalnum((unsigned char)s[i]))
            return 0;

    return 1;
}
void add_symbol(char *name,int value)
{
    int i;

    for(i=0;i<symcount;i++)
        if(strcmp(symtab[i].name,name)==0)
        {
            log_error("duplicate label",name);
            return;
        }

    if(symcount>=MAX_SYMBOLS)
    {
        log_error("symbol table overflow",name);
        return;
    }

    if(strlen(name)>=MAX_LABEL)
    {
        log_error("label too long",name);
        return;
    }

    strcpy(symtab[symcount].name,name);
    symtab[symcount].value=value;
    symtab[symcount].used=0;
    symcount++;
}
int lookup_symbol(char *name)
{
    int i;

    for(i=0;i<symcount;i++)
        if(strcmp(symtab[i].name,name)==0)
        {
            symtab[i].used=1;
            return symtab[i].value;
        }

    return UNDEF_SYMBOL;
}
int find_inst(char *name)
{
    int i=0;

    while(inst_table[i].opcode!=-99)
    {
        if(strcmp(inst_table[i].name,name)==0)
            return i;
        i++;
    }

    return -1;
}
int parse_number(char *s,long *result)
{
    char *end;
    long val;

    val=strtol(s,&end,0);

    if(*end!='\0')
    {
        log_error("invalid number",s);
        return 0;
    }

    *result=val;
    return 1;
}
void pass1(FILE *fp)
{
    char line[MAX_LINE];
    char *token;
    int truncated;

    pc=0;
    line_no=0;

    while(read_line(line,fp,&truncated))
    {
        int inst;
        char *op;
        char *extra;
        long val;
        char label[MAX_LABEL] = "";
        int has_label=0;

        line_no++;

        /* an over-long line is only a problem if code was cut off */
        if(truncated && strchr(line,';')==NULL)
            log_error("line too long",NULL);

        remove_comment(line);

        token=strtok(line," \t\r\n,");

        if(!token)
            continue;

        {
            char *colon=strchr(token,':');

            if(colon)
            {
                *colon='\0';

                if(!valid_label(token))
                    log_error("invalid label name",token);

                if(strlen(token)>=MAX_LABEL)
                {
                    log_error("label too long",token);
                    token[MAX_LABEL-1]='\0';
                }

                strcpy(label,token);
                has_label=1;

                token=colon+1;

                if(*token=='\0')
                    token=strtok(NULL," \t\r\n,");

                if(!token)
                {
                    add_symbol(label,pc);
                    continue;
                }
            }
        }

        inst=find_inst(token);

        if(inst<0)
        {
            log_error("unknown instruction",token);
            continue;
        }

        if(inst_table[inst].has_operand)
        {
            op=strtok(NULL," \t\r\n,");
            extra=strtok(NULL," \t\r\n,");

            if(!op)
            {
                log_error("missing operand for",token);
            }
            else
            {
                if(strcmp(token,"SET")==0)
                {
                    if(!has_label)
                        log_error("SET requires a label",NULL);
                    else if(parse_number(op,&val))
                        add_symbol(label,val);
                }
                else
                {
                    if(!isalpha((unsigned char)op[0]))
                        parse_number(op,&val);

                    if(has_label)
                        add_symbol(label,pc);
                }
            }

            if(extra)
                log_error("extra text at end of line",NULL);
        }
        else
        {
            op=strtok(NULL," \t\r\n,");

            if(op)
                log_error("extra operand for",token);

            if(has_label)
                add_symbol(label,pc);
        }

        if(strcmp(token,"SET")!=0)
            pc++;
    }
}
void pass2(FILE *src,FILE *obj,FILE *lst)
{
    char line[MAX_LINE];
    int truncated;

    pc=0;
    line_no=0;

    while(read_line(line,src,&truncated))
    {
        char *token;
        char *operand;
        int inst;
        long value=0;
        int machine;
        int is_label_operand=0;
        char label[MAX_LABEL] = "";

        line_no++;

        remove_comment(line);

        token=strtok(line," \t\r\n,");

        if(!token)
            continue;

        {
            char *colon=strchr(token,':');

            if(colon)
            {
                *colon='\0';

                if(strlen(token)<MAX_LABEL)
                    strcpy(label,token);

                token=colon+1;

                if(*token=='\0')
                    token=strtok(NULL," \t\r\n,");

                if(!token)
                {
                    /* label on its own line: show it in the listing */
                    if(label[0])
                        fprintf(lst,"%08X %s:\n",pc,label);
                    continue;
                }
            }
        }

        inst=find_inst(token);

        if(inst<0)
            continue;

        if(strcmp(token,"SET")==0)
            continue;

        if(label[0])
            fprintf(lst,"%08X %s:\n",pc,label);

        operand=strtok(NULL," \t\r\n,");

        if(inst_table[inst].has_operand)
        {
            if(operand)
            {
                if(isalpha((unsigned char)operand[0]))
                {
                    is_label_operand=1;
                    value=lookup_symbol(operand);

                    if(value==UNDEF_SYMBOL)
                    {
                        log_error("undefined label",operand);
                        value=0;
                    }
                }
                else
                {
                    value=strtol(operand,NULL,0);
                }
            }
        }

        /* a label operand of a branch becomes a displacement from the
           next instruction; a plain number is already an offset */
        if(inst_table[inst].is_branch && is_label_operand)
            value=value-pc-1;

        if(inst_table[inst].opcode==-1)
            machine=(int)value;
        else
        {
            if(inst_table[inst].has_operand &&
               (value<-8388608L || value>8388607L))
            {
                log_error("operand does not fit in 24 bits",operand);
                value=0;
            }
            machine=(int)(((unsigned long)value<<8)&0xFFFFFFFFUL)
                    |inst_table[inst].opcode;
        }

        fwrite(&machine,sizeof(int),1,obj);

        fprintf(lst,"%08X %08X %s",pc,machine,inst_table[inst].name);

        if(operand)
            fprintf(lst," %s",operand);

        fprintf(lst,"\n");

        pc++;
    }
}
int main(int argc,char *argv[])
{
    FILE *src,*obj,*lst;
    int i;

    char base[256];
    char objname[300];
    char lstname[300];
    char logname[300];

    if(argc!=2)
    {
        printf("Usage: asm file.asm\n");
        return 1;
    }

    src=fopen(argv[1],"r");

    if(!src)
    {
        printf("Cannot open source file\n");
        return 1;
    }

    strcpy(base,argv[1]);

    {
        char *dot=strrchr(base,'.');
        if(dot) *dot='\0';
    }

    sprintf(objname,"%s.obj",base);
    sprintf(lstname,"%s.lst",base);
    sprintf(logname,"%s.log",base);

    logfp=fopen(logname,"w");
    obj=fopen(objname,"wb");
    lst=fopen(lstname,"w");

    if(!logfp || !obj || !lst)
    {
        printf("Cannot create output files\n");
        return 1;
    }

    pass1(src);

    rewind(src);

    pass2(src,obj,lst);

    if(error_count>0)
    {
        fprintf(logfp,"\nTotal errors: %d\n",error_count);

        fclose(src);
        fclose(obj);
        fclose(lst);
        fclose(logfp);

        remove(objname);
        remove(lstname);

        printf("Assembly failed. See %s\n",logname);
        return 1;
    }

    fclose(src);
    fclose(obj);
    fclose(lst);

    for(i=0;i<symcount;i++)
        if(symtab[i].used==0)
            printf("[Warning]: label '%s' defined but not used\n",
                   symtab[i].name);

    fclose(logfp);
    remove(logname);

    printf("\nAssembly successful\n");
    printf("Created [%s] and [%s]\n",lstname,objname);

    return 0;
}