/**
 * Tree.c
 *
 * Addison Hu
 * addison.hu@yale.edu
 *
 * An implementation of a WEPL-Balanced tree that stores character key - 
 * integer count pairs
 *
 * Original attribution belongs to Stanley C. Eisenstat.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tree.h"

/**
 * Struct: tree
 * ~~~~~~~~~~~~
 * Data encapsulation of a tree node.
 *
 * members
 * ~~~~~~~
 * key: pointer to character key sequence
 * count: count associated with key
 * leaf: 1 if node is a leaf; 0 otherwise
 * wt: weight associated with this node
 * wepl: weighted external path length of this subtree
 * bereaved: 1 if key is not in any leaf node; 0 otherwise 
 * left: pointer to left child
 * right: pointer to right child
 **/
struct tree {
    char* key;
    int leaf, wt, wepl, bereaved; 
    Tree left, right;
};

/**
 * Function: create()
 * ~~~~~~~~~~~~~~~~~~
 * Initializes Tree type to NULL.
 *
 * inputs
 * ~~~~~~
 *  - t: pointer to Tree type
 * 
 * returns: status
 **/
int create(Tree* t) {
    *t = NULL;
    return 0;
}


/**
 * Function: initLeaf()
 * ~~~~~~~~~~~~~~~~~~~~
 * Initializes values in node to defaults for a newly-created leaf.
 *
 * Called by increment().
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 *  - k: key string
 *  - count: count tied to key string k
 *
 * returns: nothing
 **/
static void initLeaf(Tree t, char* k, int count) {
    t->key = k;
    t->wt = count;
    t->leaf = 1;
    t->wepl = 0;
    t->bereaved = 0;
    t->left = NULL;
    t->right = NULL;
    return;
}

/**
 * Function: update()
 * ~~~~~~~~~~~~~~~~~~
 * Applied by increment() and delete9) as their recursive calls unwind. 
 * Updates wt (weight) and wepl (WEPL).  
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 *
 * returns: nothing
 **/
static void update(Tree t) {
    if (!t->leaf) {
        t->wt = (t->left)->wt + (t->right)->wt;
        t->wepl = (t->left)->wepl + (t->right)->wepl + t->wt;
    }
    return;
}

/**
 * Function: wDiff()
 * ~~~~~~~~~~~~~~~~~~~~
 * Returns the reduction in WEPL of the subtree rooted at t
 * if a left or right rotation is performed.
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 *  - left: indicator specifying test for left or right rotation
 *
 * returns: int, specifying hypothetical reduction in WEPL.
 **/
static int wDiff(Tree t, int left) {
    if (t->leaf) {
        return 0;
    } else if ((left && (t->right)->leaf) ||
               (!left && (t->left)->leaf)) {
        return 0;
    } else if (left) {
        return ((t->right)->right)->wt - (t->left)->wt;
    } else {
        return ((t->left)->left)->wt - (t->right)->wt;
    }
}

/** 
 * Function: rotate()
 * ~~~~~~~~~~~~~~~~~~
 * Performs a left or right rotation.
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 *  - left: indicator specifying whether left or right rotation to be done
 *
 * returns: Tree type
 **/
static Tree rotate(Tree t, int left) {
    if (t == NULL || t->leaf == 1) {
        return t;
    } else if (left) {
        if ((t->right)->left == NULL) {
            return t;
        } else {
            Tree p, q;
            p = (t->right)->left;
            (t->right)->left = t;
            q = t->right;
            t->right = p;
            update(q->left);
            update(q->right);
            return q;
        }
    } else {
        if ((t->left)->right == NULL) {
            return t;
        } else {
            Tree p, q;
            p = (t->left)->right;
            (t->left)->right = t;
            q = t->left;
            t->left = p;
            update(q->left);
            update(q->right);
            return q;
        }
    }
}

/**
 * Function: increment()
 * ~~~~~~~~~~~~~~~~~~~~~
 * Increments the count f or a leaf node associated with key k, 
 * or creates a leaf node if no such leaf node exists.
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 *  - k: key string
 *  - lim: Improvement factor, specified by user (default: 0)
 *
 * returns: Tree type
 **/
Tree increment(Tree t, char* k, int lim) {
    // If we are at an internal node, decide whether to go left or right and 
    // recurse
    if (t != NULL && !t->leaf) {
        if (strcmp(k, t->key) > 0) {
            t->right = increment(t->right, k, lim);
            if (wDiff(t, 1) > lim) {
                t = rotate(t, 1);
            }
        } else {
            t->left = increment(t->left, k, lim);
            if (wDiff(t, 0) > lim) {
                t = rotate(t, 0);
            }
        }
        update(t);
    // If we are at a leaf, increment if already in tree, 
    // else create new leaves
    } else if (t != NULL && t->leaf) {
       // increment weight (count) if key already is in tree
        if (strcmp(k, t->key) == 0) {
            t->wt++;
            free(k);
        } else {
            t->left = malloc(sizeof(struct tree));
            t->right = malloc(sizeof(struct tree));
            if (strcmp(k, t->key) > 0) {
                initLeaf(t->left, t->key, t->wt);
                initLeaf(t->right, k, 1);
            } else {
                initLeaf(t->left, k, 1);
                initLeaf(t->right, t->key, t->wt);
                t->key = k;
            }
            t->leaf = 0;
            update(t);
        }
    // case when tree does not exist
    } else {
        t = malloc(sizeof(struct tree));
        initLeaf(t, k, 1);
    }
    return t;
}

