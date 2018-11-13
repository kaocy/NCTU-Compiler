
LEX = lex
YACC = yacc -d -v
CC = gcc
LIBS = -lfl -ly
RM = rm -f

TARGET = parser
OBJECT = lex.yy.c y.tab.h y.tab.c y.output

all: lex.yy.c y.tab.c
	$(CC) -o $(TARGET) lex.yy.c y.tab.c $(LIBS)

y.tab.c: parser.y
	$(YACC) parser.y

lex.yy.c: scanner.l
	$(LEX) scanner.l

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJECT)
