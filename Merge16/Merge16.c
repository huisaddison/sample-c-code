/**
 * Merge16.c
 * 
 * Computer Science 223
 * Homework 4
 * 
 * Addison Hu
 * addison.hu@yale.edu
 *
 * Merge16 is an implementation of mergeSort for character 
 * strings using queues.  Comparison is done lexigraphically 
 * with strncmp(), though the user may specify a starting 
 * position and maximum length of comparison with the 
 * [-POS[,LEN]] command-line parameters.  
 *
 **/

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/c/cs223/Hwk3/getLine.h"
#include "/c/cs223/Hwk4/Queue.h"

// Print message to stderr and exit.
#define die(msg)    exit (fprintf (stderr, "%s\n", msg))

void enqueueFiles(Queue* P, Queue* Q, int firstFile, int* pLen, 
        int* qLen, int pos, int len, int argc, char* argv[]);
void parseArgs(int* pos, int* len, int* firstFile, char* flags);
void outputLines(Queue* P, Queue* Q, int pos, int len);
void printLines(Queue* Q, int nLines);
int strnCompare (char* left, char* right, int pos, int len);
void safeAddQ(Queue* Q, char* line);
void safeRemoveQ(Queue* Q, char** line);

int main (int argc, char *argv[]) {
    // exit immediately if no arguments are given
    if (argc == 1) return EXIT_SUCCESS;

    // initialize position, length, and first file index to default values
    int pos = 0;
    int len = INT_MAX;
    int firstFile = 1; 
    parseArgs(&pos, &len, &firstFile, argv[1]);
    
    Queue P;
    Queue Q;
    // counters for the number of nodes in queues p and q
    int pLen = 0;
    int qLen = 0;
    if (!(createQ(&Q) && createQ(&P))) {
        die("createQ() failed");
    }

    // enqueue the lines from the files into the two queues, performing 
    // one round of mergeSort as they come in
    enqueueFiles(&P, &Q, firstFile, &pLen, &qLen, pos, len, argc, argv);
    // counters for the number of elements in each queue for the 
    // subsequent round of mergeSort
    int pLenNext = pLen;
    int qLenNext = qLen;
    // total lines across both queues
    int nLines = pLen + qLen;
 
    // indicator of which queue groups of sorted lines should enter
    // left corresponds with P, not left corresponds to Q
    int left = 1; 

    // counter for number of lines from each queue that have been 
    // sorted on a single pass of mergeSort
    int sortedP = 0;
    int sortedQ = 0;

    // counter for number of lines from each queue that have been 
    // sorted in each subgroup during a pass of mergeSort
    int subSortedP = 0;
    int subSortedQ = 0;
    char* line;  // line to be sorted
    char* pLine; // corresponds to line removed from P
    char* qLine; // corresponds to line removed from Q
    // groupSize starts at 2 because groups of size 1 were taken 
    // care of during enqueueing, and ends before the last round 
    // of mergeSort, which takes place during outputting
    for (int groupSize = 2; groupSize * 2 < nLines; groupSize *= 2) {
        // sorted lines always go into the left queue (P) first, 
        // to ensure stability
        left = 1;
        // update and reset counters
        pLen = pLenNext;
        qLen = qLenNext;
        pLenNext = 0;
        qLenNext = 0;
        sortedP = 0;
        sortedQ = 0;
        // while loop for a single pass / round of mergeSort
        // check whether all lines have been sorted
        while (pLenNext + qLenNext < nLines) {
            subSortedP = 0;
            subSortedQ = 0;

            // the following three while loops handle a single 
            // subgroup of a pass of mergeSort, and they while 
            // loop above ensures that all the subgroups are 
            // taken care of before moving onto the next pass 
            // of mergeSort.  left is flipped between 1 and 
            // -1 at the bottom of this (current) while loop

            // considering lines from both queues
            while (subSortedP < groupSize && 
                   subSortedQ < groupSize && 
                   sortedP < pLen && 
                   sortedQ < qLen) {
                if (!(headQ(&P, &pLine) &&
                      headQ(&Q, &qLine))) {
                    die("headQ() failed");
                }
                // choose  line from Q only if it comes before 
                // the line from P; P is chosen if it comes before 
                // Q _or is the same_, ensuring stability
                if (strnCompare(pLine, qLine, pos, len) > 0) {
                    safeRemoveQ(&Q, &line);
                    subSortedQ++;
                    sortedQ++;
                } else {
                    safeRemoveQ(&P, &line);
                    subSortedP++;
                    sortedP++;
                }
                if (left == 1) {
                    safeAddQ(&P, line); 
                    pLenNext++;
                } else {
                    safeAddQ(&Q, line);
                    qLenNext++;
                }
            }
            // lines for Q depleted, dump lines from P 
            // into appropriate queue
            while (subSortedP < groupSize && sortedP < pLen) {
                safeRemoveQ(&P, &line);
                subSortedP++;
                sortedP++;
                if (left == 1) {
                    safeAddQ(&P, line);
                    pLenNext++;
                } else {
                    safeAddQ(&Q, line);
                    qLenNext++;
                }
            }
            // lines from P depleted, dump lines from Q 
            // into appropriate queue
            while (subSortedQ < groupSize && sortedQ < qLen) {
                safeRemoveQ(&Q, &line);
                subSortedQ++;
                sortedQ++;
                if (left == 1) {
                    safeAddQ(&P, line);
                    pLenNext++;
                } else {
                    safeAddQ(&Q, line);
                    qLenNext++;
                }
            }
            // change queue that sorted lines go into
            left *= -1;
        }
    }
    // last sorting set, output lines
    outputLines(&P, &Q, pos, len);
    if (!destroyQ(&Q) || !destroyQ(&P)) {
        die("destroyQ() failed");
    }
    
    return EXIT_SUCCESS;
}

