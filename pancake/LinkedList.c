/**
 * LinkedList.c
 *
 * Computer Science 223
 *
 * Addison Hu
 * addison.hu@yale.edu
 *
 * Implementation of a headed linked list.
 *
 **/
#include <string.h>
#include "LinkedList.h"

/**
 * Function: createL()
 * ~~~~~~~~~~~~~~~~~~~
 * Creates an instance of a linked list by setting the head 
 * to NULL.
 *
 * input
 * ~~~~~
 * lst: pointer to List struct
 * 
 * returns: status
 **/
int createL(List* lst) {
    lst->head = malloc(sizeof(Node));
    memset(lst->head, 0, sizeof(*(lst->head)));
    (lst->head)->next = NULL;
    return true;
}

/** 
 * Function: appendL()
 * ~~~~~~~~~~~~~~~~~~~
 * Creates a new load to store pointer to triple struct and appends it 
 * to the end of the linked list.
 *
 * input
 * ~~~~~
 * lst: List struct
 * triple: pointer to Triple type (payload)
 *
 * returns: status
 **/
int appendL(List lst, Triple* triple) {
    Node* prev = lst.head;
    Node* cur = (lst.head)->next;
    while (cur != NULL) {
        prev = cur;
        cur = cur->next;
    }
    prev->next = malloc(sizeof(Node));
    cur = prev->next;
    cur->key = triple->config;
    cur->triple = triple;
    cur->next = NULL;
    return true;
}

/**
 * isEmptyL()
 * ~~~~~~~~~~
 * Checks whether a linked list is empty.
 *
 * input
 * ~~~~~
 * lst: List struct
 *
 * returns: true if empty, false otherwise
 **/
int isEmptyL(List lst) {
    if ((lst.head)->next == NULL) {
        return true;
    } else {
        return false;
    }
}

/**
 * Function: retrieveL()
 * ~~~~~~~~~~~~~~~~~~~~~
 * Retrieves pointer to triple struct associated with a given key, 
 * if it is in the list.  
 *
 * input
 * ~~~~~
 * lst: List struct
 * key: char string associated with triple to be found
 *
 * returns: pointer to triple struct if found, NULL otherwise.
 **/
Triple* retrieveL(List lst, char* key) {
    Node* cur = (lst.head)->next;
    while (cur != NULL && strcmp(key, cur->key)) {
        cur = cur->next;
    }
    if (cur != NULL) {
        return cur->triple;
    } else {
        return NULL;
    }
}

/**
 * removeL()
 * ~~~~~~~~~
 * Removes the first node  found associated with a given key, if any are in 
 * the list, otherwise, does nothing.
 *
 * Assuming that a node with the same key is never entered twice, this 
 * guarantees that the linked list will not have any node with the given 
 * key after this function is invoked.
 *
 * input
 * ~~~~~
 * lst: List struct
 * key: char string associated with node to be found
 *
 * returns: status
 **/
int removeL(List lst, char* key) {
    Node* prev = lst.head;
    Node* cur = (lst.head)->next;
    while (cur != NULL && strcmp(key, cur->key)) {
        prev = cur;
        cur = cur->next;
    }
    if (cur != NULL) {
        prev->next = cur->next;
        free(cur);
    } 
    return true;
}

/**
 * Function: destroyL()
 * ~~~~~~~~~~~~~~~~~~~~
 * Frees all memory associated with the linked list, _including_ the memory
 * allocated to the triples to which the nodes point.  This design choice 
 * because the pointers between nodes give us an easy way to get memory 
 * addresses of all the blocks of memory allocated to triples.
 *
 * input
 * ~~~~~
 * lst: List struct
 *
 * returns: status
 **/
int destroyL(List lst) {
    Node* prev = lst.head;
    Node* cur = (lst.head)->next;
    while (cur != NULL) {
        free(prev->triple);
        free(prev);
        prev = cur;
        cur = cur->next;
    }
    free(prev->triple);
    free(prev);
    return true;
}