/**
 * Function: delete()
 * ~~~~~~~~~~~~~~~~~~
 * Deletes leaf node associated with key k if it is in tree, otherwise 
 * does nothing.
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 *  - k: key string
 *  - lim: Improvement factor (rotation threshold)
 *  - delKey: pointer to char*, to check whether removed key still exists 
 *      in tree (in which case it cannot be free()'d
 *  - delFlag: pointer to (Bool) int, specifying whether key can be 
 *      free()'d immediately
 *
 * returns: Tree type
 **/
Tree delete(Tree t, char* k, int lim, char** delKey, int* delFlag) {
    // Indicates whether tree has changed; if so, perform 
    // rotations as recursion unwinds.
    static int changed = 0;
    if (t != NULL && !t->leaf) {
        // Node if present is in right subtree
        if (strcmp(k, t->key) > 0) {
            // Continue recursing if children are not leaves
            if (!(t->right)->leaf) {
                t->right = delete(t->right, k, lim, delKey, delFlag);
                // If current node shares a key with the leaf that was 
                // removed, then make sure that we do _not_ free it
                if (*delKey == t->key) {
                    *delFlag = 0;
                    t->bereaved = 1;
                }
                update(t);
                if (changed && wDiff(t, 0) > lim) {
                    t = rotate(t, 0);
                }
            // Remove right and promote left if match
            } else if (strcmp((t->right)->key, k) == 0) {
                *delKey = (t->right)->key;
                Tree s;
                s = t->left;
                if ((t->right)->bereaved) {
                    free((t->right)->key);
                }
                if (t->bereaved) {
                    free(t->key);
                }
                free(t->right);
                free(t);
                t = s;
                changed = 1;
            } else {
                changed = 0;
            }
        // Node if present is in left subtree
        } else {
            if (!(t->left)->leaf) {
                t->left = delete(t->left, k, lim, delKey, delFlag);
                // If current node shares a key with the leaf that was 
                // removed, then make sure that we do _not_ free it
                if (*delKey == t->key) {
                    *delFlag = 0;
                    t->bereaved = 1;
                }
                update(t);
                if (changed && wDiff(t, 1) > lim) {
                    t = rotate(t, 1);
                }
            // Remove left and promote right if match
            } else if (strcmp((t->left)->key, k) == 0) {
                *delKey = (t->left)->key;
                Tree s;
                s = t->right;
                if ((t->left)->bereaved) {
                    free((t->left)->key);
                }
                if (t->bereaved) {
                    free(t->key);
                }
                free(t->left);
                free(t);
                t = s;
                changed = 1;
            } else {
                changed = 0;
            }
        }
        update(t);
        return t;
    // handle case of single node tree
    } else if (t != NULL && t->leaf && strcmp(k, t->key) == 0) {
        free(t);
        return NULL;
    } else if (t == NULL) {
        return t;
    } else {
        return t;
    }
}

/**
 * Function: dump()
 * ~~~~~~~~~~~~~~~~
 * Dumps keys in tree to stdout in preorder.
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 *
 * returns: status
 **/
int dump(Tree t) {
    if (t != NULL) {
        printf("%s\n", t->key);
        if (!t->leaf) {
            dump(t->left);
            dump(t->right);
        }
        return 1;
    } else {
        return 0;
    }
}

/**
 * Function: printPairs()
 * ~~~~~~~~~~~~~~~~~~~~~~
 * Prints key-weight pairs to stdout in inorder.
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 *
 * returns: status
 **/
int printPairs(Tree t) {
    if (t != NULL && !t->leaf) {
        printPairs(t->left);
        printPairs(t->right);
        return 1;
    } else if (t != NULL && t->leaf) {
        printf("%3d %s\n", t->wt, t->key);
        return 1;
    } else {
        return 0;
    }
}

/**
 * Function: printEPL()
 * ~~~~~~~~~~~~~~~~~~~~
 * Prints total weight and weighted external path length of tree t to stdout.
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 *
 * returns: status
 **/
int printEPL(Tree t) {
    if (t != NULL) {
        printf("%d, %d\n", t->wt, t->wepl);
        return 1;
    } else {
        printf("0, 0\n");
        return 0;
    }
}

/**
 * Function: destroy()
 * ~~~~~~~~~~~~~~~~~~~
 * Destroys tree t and frees all memory still used by it.
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 * 
 * returns: Tree type
 **/
Tree destroy(Tree t) {
    if (t != NULL) {
        if (!t->leaf) {
            t->left = destroy(t->left);
            t->right = destroy(t->right);
            // Also free key if it is still in the tree only for 
            // searching purposes (i.e., not in any leaves)
            // This prevents double-freeing
            if (t->bereaved) {
                free(t->key);
            }
        } else {
            // Free all keys stored in leaves
            free(t->key);
        }
        free(t);
        return NULL;
    } else {
        return t;
    }
}