/**
 * Function: parseArgs()
 * ~~~~~~~~~~~~~~~~~~~~~
 * Parses command line arguments: position and length if present, 
 * and names of files containing lines to be sorted.  Values to be 
 * updated are passed in by reference.
 *
 * input
 * ~~~~~
 * - *pos: integer denoting sort key start position
 * - *len: integer denoting sort key length
 * - *firstFile: integer denoting index of the first file in 
 *      the vector of command-line arguments argv[]
 * - *flags: character string corresponding to argv[1]
 *
 * returns: nothing
 **/
void parseArgs(int* pos, int* len, int* firstFile, char* flags) {
    // if the first argument does not denote position/length, 
    // then it must be a file (first file may not start with '-')
    if (*flags != '-') {
        return;
    } else {
        // otherwise, the first file starts at argv[2]
        (*firstFile)++;
        // increment pointer and extract integer
        flags = flags + sizeof(char);
        if (isdigit(*flags)) {
            // store position in *pos, and set character 
            // pointer to point to remainder of string after 
            // the integer
            *pos = strtol(flags, &flags, 10);
            // if it's not the end of string or ',', then 
            // input is invalid
            if (*flags != '\0' && *flags != ',') {
                die("Invalid -POS,[LEN]");
            // if it is a comma, then length is also specified
            } else if (*flags == ',') {
                // same routine as above
                flags = flags + sizeof(char);
                if (isdigit(*flags)) {
                    *len = strtol(flags, &flags, 10);
                    if (*flags != '\0') {
                        die("Invalid -POS,[LEN]");
                    }
                } else {
                    die("Invalid -POS,[LEN]");
                }
            }
        } else {
                die("Invalid -POS,[LEN]");
        }
    } 
}

/**
 * Function: enqueueFiles()
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 * Enqueues lines from the files specified in the command-line 
 * into two queues, performing one round of mergeSort during 
 * the enqueueing process.  
 *
 * input
 * ~~~~~
 * - *P: Pointer to a queue, the "left" queue (i.e., lines in 
 *      P always come before lines in Q, ensuring stability)
 * - *Q: Pointer to the "right" queue
 * - firstFile: index of the first file in argv[]
 * - *pLen: counter for number of lines in Queue P
 * - *qLen: counter for number of lines in Queue Q
 * - pos: integer denoting start index of sort key
 * - len: integer deneoting length of sort key
 * - argc: integer denoting length of argv[]
 * - argv[]: string vector holding command line arguments
 *
 * returns: nothing
 **/
void enqueueFiles(Queue* P, Queue* Q, int firstFile, int* pLen, 
        int* qLen, int pos, int len, int argc, char* argv[]) {
    FILE* fp; 
    char* line;
    char* line1;
    char* line2;
    // flag for whether lines go into P or Q next
    // 1 corresponds to P, 0 to Q
    int left = 1;
    // counter for number of lines
    int nLines = 0;

    for (int i = firstFile; i < argc; i++) {
        if ((fp = fopen(argv[i], "r")) != NULL) {
            while ((line = getLine(fp))) {
                int strLen = strlen(line);
                // remove trailing newline, if any
                if (strLen > 0 && line[strLen - 1] == '\n') {
                    line[strLen - 1] = '\0';
                }
                // if we've enqueued an even number of lines, 
                // store in it line1 and enquee another line
                if ((nLines % 2) == 0) {
                    line1 = line;
                    nLines++;
                // if we've enqueued an odd number of lines, 
                // store in in line2 and sort lines line1 and line2
                } else {
                    nLines++;
                    line2 = line;
                    if (left == 1) {

                        if (strnCompare(line1, line2, pos, len) > 0) {
                            safeAddQ(P, line2);
                            safeAddQ(P, line1);
                        } else {
                            safeAddQ(P, line1);
                            safeAddQ(P, line2);
                        }                  
                        // increment counters
                        (*pLen) += 2;
                        left = 0;
                    } else {
                        if (strnCompare(line1, line2, pos, len) > 0) {
                            safeAddQ(Q, line2);
                            safeAddQ(Q, line1);
                        } else {
                            safeAddQ(Q, line1);
                            safeAddQ(Q, line2);
                        }                  
                        (*qLen) += 2;
                        left = 1;
                    }               
                }
            }
            fclose(fp);
        } else {
            die("file does not exist");
        }
    }
    // if we have a leftover line (because we enqueued two at a time), 
    // stick it into the appropriate queue
    if ((nLines % 2) == 1) {
        if (left == 1) {
            safeAddQ(P, line1);
            (*pLen)++;
        } else {
            safeAddQ(Q, line1);
            (*qLen)++;
        }
    }
}

