/**
 * Hashtable.h
 *
 * Computer Science 223
 * Homework 5
 *
 * Addison Hu
 * addison.hu@yale.edu
 *
 * Header file for Hashtable implementation.
 *
 **/
#include "LinkedList.h"

/**
 * Struct: Hashtable
 * ~~~~~~~~~~~~~~~~~
 *
 * Fully defines a hashtable.  Includes a pointer to a linked List struct, 
 * upon which pointer arithmetic may be performed to access different 
 * chains of the Hashtable.  
 *
 * members
 * ~~~~~~~
 * List* entry:     pointer to linked List
 * long size:       number of linked lists
 **/
typedef struct Hashtable {
    List* entry;
    long size;
} Hashtable;

/* See Hashtable.c for full explanations */
int createH(Hashtable* dict, long size);

int addH(Hashtable dict, char* key, Triple* triple);

Triple* retrieveH(Hashtable dict, char* key);

int removeH(Hashtable dict, char* key);

int destroyH(Hashtable dict);
