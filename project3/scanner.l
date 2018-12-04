%{
#include <string.h>
#include "y.tab.h"

#define LIST                strcat(buf, yytext)
#define token(t)            {LIST; if (Opt_Token) printf("<%s>\n", #t);}
#define tokenInteger(t, i)  {LIST; if (Opt_Token) printf("<%s:%d>\n", t, i);}
#define tokenString(t, s)   {LIST; if (Opt_Token) printf("<%s:%s>\n", t, s);}
#define MAX_LINE_LENG 256
#define MAX_ID_LENG 256

int Opt_Source = 0;
int Opt_Token = 0;
int Opt_Statistic = 0;
int Opt_Symbol = 0;
int linenum = 1;
char buf[MAX_LINE_LENG];
char strContent[MAX_LINE_LENG];

typedef struct ids {
    char id_name[256];
    int freq;
    struct ids *next;
} id;
id *id_head = NULL;

void set(id **, char *);
id *new_id();
%}

%x PRAGMA
%x SOURCE
%x TOKEN
%x STATISTIC
%x SYMBOL
%x OTHER
%x STR
%x STR_ESCAPE
%x COMMENT

%%

<INITIAL>[\t ]+ {
    LIST;
}

^"#pragma" {
    LIST;
    BEGIN PRAGMA;
}

<PRAGMA>"source" {
    LIST;
    BEGIN SOURCE;
}

<PRAGMA>"token" {
    LIST;
    BEGIN TOKEN;
}

<PRAGMA>"statistic" {
    LIST;
    BEGIN STATISTIC;
}

<PRAGMA>"symbol" {
    LIST;
    BEGIN SYMBOL;
}

<PRAGMA>[\t ]+ {
    LIST;
}

<PRAGMA>. {
    printf("Error at pragma line %d: %s\n", linenum, yytext);
    exit(0);
}

<SOURCE>"on" {
    LIST;
    Opt_Source = 1;
    BEGIN OTHER;
}

<SOURCE>"off" {
    LIST;
    Opt_Source = 0;
    BEGIN OTHER;
}

<SOURCE>[\t ]+ {
    LIST;
}

<SOURCE>. {
    printf("Error at line %d: %s\n", linenum, yytext);
    exit(0);
}

<TOKEN>"on" {
    LIST;
    Opt_Token = 1;
    BEGIN OTHER;
}

<TOKEN>"off" {
    LIST;
    Opt_Token = 0;
    BEGIN OTHER;
}

<TOKEN>[\t ]+ {
    LIST;
}

<TOKEN>. {
    printf("Error at token line %d: %s\n", linenum, yytext);
    exit(0);
}

<STATISTIC>"on" {
    LIST;
    Opt_Statistic = 1;
    BEGIN OTHER;
}

<STATISTIC>"off" {
    LIST;
    Opt_Statistic = 0;
    BEGIN OTHER;
}

<STATISTIC>[\t ]+ {
    LIST;
}

<STATISTIC>. {
    printf("Error at line %d: %s\n", linenum, yytext);
    exit(0);
}

<SYMBOL>"on" {
    LIST;
    Opt_Symbol = 1;
    BEGIN OTHER;
}

<SYMBOL>"off" {
    LIST;
    Opt_Symbol = 0;
    BEGIN OTHER;
}

<SYMBOL>[\t ]+ {
    LIST;
}

<SYMBOL>. {
    printf("Error at line %d: %s\n", linenum, yytext);
    exit(0);
}

<OTHER>"[\t ]*" {
    LIST;
}

<OTHER>\/\/.* {
    LIST;
    BEGIN INITIAL;
}

<OTHER>"/*" {
    LIST;
    BEGIN COMMENT;
}

<OTHER>\n {
    LIST;
    if (Opt_Source)
        printf("%d:%s\n", linenum, buf);
    linenum++;
    buf[0] = '\0';
    BEGIN INITIAL;
}

