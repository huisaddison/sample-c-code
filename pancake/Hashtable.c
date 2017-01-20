/**
 * Hashtable.c
 *
 * Computer Science 223
 * Homework 5
 *
 * Addison Hu
 * addison.hu@yale.edu
 *
 * Implementation of a hashtable backed by linked lists.
 *
 **/
#include <string.h>
#include "Hashtable.h"

static long hash(char* key, long size);

/**
 * Function: createH()
 * ~~~~~~~~~~~~~~~~~~~
 * Creates a table with size chains.
 *
 * inputs
 * ~~~~~~
 * dict: pointer to Hashtable struct
 * size: number of buckets to include in hashtable
 *
 * returns: status
 **/
int createH(Hashtable* dict, long size) {
    dict->size = size;
    dict->entry = malloc(dict->size * sizeof(List));
    memset(dict->entry, 0, sizeof(*(dict->entry)));
    for (int i = 0; i < dict->size; i++) {
        createL(&dict->entry[i]);
    }
    return true;
}

/**
 * Function: addH()
 * ~~~~~~~~~~~~~~~~
 * Adds an entry to the Hashtable if there does _not_ already exist 
 * an entry with the same key.
 *
 * input
 * ~~~~~
 * dict: Hashtable struct
 * key:  hash key
 * triple: payload to be stored
 *
 * returns: true if successfuly added, false if there already exists 
 *          entry with the same key.
 **/
int addH(Hashtable dict, char* key, Triple* triple) {
    long index = hash(key, dict.size);
    if (retrieveL(dict.entry[index], key) == NULL) {
        appendL(dict.entry[index], triple);
        return true;
    } else {
        return false;
    }
}

/**
 * Function: retrieveH()
 * ~~~~~~~~~~~~~~~~~~~~~
 * Retrieves the pointer to a triple struct associated with a 
 * given key.  
 *
 * Note that this calls the retrieveL() function implemented in 
 * LinkedList.c, which guarantees that if there is no entry 
 * associated with the given key, NULL is returned.
 *
 * input
 * ~~~~~
 * dict: Hashtable struct
 * key: hash key
 *
 * returns: pointer to triple struct if entry is present, NULL otherwise
 **/
Triple* retrieveH(Hashtable dict, char* key) {
    long index = hash(key, dict.size);
    return retrieveL(dict.entry[index], key);
}

/**
 * Function: removeH()
 * ~~~~~~~~~~~~~~~~~~~
 * Removes an entry from the hashtable.  Does nothing if the entry 
 * does not exist.  
 *
 * See implementation of removeL() in LinkedList.c.
 *
 * input
 * ~~~~~
 * dict: Hashtable struct
 * key: hash key
 *
 * returns: staus
 **/
int removeH(Hashtable dict, char* key) {
    long index = hash(key, dict.size);
    removeL(dict.entry[index], key);
    return true;
}

/** 
 * Function: destroyH()
 * ~~~~~~~~~~~~~~~~~~~~
 * Frees all memory associated with the hashtable.
 *
 * input
 * ~~~~~
 * dict: Hashtable struct
 *
 * returns: status
 **/
int destroyH(Hashtable dict) {
    for (int i = 0; i < dict.size; i++) {
        destroyL(dict.entry[i]);
    }
    free(dict.entry);
    return true;
}

/**
 * Function: hash()
 * ~~~~~~~~~~~~~~~~
 * Hash function, written by Stanley C. Eisenstat<stanley.eisenstat@yale.edu>
 *
 * input
 * ~~~~~
 * key: hash key
 * size: size of hashtable
 *
 * returns: hash value
 **/
static long hash(char* key, long size) {
    unsigned long sum;
    int shift;
    const unsigned long prime = 3141592653589793239L;

    for (sum = 0, shift = 0; *key; key++) {
        sum ^= *key<<shift;
        shift += 7;
        if (shift >= 57) {
            shift -= 57;
        }
    }
    return ((prime * sum) % size);
}
