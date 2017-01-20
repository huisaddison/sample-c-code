CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -g3

HWK3= /c/cs223/Hwk3
HWK4= /c/cs223/Hwk4

all:	Words16
 
#####
# Instructions to make Words16
#####

Words16: Words16.c Tree.o ${HWK3}/getLine.o
	${CC} ${CFLAGS} -o $@ $^

Words16.o: ./Tree.h ${HWK3}/getLine.h 

