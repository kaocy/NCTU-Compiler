all:lex.yy.c
	gcc  -o scanner lex.yy.c -lfl
lex.yy.c:scanner.l
	lex scanner.l
clean:
	rm -f lex.yy.c