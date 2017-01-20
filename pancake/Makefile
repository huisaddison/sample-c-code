CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -g3

HWK4= /c/cs223/Hwk4
HWK5= /c/cs223/Hwk5

all:	pancake
 
#####
# Instructions to make Merge16
#####

pancake: pancake.o Hashtable.o LinkedList.o ${HWK5}/Queue.o
	${CC} ${CFLAGS} -o $@ $^ 

pancake.o: ./Hashtable.h ${HWK4}/Queue.h
Hashtable.o: ./LinkedList.h
Queue.o: ${HWK4}/Queue.h

