TARGET = compiler
OBJECT = lex.yy.c y.tab.c y.tab.o lex.yy.o y.output compiler y.tab.h main.o symtable.o codeGenerator.o
CC = gcc -g -O0
LEX = lex
YACC = yacc -v
YACCFLAG = -d
LIBS = -lfl -ly

all: $(TARGET)

compiler: y.tab.o lex.yy.o main.o symtable.o codeGenerator.o
	$(CC) -o $(TARGET) y.tab.o lex.yy.o main.o symtable.o codeGenerator.o $(LIBS)

y.tab.o: y.tab.c
	$(CC) -c y.tab.c

y.tab.c: parser.y symtable.h datatype.h codeGenerator.h
	$(YACC) $(YACCFLAG) parser.y

lex.yy.o: lex.yy.c
	$(CC) -c lex.yy.c

lex.yy.c: scanner.l datatype.h
	$(LEX) scanner.l

main.o: main.c symtable.h datatype.h codeGenerator.h
	$(CC) -c main.c

symtable.o: symtable.c symtable.h datatype.h
	$(CC) -c symtable.c

codeGenerator.o: codeGenerator.c codeGenerator.h datatype.h
	$(CC) -c codeGenerator.c

clean:
	rm -f $(TARGET) $(OBJECT)

