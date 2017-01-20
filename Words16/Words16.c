/**
 * Words16.c
 *
 * Computer Science 223
 * Homework 6
 *
 * Addison Hu
 * addison.hu@yale.edu
 *
 **/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/c/cs223/Hwk3/getLine.h"
#include "Tree.h"

// Number base to be used with strtol()
#define BASE 10

/**
 * Enum: Arg
 * ~~~~~~~~~
 * Specifies the type of command-line argument currently being considered 
 * by checkArgs()
 *
 * members
 * ~~~~~~~
 *  - DELETE: corresponds to -d
 *  - PRINT: corresponds to -print
 *  - DUMP: -dump
 *  - EPL: -epl
 *  - SET: -set
 *  - OTHER: any other argument (attempt to intepret as input text file)
 **/
typedef enum Arg {
    DELETE,
    PRINT,
    DUMP,
    EPL,
    SET,
    OTHER
} Arg; 


Arg checkArg(char* arg);
void insertRemoveFile(Tree* t, char* file, int lim, int insert);
void splitLine(Tree* t, char* line, int lim, int insert);

int main(int argc, char* argv[]) {
    Tree t;
    create(&t);
    // Initialize improvement factor to 0
    int lim = 0;

    // Skip to first non-program-name command-line arg
    int i = 1;
    Arg arg;
    while (i < argc) {
        // If doesn't match any flag, then attempt to open file and 
        // insert words
        if ((arg = checkArg(argv[i])) == OTHER) {
            insertRemoveFile(&t, argv[i], lim, 1);
        } else if (arg == DUMP) {
            dump(t);
        } else if (arg == PRINT) {
            printPairs(t);
        } else if (arg == EPL) {
            printEPL(t);
        // If current flag is -d, then remove words from following command 
        // line argument, unless -d is the _last_ arg, in which case 
        // attempt to open a file named "-d".
        } else if (arg == DELETE) {
            if (i + 1 == argc) {
                insertRemoveFile(&t, argv[i], lim, 1);
            } else {
                insertRemoveFile(&t, argv[++i], lim, 0);
            }
        // If current flag is "-set", then set lim (improvement factor) 
        // to following arg, unless "-set" is last arg, in which was 
        // attempt to insert words from file "-set".
        } else {
            if (i + 1 == argc) {
                insertRemoveFile(&t, argv[i], lim, 1);
            } else {
                lim = strtol(argv[++i], NULL, BASE);
            }
        }
        i++;
    }
    destroy(t);
    return EXIT_SUCCESS;
}

/**
 * Function: checkArg()
 * ~~~~~~~~~~~~~~~~~~~~
 * Attempts to match command-line argument to a flag.
 *
 * input
 * ~~~~~
 *  - arg: character string of command-line argument
 *  
 * returns: Arg type corresponding to argument flag / file
 **/
Arg checkArg(char* arg) {
    if (strcmp(arg, "-d") == 0) {
        return DELETE;
    } else if (strcmp(arg, "-print") == 0) {
        return PRINT;
    } else if (strcmp(arg, "-dump") == 0) {
        return DUMP;
    } else if (strcmp(arg, "-epl") == 0) {
        return EPL;
    } else if (strcmp(arg, "-set") == 0) {
        return SET;
    } else {
        return OTHER;
    }
}

/**
 * Function: insertRemoveFile()
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Gets lines from input file and passes them into splitLine(), which 
 * splits them into words for insertion or deletion.
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 *  - file: input file name (char string)
 *  - lim: improvement factor
 *  - insert: 1 if inserting words, 0 if deleting
 *
 * returns: nothing
 **/
void insertRemoveFile(Tree* t, char* file, int lim, int insert) {
    FILE* fp;
    if ((fp = fopen(file, "r")) != NULL) {
        char* line;
        while ((line = getLine(fp))) {
            splitLine(t, line, lim, insert);
            free(line);
        }
        fclose(fp);
    } else {
        exit(fprintf(stderr, "Words16: cannot open %s\n", file));
    }
   return;
}

/**
 * Function: splitLine()
 * ~~~~~~~~~~~~~~~~~~~~~
 * Parses a string of characters into maximal alphanumeric strings (words), 
 * and insert them to [deletes them from] tree.  
 *
 * inputs
 * ~~~~~~
 *  - t: Tree type
 *  - line: character string of line from file
 *  - lim: improvement factor
 *  - insert: 1 if inserting words, 0 if deleting
 *
 * returns: nothing
 **/
void splitLine(Tree* t, char* line, int lim, int insert) {
    char* start;
    char* word;
    int len;
    while (*line != '\0') {
        len = 0;
        start = line;
        while (isalnum(*line)) {
            len++;
            line++;
        }
        if (len > 0) {
            word = malloc(sizeof(char) * (len + 1));
            char* letter = word;
            for (int i = 0; i < len; i++) {
                *letter = tolower(*start);
                letter++;
                start++;
            }
            *letter = '\0';
            if (insert) {
                *t = increment(*t, word, lim);
            } else {
                char* delKey = NULL;
                int delFlag = 1;
                *t = delete(*t, word, lim, &delKey, &delFlag);
                free(word);
                if (delFlag) {
                    free(delKey);
                }
            }
        }
        if (*line == '\0') {
            break;
        }
        line++;
    }
    return;
}

