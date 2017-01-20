/**
 * Queue.c
 *
 * Computer Science 223
 * Homework 4
 *
 * Addison Hu
 * addison.hu@yale.edu
 *
 * An implementation of the Queue ADT as a headless, singly-linked, 
 * circular list.  The Queue data type is a pointer to the _last_ 
 * node in the list, or NULL if the queue is empty.
 **/

#include <stdlib.h>
#include "/c/cs223/Hwk4/Queue.h"

/** 
 * Struct: node
 * ~~~~~~~~~~~~
 * This structure represents a node in a singly-linked list.
 *
 * members
 * ~~~~~~~
 * - line: character string stored as data
 * - *next: pointer to the next node
 **/
typedef struct node {       
    char* line;
    struct node* next;
} Node;

/**
 * Function: createQ()
 * ~~~~~~~~~~~~~~~~~~~
 * Sets *q to a new object of type Queue.
 *
 * input
 * ~~~~~
 * - q: pointer to a queue
 * 
 * returns: true upon successful creation
 **/
int createQ(Queue* q) {
    *q = NULL; 
    return true;
}

/**
 * Function: addQ()
 * ~~~~~~~~~~~~~~~~
 * Adds the string pointer s to the tail of Queue *q.  
 *
 * inputs
 * ~~~~~~
 * - q: pointer to a queue
 * - s: pointer to a character string
 *
 * returns: true if successful, false otherwise
 **/ 
int addQ(Queue* q, char* s) {
    Node* new;
    new = malloc(sizeof(Node));
    if (new != NULL) {
        // store line in new node
        new->line = s;
        // handle case when Queue is empty
        if (*q == NULL) { 
            *q = new;
            new->next = new;
        // otherwise point old tail to new node, 
        // and point new node to the head
        } else {
            Node* old = *q;
            Node* head = old->next;
            *q = new;
            old->next = new;
            new->next = head;
        }
    // return false in malloc failed
    } else {
        return false; 
    }
    return true;
}
									    
/** 
 * Function: isEmptyQ()
 * ~~~~~~~~~~~~~~~~~~~~
 * Checks whether a queue is empty.
 *
 * input
 * ~~~~~
 * - q: pointer to a queue
 * 
 * returns: true if the queue is empty, false otherwise
 **/
int isEmptyQ(Queue* q) {
    if (*q == NULL) {
        return true;
    } else {
        return false;
    }
}

/**
 * Function: headQ()
 * ~~~~~~~~~~~~~~~~~
 * Copies the string pointer at the head of Queue *q to *s, but 
 * does not remove it from *q.
 *
 * inputs
 * ~~~~~~
 * - q: pointer at a queue
 * - *s: pointer to a character string
 *
 * returns: true if successful, false otherwise
 **/
int headQ (Queue* q, char** s) {
    // return false if queue is empty
    if (*q == NULL) {
        return false;
    } else {
        // get the head node via the last node
        Node* head = (*q)->next;
        *s = head->line;
    }
    return true;
}

/** 
 * Function: removeQ()
 * ~~~~~~~~~~~~~~~~~~~
 * Removes the string pointer at the head of the queue *q and stores 
 * it in *s.
 *
 * inputs
 * ~~~~~~
 * - q: pointer to a quee
 * - *s: pointer to a character string
 *
 * returns: true if successful, false otherwise
 **/
int removeQ(Queue* q, char** s) {
    // return false if queue is empty
    if (*q == NULL) {
        return false;
    } else {
        Node* tail = *q;
        Node* head = tail->next;
        // copy string pointer at head to *s
        *s = head->line;
        // point the tail to the node after the head if 
        // there is one
        if (head != tail) {
            tail->next = head->next;
        // otherwise, queue is empty, so set *q to NULL
        } else {
            *q = NULL;
        }
        free(head);
    }
    return true;
}

/** 
 * Function: destroyQ()
 * ~~~~~~~~~~~~~~~~~~~~
 * Destroys the Queue *q by freeing any storage it uses, and sets *q to NULL.
 *
 * input
 * ~~~~~
 * - q: pointer to a queue
 * 
 * returns: true if successful
 **/
int destroyQ (Queue* q) {
    // if queue is empty, return true
    if (*q == NULL) {
        return true;
    } else {
        Node* cur = (*q)->next;
        Node* prev;
        // loop through all the nodes, freeing them
        while (cur != *q) {
            prev = cur;
            cur = cur->next;
            free(prev);
        }
        free(*q);
        // set *q to NULL
        *q = NULL;
        return true;
    }
}
