/**
 * pancake.c
 *
 * Computer Science 223
 * Homework 5
 *
 * Addison Hu
 * addison.hu@yale.edu
 *
 * pancake is an implementation of generalized pancake sort.  Given 
 * two strings of printable characters, pancake determines whether 
 * there is a possible path of two-dimensional pancake flips from 
 * the INITIAL to the GOAL within MAX_LENGTH steps.
 *
 **/
#include <stdio.h>
#include <string.h>
#include "Hashtable.h"
#include "/c/cs223/Hwk4/Queue.h"

#define die(msg)    exit (fprintf (stderr, "%s\n", msg))
// Base 10 integers should be returned from strtol()
#define BASE 10 
// Minimum and maximum pancake dimensions
#define MINWH 1
#define MAXWH 16
// Minimum MAX_LENGTH
#define MINLEN 1
// Size of hashtable (number of chains)
#define CHAINS 100003

/**
 * Struct: Rule
 * ~~~~~~~~~~~~
 * A container that completely defines the parameters passed 
 * by the user with which pancake should be working.
 *
 * int height:  height of the pancake
 * int width:   width of the pancake
 * int maxlen:  maximum flips allowed from initial to goal
 * char* initial:   character string of initial configuration
 * char* goal:      character string of desired configuration
 **/
typedef struct Rule {
    int height;
    int width;
    int maxlen;
    char* initial;
    char* goal;
} Rule;

int parseArgs(Rule* rule, int argc, char* argv[]);
int initTriples(Triple* initial, Triple* goal, Rule rule);
char* flipPancake(char* old, int w, int h, int fw, int fh, int v);
char** getBatch(char* config, int w, int h);
void sort(char** unsorted, char** sorted, int len);


int main(int argc, char* argv[]) {
    // Parse command line arguments into a Rule struct 
    Rule rule;
    parseArgs(&rule, argc, argv);
    // Number of possible permutations possible, within one flip
    // of a configuration
    int nStep = 2 * (rule.width - 1) * (rule.height - 1) + (rule.width - 1) 
                + (rule.height - 1);
    
    // Queue to perform BFS
    Queue q;
    // Queue to hold a pointer to every block of memory malloc'd 
    // to store newly created character strings.
    Queue p;
    createQ(&q);
    createQ(&p);
    Hashtable h;
    createH(&h, CHAINS);

    // Create Triples that contain information about a configuration 
    // and a pointer to its predecessor.  Defined in LinkedList.c
    Triple goal;
    Triple initial;
    initTriples(&goal, &initial, rule);
    
    addQ(&q, goal.config);
    addQ(&q, initial.config);
    addH(h, goal.config, &goal);
    addH(h, initial.config, &initial);

    int found = 0;
    // Pointers to keep track of configurations as we encounter them
    char* curConfig;
    char* nextConfig;
    char* prevConfig;
    Triple* curTriple;
    Triple* nextTriple;
    Triple* dictTriple;
    char** batch;

    // Continue BFS until we have explored every node, or found a path 
    // between INITIAL and GOAL
    while (!isEmptyQ(&q) && !found) {
        removeQ(&q, &curConfig);
        curTriple = retrieveH(h, curConfig);
        if (curTriple == NULL) {
            die("failed");
        }
        // Get every possible configuration within one flip of the current 
        // configuration and enqueue them
        if (curTriple->len < rule.maxlen) {
            batch = getBatch(curTriple->config, rule.width, rule.height);
            // Store pointer to free at end of program
            for (int i = 0; i < nStep; i++) {
                addQ(&p, batch[i]);
            }
            for (int i = 0; i < nStep; i++) {
                nextConfig = batch[i];
                // If this is our first time seeing this configuration, 
                // add it to the queue to continue BFS
                if ((dictTriple = retrieveH(h, batch[i])) == NULL) {
                    nextTriple = malloc(sizeof(Triple));
                    nextTriple->config = batch[i];
                    nextTriple->prev = curTriple->config;
                    nextTriple->len = curTriple->len + 1;
                    // Inherit starting node from predecessor
                    nextTriple->fromGoal = curTriple->fromGoal;
                    addQ(&q, nextTriple->config);
                    addH(h, nextTriple->config, nextTriple);
                // If we _have_ seen this configuration before, that 
                // means that we came from the other side of 
                // bidirection BFS, so there is a path between 
                // INITIAL and GOAL!
                } else if (dictTriple->fromGoal != curTriple->fromGoal
                        && dictTriple->len + curTriple->len < rule.maxlen) {
                    found = 1;
                    break;
                }
            }
            // Free pointer to array of character pointers and start over
            free(batch);
        }
    }
    // Outputting if a solution was found
    if (found == 1) {
        // Array of character pointers to store the path to 
        // INITIAL (need to be printed in reverse order)
        char** path;
        int pathlen;
        // If the Triple we currently have a pointer to came from 
        // the GOAL, then we can follow the pointers and print 
        // the configuration out in order.  
        if (curTriple->fromGoal == 1) {
            path = malloc(sizeof(char*) * (dictTriple->len + 1));
            pathlen = dictTriple->len + 1;
            *path = dictTriple->config;
            // Store path to INITIAL
            while ((prevConfig = dictTriple->prev) != NULL) {
                path++;
                dictTriple = retrieveH(h, prevConfig);
                *path = dictTriple->config;
            }
        } else {
            // Store path to INITIAL
            path = malloc(sizeof(char*) * (curTriple->len + 1));
            pathlen = curTriple->len + 1;
            *path = curTriple->config;
            while ((prevConfig = curTriple->prev) != NULL) {
                path++;
                curTriple = retrieveH(h, prevConfig);
                *path = curTriple->config;
            }
            // Update pointer to point to path to 
            // GOAL
            curTriple = dictTriple;
        }
        // Output path to INITIAL in reverse order
        for (int i = 0; i < pathlen; i++) {
            printf("%s\n", *path);
            path--;
        } 
        free(++path);
        // Output path to GOAL
        while (curTriple->prev != NULL) {
            printf("%s\n", curTriple->config);
            curTriple = retrieveH(h, curTriple->prev);
        }
        printf("%s\n", curTriple->config);
    }


    // Free all blocks of character strings we allocated by 
    // flipPancake()
    while (!isEmptyQ(&p)) {
        removeQ(&p, &nextConfig);
        free(nextConfig);
    }
    // Remove configurations from dict that _weren't_ malloc'd
    removeH(h, goal.config);
    removeH(h, initial.config);
    // destroy dict, which frees payloads as well
    destroyH(h);
    destroyQ(&p);
    destroyQ(&q);
    return EXIT_SUCCESS;
}

