#include <stdio.h>
#include <stdlib.h>
#include "symbolTable.h"

extern int yyparse();
extern FILE* yyin;
extern SymbolTable *table;
extern IdList *idlist_head, *idlist_tail;
extern ArraySignature *arraylist_head, *arraylist_tail;

int main(int argc, char **argv) {
    table = InsertTable(NULL, 0);
    idlist_head = idlist_tail = NULL;
    arraylist_head = arraylist_tail = NULL;

    if (argc == 1) {
        yyin = stdin;
    }
    else if (argc == 2) {
        FILE *fp = fopen( argv[1], "r" );
        if(fp == NULL) {
            fprintf(stderr, "Open file error\n");
            exit(-1);
        }
        yyin = fp;
    }
    else {
        fprintf(stderr, "Usage: ./parser [filename]\n");
        exit(0);
    } 

    yyparse(); /* primary procedure of parser */

    fprintf(stdout, "\n");
    fprintf(stdout, "|--------------------------------|\n");
    fprintf(stdout, "|  There is no syntactic error!  |\n");
    fprintf(stdout, "|--------------------------------|\n");
    exit(0);
}
