/**
 * LinkedList.c
 *
 * Computer Science 223
 * 
 * Addison Hu
 * addison.hu@yale.edu
 *
 * Implementation of headed linked list.
 * 
 **/
#include <stdbool.h>
#include <stdlib.h>

/** 
 * Struct: Triple
 * ~~~~~~~~~~~~~~
 * Defined for the purposes of pancake.c.  Fully defines a configuration 
 * in the configuration graph produced by the bidirection breadth-first 
 * search of pancake.c.
 *
 * Note that "Triple" is a misnomer.  A fourth field was added to 
 * implement bi-direction BFS
 *
 * members
 * ~~~~~~~
 * config:  current configuration
 * prev:    predecessor to config (used as key to access its Triple)
 * len:     distance from root
 * int:     1 if root is GOAL; 0 if root is INITIAL
 **/
typedef struct Triple {
    char* config;
    char* prev;
    int len;
    int fromGoal;
} Triple;

/**
 * Struct: Node
 * ~~~~~~~~~~~~
 * Nodes in a the linked list.
 *
 * members
 * ~~~~~~~
 * key: character string included to implement hashtable.
 * triple: pointer to Triple (payload)
 * next: pointer to next node
 **/
typedef struct Node {
    char* key;
    Triple* triple;
    struct Node* next;
} Node;

/**
 * Struct: List
 * ~~~~~~~~~~~~
 * A struct containing a pointer to a single node, the head of the list.
 *
 * members
 * ~~~~~~~
 * head: pointer to head of linked list
 **/
typedef struct List {
    Node* head;
} List;

/* For full descriptions, please see LinkedList.c */
int createL(List* lst);

int appendL(List lst, Triple* triple);

int isEmptyL(List lst);

Triple* retrieveL(List lst, char* key);

int removeL(List lst, char* key);

int destroyL(List lst);