<OTHER>[^ \t\n] {
    printf("Error at line %d: %s\n",linenum, yytext);
    exit(0);
}

"(" { tokenString("delim", "("); yylval.str=strdup(yytext); return L_PAREN; }
")" { tokenString("delim", ")"); yylval.str=strdup(yytext); return R_PAREN; }
"," { tokenString("delim", ","); yylval.str=strdup(yytext); return COMMA; }
";" { tokenString("delim", ";"); yylval.str=strdup(yytext); return SEMICOLON; }
"[" { tokenString("delim", "["); yylval.str=strdup(yytext); return ML_BRACE; }
"]" { tokenString("delim", "]"); yylval.str=strdup(yytext); return MR_BRACE; }
"{" { tokenString("delim", "{"); yylval.str=strdup(yytext); return L_BRACE; }
"}" { tokenString("delim", "}"); yylval.str=strdup(yytext); return R_BRACE; }

"+" { token("+"); yylval.str=strdup(yytext); return ADD_OP; }
"-" { token("-"); yylval.str=strdup(yytext); return SUB_OP; }
"*" { token("*"); yylval.str=strdup(yytext); return MUL_OP; }
"/" { token("/"); yylval.str=strdup(yytext); return DIV_OP; }
"%" { token("%"); yylval.str=strdup(yytext); return MOD_OP; }
"=" { token("="); yylval.str=strdup(yytext); return ASSIGN_OP; }
"<" { token("<"); yylval.str=strdup(yytext); return LT_OP; }
"<=" { token("<="); yylval.str=strdup(yytext); return LE_OP; }
"!=" { token("!="); yylval.str=strdup(yytext); return NE_OP; }
">=" { token(">="); yylval.str=strdup(yytext); return GE_OP; }
">" { token(">"); yylval.str=strdup(yytext); return GT_OP; }
"==" { token("=="); yylval.str=strdup(yytext); return EQ_OP; }
"&&" { token("&&"); yylval.str=strdup(yytext); return AND_OP; }
"||" { token("||"); yylval.str=strdup(yytext); return OR_OP; }
"!"	{ token("!"); yylval.str=strdup(yytext); return NOT_OP; }

"read" { tokenString("KW", yytext); return READ; }
"boolean" { tokenString("KW", yytext); yylval.str = strdup("bool"); return BOOLEAN; }
"while" { tokenString("KW", yytext); return WHILE; }
"do" { tokenString("KW", yytext); return DO; }
"if" { tokenString("KW", yytext); return IF; }
"else" { tokenString("KW", yytext); return ELSE; }
"true" { tokenString("KW", yytext); return TRUE; }
"false" { tokenString("KW", yytext); return FALSE; }
"for" { tokenString("KW", yytext); return FOR; }
"int" { tokenString("KW", yytext); yylval.str = strdup("int"); return INT; }
"print" { tokenString("KW", yytext); return PRINT; }
"bool" { tokenString("KW", yytext); yylval.str = strdup("bool"); return BOOL; }
"void" { tokenString("KW", yytext); yylval.str = strdup("void"); return VOID; }
"float" { tokenString("KW", yytext); yylval.str = strdup("float"); return FLOAT; }
"double" { tokenString("KW", yytext); yylval.str = strdup("double"); return DOUBLE; }
"string" { tokenString("KW", yytext); yylval.str = strdup("string"); return STRING; }
"continue" { tokenString("KW", yytext); return CONTINUE; }
"break" { tokenString("KW", yytext); return BREAK; }
"return" { tokenString("KW", yytext); return RETURN; }
"const" { tokenString("KW", yytext); return CONST; }

-?0 |
[1-9][0-9]* {
    tokenString("integer", yytext);
    yylval.inum = atoi(yytext);
    return INT_CONST;
}

-?[0-9]+\.[0-9]+ {
    tokenString("float", yytext);
    yylval.dnum = atof(yytext);
    return FLOAT_CONST;
}