/**
 * Function: outputLines()
 * ~~~~~~~~~~~~~~~~~~~~~~~
 * Outputs sorted lines to stdout, performing the last 
 * round of mergeSort in the process.  Frees lines 
 * as they are dequeued for the final time.
 *
 * input
 * ~~~~~
 * - *P: Pointer to the "left" queue
 * - *Q: Pointer to the "right" queue
 * - pos: start index of sort key
 * - len: length of sort key
 *
 * returns: nothing
 **/
void outputLines(Queue* P, Queue* Q, int pos, int len) {
    char* line; 
    char* pLine;
    char* qLine;
    // merge lines in P and Q
    while (!isEmptyQ(Q) && !isEmptyQ(P)) {
        if (!(headQ(Q, &qLine) && 
              headQ(P, &pLine))) {
            die("headQ() failed");
        }
        if (strnCompare(pLine, qLine, pos, len) > 0) {
            safeRemoveQ(Q, &line);
        } else {
            safeRemoveQ(P, &line);
        }
        fputs(line, stdout);
        fputs("\n", stdout);
        free(line);
    }   
    // dump the remaining lines to stdout
    while (!isEmptyQ(P)) {
        safeRemoveQ(P, &line);
        fputs(line, stdout);
        fputs("\n", stdout);
        free(line);
    }
    while (!isEmptyQ(Q)) {
        safeRemoveQ(Q, &line);
        fputs(line, stdout);
        fputs("\n", stdout);
        free(line); 
    }
}

/** 
 * Function: strnCompare()
 * ~~~~~~~~~~~~~~~~~~~~~~~
 * Wrapper for strncmp() to support the sort key starting 
 * position required by Merge 16, which is achieved via 
 * pointer arithmetic.
 *
 * input
 * ~~~~~
 * left: character string to be compared
 * right: character string to be compared
 * pos: integer denoting starting index of sort key
 * len: integer denoting length of sort key
 *
 * returns: 0 if left comes before or is equal to right, 
 *      1 if right comes before left
 **/
int strnCompare (char* left, char* right, int pos, int len) {
    int leftLen = strlen(left);
    int rightLen = strlen(right);
    // pointers to the keys used for sorting
    char* leftKey;
    char* rightKey;
    // set leftKey and rightKey to be the position-th character 
    // in left and right respectively, or to the 
    // null terminator if position is greater than the lengths 
    // of those strings
    if (leftLen > pos) {
        leftKey = left + sizeof(char) * pos;
    } else {
        leftKey = left + sizeof(char) * leftLen;
    }
    if (rightLen > pos) {
        rightKey = right + sizeof(char) * pos;
    } else {
        rightKey = right + sizeof(char) * rightLen;
    }
    return strncmp(leftKey, rightKey, len);
}

/**
 * Function: safeAddQ()
 * ~~~~~~~~~~~~~~~~~~~
 * Wrapper for AddQ() to allow for cleaner code.
 *
 * inputs
 * ~~~~~~
 * - *Q: Pointer to Queue
 * - line: line to be added to Q
 *
 * returns: nothing
 **/
void safeAddQ(Queue* Q, char* line) {
    if (!addQ(Q, line)) {
        die("addQ() failed");
    } 
}

/** 
 * Function: safeRemoveQ()
 * ~~~~~~~~~~~~~~~~~~~~~~~
 * Wrapper for RemoveQ().
 *
 * inputs
 * ~~~~~~
 * - *Q: Pointer to Queue
 * - *line: pointer to a char* to hold line removed from Q
 *
 * returns: nothing
 **/
void safeRemoveQ(Queue* Q, char** line) {
    if (!removeQ(Q, line)) {
        die("removeQ() failed");
    } 
}
