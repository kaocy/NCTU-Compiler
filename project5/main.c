#include <stdio.h>
#include <stdlib.h>
#include "datatype.h"
#include "symtable.h"
#include "codeGenerator.h"

// from parser.y
extern int yyparse();
extern FILE* yyin;
extern struct SymTableList *symbolTableList;
int error = 0;

FILE *fpout; // for Java bytecode file 

int main(int argc, char **argv) {

    if (argc == 1) {
        yyin = stdin;
    }
    else if (argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, "Open file error\n");
            exit(-1);
        }
        yyin = fp;
    }
    else {
        fprintf(stderr, "Usage: ./compiler [filename]\n");
        exit(0);
    }

    fpout = fopen("output.j", "w");
    if (fpout == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    symbolTableList = (struct SymTableList*)malloc(sizeof(struct SymTableList));
    initSymTableList(symbolTableList);
    AddSymTable(symbolTableList, false); // global
    programInitialization();

    yyparse(); /* primary procedure of parser */

    destroySymTableList(symbolTableList);

    if (!error) {
        fprintf(stdout, "\n");
        fprintf(stdout, "|---------------------------------------------|\n");
        fprintf(stdout, "|  There is no syntactic and semantic error!  |\n");
        fprintf(stdout, "|---------------------------------------------|\n");
    }
    // fprintf(stdout, "\n");
    // fprintf(stdout, "|--------------------------------|\n");
    // fprintf(stdout, "|  There is no syntactic error!  |\n");
    // fprintf(stdout, "|--------------------------------|\n");
    exit(0);
}