-?([0-9]+|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+) {
    tokenString("scientific", yytext);
    yylval.str = strdup(yytext);
    return SCIENTIFIC;
}

"\"" {
    LIST;
    memset(strContent, 0, MAX_LINE_LENG);
    BEGIN STR;
}

<STR>"\\" {
    LIST;
    BEGIN STR_ESCAPE;
}

<STR_ESCAPE>\" {
    LIST;
    strcat(strContent, "\"");
    BEGIN STR;
}

<STR_ESCAPE>n {
    LIST;
    strcat(strContent, "\n");
    BEGIN STR;
}

<STR_ESCAPE>t {
    LIST;
    strcat(strContent, "\t");
    BEGIN STR;
}

<STR_ESCAPE>\\ {
    LIST;
    strcat(strContent, "\\");
    BEGIN STR;
}

<STR_ESCAPE>[^\"nt\\] {
    LIST;
    strcat(strContent, "\\");
    strcat(strContent, yytext);
    BEGIN STR;
}

<STR>"\n" {
    printf("Error at line %d: newline\n", linenum);
    exit(0);
}

<STR>\" {
    //LIST;
    //strcat(buf, "\"");
    tokenString("string", strContent);
    yylval.str = strdup(strContent);
    BEGIN INITIAL;
    return STR_CONST;
}

<STR>[^\\\n\"] {
    LIST;
    strcat(strContent, yytext);
}

"/*" {
    LIST;
    BEGIN COMMENT;
}

<COMMENT>"\n" {
    LIST;
    if (Opt_Source)
        printf("%d:%s\n", linenum, buf);
    linenum++;
    memset(buf, 0, MAX_LINE_LENG);
}

<COMMENT>. {
    LIST;
}

<COMMENT>"*/" {
    LIST;
    BEGIN INITIAL;
}

\/\/.* {
    LIST;
}

\n {
    LIST;
    if (Opt_Source)
        printf("%d:%s\n", linenum, buf);
    linenum++;
    buf[0] = '\0';
}

[a-zA-Z_][a-zA-Z0-9_]{0,255} {
    char *idname;
    if (strlen(yytext) >= 32) {
        idname = (char*) malloc(sizeof(char) * 33);
        strncpy(idname, yytext, 32);
        idname[32] = '\0';
    }
    else {
        idname = strdup(yytext);
    }
    yylval.str = idname;

    if (Opt_Statistic) {
        set(&id_head, idname);
    }
    tokenString("id", idname);
    return ID;
}

. {
    printf("Error at line %d: %s\n", linenum, yytext);
    exit(0);
}

%%

/*
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./scanner [filename]\n");
        exit(0);
    }
  
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yyin = fp;
    id_head = NULL;
    yylex();

    if (Opt_Statistic) {
        id *offset = id_head;
        while (offset) {
            printf("%s\t%d\n", offset->id_name, offset->freq);
            offset = offset->next;
        }
    }
    
    exit(0);
}
*/

void set(id **head, char *yytext) {
    id *offset = *head;
    id *prev = NULL;
    if (offset == NULL) {
        *head = new_id();
        strncpy((*head)->id_name, yytext,strlen(yytext));
        (*head)->freq = 1;
    }
    else {
        while (offset) {
            if(!strncmp(offset->id_name, yytext, (strlen(yytext) >= strlen(offset->id_name) ? strlen(yytext) : strlen(offset->id_name)))) {
                offset->freq+=1;
                break;
            }
            prev = offset;
            offset = offset -> next;
        }
        if (!offset) {
            prev->next = new_id();
            strncpy(prev->next->id_name, yytext, strlen(yytext));
            prev->next->freq += 1;
        }
    }
}

id *new_id() {
    id *temp = (id *)malloc(sizeof(id));
    temp->next = NULL;
    memset(temp->id_name, 0, 256);
    temp->freq = 0;
    return temp;
}