/**
 * Function: parseArgs()
 * ~~~~~~~~~~~~~~~~~~~~~
 * Parses command line arguments into a Rule struct.
 *
 * inputs
 * ~~~~~~
 * rule: pointer to Rule struct
 * argc: number of arguments 
 * argv: array of argument strings
 *
 * returns: 0 upon success (no invalid input)
 **/
int parseArgs(Rule* rule, int argc, char* argv[]) {
    // Check for correct number of arguments
    if (argc != 4 && argc != 6) {
        die("pancake: pancake [HEIGHT WIDTH] MAXLENGTH INITIAL GOAL");
    }
    int curArg = 1;
    char* endptr;
    // If width and length are supplied, parse them
    if (argc == 6) {
        if ((rule->height = strtol(argv[curArg++], &endptr, BASE)) < MINWH || 
                rule->height > MAXWH) {
            die("pancake: Invalid HEIGHT");
        } else if (*endptr != '\0') {
            die ("pancake: Invalid HEIGHT");
        }
        if ((rule->width = strtol(argv[curArg++], &endptr, BASE)) < MINWH || 
                rule->width > MAXWH) {
            die("pancake: Invalid WIDTH");
        } else if (*endptr != '\0') {
            die("pancake: Invalid WIDTH");
        }
    // Otherwise, initialize to defaults
    } else {
        rule->height = 3;
        rule->width = 3;
    }
    // Check for argument validity
    if ((rule->maxlen = strtol(argv[curArg++], &endptr, BASE)) < MINLEN) {
        die("pancake: Invalid MAXLENGTH");
    } else if (*endptr != '\0') {
        die("pancake: Invalid MAXLENGTH"); 
    }
    if (strlen(rule->initial = argv[curArg++]) != rule->height * rule->width) {
        die("pancake: strlen(INITIAL) != HEIGHT*WIDTH");
    }
    if (strlen(rule->goal = argv[curArg]) != rule->height * rule->width) {
        die("pancake: strlen(GOAL) != HEIGHT*WIDTH");
    } 
    // Check whether INITIAL and GOAL are permutations
    // If not, quit immediately.  there is no possible path between them
    char* goalChars;
    char* initChars;
    goalChars = malloc(sizeof(char) * (rule->height * rule->width + 1));
    initChars = malloc(sizeof(char) * (rule->height * rule->width + 1));
    memset(goalChars, 0, sizeof(*goalChars));
    memset(initChars, 0, sizeof(*initChars));
    sort(&(rule->initial), &initChars, rule->height * rule->width);
    sort(&(rule->goal), &goalChars, rule->height * rule->width);
    if (strcmp(goalChars, initChars)) {
        die("pancake: GOAL != permutation of INITIAL");
    } 
    free(goalChars);
    free(initChars);
    return 0;
}

