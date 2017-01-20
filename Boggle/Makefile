CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -g3

HWK3= /c/cs223/Hwk3

all:	Boggle
 
#####
# Instructions to make Boggle
#####

Boggle: Boggle.c ${HWK3}/getLine.o
	${CC} ${CFLAGS} -o $@ $^

Boggle.o: ${HWK3}/getLine.h 

