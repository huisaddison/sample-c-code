/**
 * Tree.h
 *
 * Addison Hu
 * addison.hu@yale.edu
 *
 * Specification of the interface for the Tree ADT that stores key-count 
 * pairs in a WEPL-balanced specification.
 *
 * For full function descriptions, please refer to Tree.c.
 *
 * Original attribution belongs to Stanley C. Eisenstat 
 * <stanley.eisenstat@yale.edu>
 **/

typedef struct tree* Tree;      // External definition of Tree

// Initializes tree 
int create(Tree* T);

// Inserts key into tree if not present; else increments key's count
Tree increment(Tree T, char* k, int lim);

// Removes leaf node associated with key
Tree delete(Tree T, char* k, int lim, char** delKey, int* delFlag);

// Dumps keys in preorder
int dump(Tree T);

// Prints key-count pairs in inorder
int printPairs(Tree T);

// Prints weight and WEPL of tree
int printEPL(Tree T);

// Frees all memory tied to tree
Tree destroy(Tree T);