/**
 * Function: initTriples()
 * ~~~~~~~~~~~~~~~~~~~~~~~
 * Initalizes the INITIAL and GOAL Triple structs.
 *
 * inputs
 * ~~~~~~
 * initial: pointer to triple
 * goal: pointer to triple
 * rule: Rule struct specifying INITIAL and GOAL
 *
 * returns: 0 upon successful parsing
 **/
int initTriples(Triple* initial, Triple* goal, Rule rule) {
    goal->config = rule.goal;
    goal->prev = NULL;
    goal->len = 0;
    goal->fromGoal = 1;
    initial->config = rule.initial;
    initial->prev = NULL;
    initial->len = 0;
    initial->fromGoal = 0;
    return 0;
}

/**
 * Function: flipPancake()
 * ~~~~~~~~~~~~~~~~~~~~~~~
 * Given a character string, creates a horizontal or vertical "flip" 
 * permutation of it along a specified width and height.
 *
 * input
 * ~~~~~
 * old: character string of configuration to permute
 * w: width of old
 * h: height of old
 * fw: width of the flip, starting from upper left
 * fh: height of the flip, starting from upper left
 * v: 1 if vertical flip, 0 if horizontal flip
 *
 * returns: pointer to sequence of permuted characters
 **/
char* flipPancake(char* old, int w, int h, int fw, int fh, int v) {
    char* new;
    new = malloc(sizeof(char) * (w * h + 1));
    for (int i = 0; i < w * fh; i++) {
        // Permuting of characters only necessary if we are 
        // in a column that is permuted
        if (i % w < fw) {
            // Permute characters depending on vertical/horizontal flip
            if (v) {
               new[(i / w) * w + fw - 1 - (i % w)] = old[i];
            } else {
               new[(fh - 1 - (i / w)) * w + (i % w)] = old[i]; 
            }
        } else {
            new[i] = old[i];
        }
    }
    // Copy characters on rows that do not require permuting 
    // directly
    for (int i = w * fh; i < w * h; i++) {
        new[i] = old[i];
    }
    new[w * h] = '\0';
    return new;
}


/**
 * Function: getBatch()
 * ~~~~~~~~~~~~~~~~~~~~
 * Given a configuration, its width, and its height, getBatch() returns a 
 * pointer to a series af character pointers to possible permutations 
 * reachable from a single flip.  
 *
 * (w - 1) * (h - 1) + (w - 1) + (h - 1) configurations are returned.
 *
 * input
 * ~~~~~
 * config: character string to be permuted
 * w: width of config
 * h: height of config
 *
 * returns: pointer to character pointer
 **/
char** getBatch(char* config, int w, int h) {
    // Pointer to first permutation (to be returned)
    char** batch;
    // Pointer to current permutation (updated on each iteration)
    char** pancake;
    batch = malloc(sizeof(char*) * (2 * (w - 1) * (h - 1) + (w - 1) + (h - 1)));
    pancake = batch;
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            // Don't create a permutation with a flip height of 
            // 1 and flip width of 1... it's invariant
            if (i != 0) {
                *pancake = flipPancake(config, w, h, i + 1, j + 1, 1);
                pancake++;
            }
            if (j != 0) {
                *pancake = flipPancake(config, w, h, i + 1, j + 1, 0);
                pancake++;
            }
        }
    }
    return batch;
}

/**
 * Function: sort()
 * ~~~~~~~~~~~~~~~~
 * Simple implementation of bubble sort (polynomial time algorithm 
 * implemented because the input is size at most 256 chars).
 *
 * input
 * ~~~~~
 * unsorted: pointer to character string to be sorted
 * sorted: pointer at which to store sorted character string
 * len: length of character unsorted
 *
 * returns: nothing
 **/
void sort(char** unsorted, char** sorted, int len) {
    // Duplicate values[] to (*sorted)values[]
    for (int k = 0; k < len; k++) {
        (*sorted)[k] = (*unsorted)[k];
    }
    // sort ascending
    int i = 0;
    for (i = 1; i < len; i++) {
        int elt = (*sorted)[i];
        int j = i;
        while (((j > 0) && (*sorted)[j] < (*sorted)[j-1])) {
            (*sorted)[j] = (*sorted)[j-1];
            (*sorted)[j-1] = elt;
            j--;
        }
    }
    (*sorted)[len] = '\0';
    return;
}
