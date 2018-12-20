#include <stdio.h>
#include <stdlib.h>
#include "datatype.h"
#include "symtable.h"

// from parser.y
extern int yyparse();
extern FILE* yyin;
extern struct SymTableList *symbolTableList;

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
        fprintf(stderr, "Usage: ./parser [filename]\n");
        exit(0);
    } 

    symbolTableList = (struct SymTableList*)malloc(sizeof(struct SymTableList));
    initSymTableList(symbolTableList);
    AddSymTable(symbolTableList); // global
    yyparse(); /* primary procedure of parser */

    destroySymTableList(symbolTableList);
    fprintf(stdout, "\n");
    fprintf(stdout, "|--------------------------------|\n");
    fprintf(stdout, "|  There is no syntactic error!  |\n");
    fprintf(stdout, "|--------------------------------|\n");
    exit(0);
}

