%{
#define LIST                { strncat(buf, yytext, sizeof(buf) - strlen(buf) - 1); }
#define token(t)            { LIST; if (Opt_Token) printf("<%s>\n", #t); }
#define tokenString(t, s)   { LIST; if (Opt_Token) printf("<%s:%s>\n", t, s); }
#define MAX_LINE_LENGTH 257
#define MAX_ID_LENGTH 257
int Opt_Source = 1;
int Opt_Token = 1;
int Opt_Statistic = 1;
int linenum = 1;
char buf[MAX_LINE_LENGTH];

struct Node {
    char identifier[MAX_ID_LENGTH];
    int frequency;
    struct Node *next;
};

struct Node* head = NULL;
struct Node* tail = NULL;
void find(struct Node*, char*);
struct Node* create(char*);
struct Node* push(struct Node*, struct Node*);
void print(struct Node*);
%}

space       [ \t]+
delimiter   [,;\(\)\[\]\{\}]
keyword     ("while"|"do"|"if"|"else"|"true"|"false"|"for"|"int"|"print"|"const"|"read"|"boolean"|"bool"|"void"|"float"|"double"|"string"|"continue"|"break"|"return")
digit       [0-9]
letter      [a-zA-z_]
identifier  {letter}({digit}|{letter})*
integer     (0|[1-9]{digit}*)
float       {digit}+\.{digit}+
scientific  ({digit}+|{float})[eE][\+\-]?{digit}+

%s PRAGMA
%s PRAGMA_DONE
%s COMMENT

%%
    /* for pragma source */
<PRAGMA>"source"{space}+"on" {
    LIST;
    Opt_Source = 1;
    BEGIN PRAGMA_DONE;
}

<PRAGMA>"source"{space}+"off" {
    LIST;
    Opt_Source = 0;
    BEGIN PRAGMA_DONE;
}

    /* for pragma token */
<PRAGMA>"token"{space}+"on" {
    LIST;
    Opt_Token = 1;
    BEGIN PRAGMA_DONE;
}

<PRAGMA>"token"{space}+"off" {
    LIST;
    Opt_Token = 0;
    BEGIN PRAGMA_DONE;
}

    /* for pragma statistic */
<PRAGMA>"statistic"{space}+"on" {
    LIST;
    Opt_Statistic = 1;
    BEGIN PRAGMA_DONE;
}

<PRAGMA>"statistic"{space}+"off" {
    LIST;
    Opt_Statistic = 0;
    BEGIN PRAGMA_DONE;
}

    /* handle character after pragma directive */
<PRAGMA_DONE>(("//")(.)*)?"\n" {
    LIST;
    if (Opt_Source) printf("%d:%s\n", linenum, buf);
    ++linenum;
    buf[0] = '\0';
    BEGIN 0;
}

<PRAGMA_DONE>"/*" {
    LIST;
    BEGIN COMMENT;
}

    /* handle comment */
<COMMENT>. {
    LIST;
}

<COMMENT>"*/" {
    LIST;
    BEGIN 0;
}

<INITIAL>{delimiter} { tokenString("delim", yytext); }
<INITIAL>{keyword} { tokenString("KW", yytext); }
<INITIAL>{identifier} { tokenString("id", yytext); find(head, yytext); }
<INITIAL>{integer} { tokenString("integer", yytext); }
<INITIAL>{float} { tokenString("float", yytext); }
<INITIAL>{scientific} { tokenString("scientific", yytext); }

<INITIAL>"+" { token("+"); }
<INITIAL>"-" { token("-"); }
<INITIAL>"*" { token("*"); }
<INITIAL>"/" { token("/"); }
<INITIAL>"%" { token("%"); }
<INITIAL>"=" { token("="); }
<INITIAL>"<" { token("<"); }
<INITIAL>"<=" { token("<="); }
<INITIAL>"!=" { token("!="); }
<INITIAL>">=" { token(">="); }
<INITIAL>">" { token(">"); }
<INITIAL>"==" { token("=="); }
<INITIAL>"&&" { token("&&"); }
<INITIAL>"||" { token("||"); }
<INITIAL>"!" { token("!"); }

^"#pragma" {
    LIST;
    BEGIN PRAGMA;
}

"/*" {
    LIST;
    BEGIN COMMENT;
}

"//"(.)*?"\n" {
    LIST;
    if (Opt_Source) printf("%d:%s\n", linenum, buf);
    ++linenum;
    buf[0] = '\0';
}

    /* handle string*/
\"(\\.|[^"\\])*\" {
    char tmp[yyleng];
    int i = 0, j = 0;
    while (i < yyleng) {
        if (yytext[i] == '\\') {
            if (yytext[i+1] == '\"'|| yytext[i+1] == '\\') i++;
        }
        tmp[j++] = yytext[i++];
    }
    tmp[j-1] = '\0';
    char *real = &tmp[1];

    tokenString("string", real);
}

\n {
    LIST;
    if (Opt_Source) printf("%d:%s\n", linenum, buf);
    ++linenum;
    buf[0] = '\0';
}

{space} {
    LIST;
}

. { // Unrecognized character
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}

%%
void find(struct Node* head, char* identifier) {
    while (head != NULL) {
        if (!strcmp(head->identifier, identifier)) {
            head->frequency++;
            return ;
        }
        head = head->next;
    }
    tail = push(tail, create(identifier));
}

struct Node* create(char* identifier) {
    struct Node* res = (struct Node*) malloc(sizeof(struct Node));
    strcpy(res->identifier, identifier);
    res->frequency = 1;
    return res;
}

struct Node* push(struct Node* tail, struct Node* item) {
    if (tail == NULL) {
        head = tail = item;
    }
    else {
        tail->next = item;
        item->next = NULL;
        tail = tail->next;
    }
    return tail;
}

void print(struct Node* head) {
    while (head != NULL) {
        printf("%s\t%d\n", head->identifier, head->frequency);
        head = head->next;
    }
}

int main( int argc, char **argv ) {
    if (argc >= 2) {
        yyin = fopen( argv[1], "r" );
        if ( NULL == yyin ) {
            fprintf( stderr, "Cannot open: %s\n", argv[1] );
            exit(-1);
        }
    }
    else {
        yyin = stdin;
    }

    yylex();

    if (Opt_Statistic) {
        // Print frequency table
        printf("frequencies of identifiers:\n");
        print(head);
    }

    exit(0);
}