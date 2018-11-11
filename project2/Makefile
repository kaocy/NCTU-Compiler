#makefile for scanner
LEX = lex
LEXFN = lex.l
CC = gcc
LIBS = -lfl
RM = rm -f
SCANNERN = scanner
SCANNERSN = scanner.c

all: scanner

scanner: lex
	$(CC) -o $(SCANNERN) $(SCANNERSN) $(LIBS)

lex:
	$(LEX) -o $(SCANNERSN) $(LEXFN)

clean:
	$(RM) $(SCANNERN)
	$(RM) $(SCANNERSN)
