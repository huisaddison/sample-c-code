/**
 * Subst16
 *
 * Computer Science 223
 * Homework 3
 *
 * Addison Hu
 * addison.hu@yale.edu
 *
 * Subst16 is a string filter that, given input text and a set of replacement
 * rules , will replace occurences of FROM with TO, with support for the 
 * following options:
 * 
 * 
 * Wildcards
 * ~~~~~~~~~
 * '.': in the FROM field, matches any character
 * '^': in the TO field, substitutes the matched FROM text into the 
 *      replacement text
 * Both wildcards may be escaped with '@'.  
 *
 * Flags
 * ~~~~~
 * Subst16 parses a file line-by-line.  These flags are preceded with a 
 * dash, and control Subst16's behavior with respect to each individual 
 * line.  In a string of flags with conflicting options, the _last_ 
 * flag of each category will be followed.
 *
 * Sn: instructs Subst16 to move to the nth rule, given success of the 
 *     current rule.
 * Fm: instructs Subst16 to move to the mth rule, given success of the 
 *     current rule.
 * q : instructs Subst16 to stop making substitutions after the first 
 *     successful substitution on each line.
 * g : instructs Subst16 to continue scanning the line after each 
 *     substitution, starting at the character immediately following 
 *     the substitution that was made.
 * r : instructs Subst16 to continue scanning the line after each 
 *     substitution, starting at the beginning of the substitution 
 *     that was made.
 **/

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/c/cs223/Hwk3/getLine.h"

// Number of command line arguments which together constitute 
// a substitution rule
#define RULE_SIZE 3 
// An offset, indicating the integer value to which the 
// character zero is mapped.  Integer values for the 
// other numeral characters are defined relative to this value.
#define ASCII_ZERO 48

/** Enumeration: Flag
 * Allows easy recording and checking of the proper 
 * input line scanning method.  Q, G, and R represent 
 * the -q, -g, and -r flags.
 **/
typedef enum Flag {
    Q,
    G,
    R
} Flag;

/** Struct: Rule
 * This compact structure facilities storage of each 
 * substitution rule from the command line and easy 
 * access by the substitution methods.
 * 
 * Members
 * ~~~~~~~
 * from:    FROM string to be found in input text
 * to:      TO replacement string for matched text
 * flag:    q, g, or r scanning flag to be applied
 * s:       integer indicating which rule to execute 
 *              next upon success, or -1 if not specified
 * f:       integer indicating which rule to execute 
 *              next given failure, or -1 if not specified
 **/
typedef struct Rule {
    char* from;
    char* to;
    Flag flag;
    int s;
    int f;
} Rule;

/** Struct: Replacement
 * Created while parsing a line, it fully specfies what is to 
 * be replaced with what.
 * 
 * Members
 * ~~~~~~~
 * start:       index of line where substitution begins
 * oldFinish:   original index of line where substitution ends
 * newFinish:   index of line where substitution ends post substitution
 * newLength:   length of line post substitution
 * insertion:   character string to be substituted into the line
 * modified:    flag indicating whether a replacement will be executed
 *                  on the current scan.  
 * success:     flag indicating whether a replacement was _ever_ done
 *                  on any scan.  Used for the success/failure flags.
 **/
typedef struct Replacement {
    int start;
    int oldFinish;
    int newFinish;
    int newLength;
    char* insertion;
    bool modified;
    bool success;
} Replacement;

bool isFlagSeq(char* flags);
void initRule(Rule* rule);
void resetReplacement(Replacement* rpSet);
void parseFlagSeq(char* flags, Rule* curRule);
void parseArgs(Rule ruleList[], int argc, char* argv[]);
void parseLine(char* line, int searchStart, Rule curRule, Replacement* rpSet);
char* rewriteLine(char* oldLine, Replacement rpSet);
bool stringCompare(char* from, char* substring);
int stringLength (char* string);
int caretIndex(char* word, int lastStop);
int updateRule(char* curLine, Rule curRule, Replacement* rpSet, int caretInd);

int main(int argc, char* argv[]) {
    // Input is valid only if the number of arguments (excluding filename) is 
    // a multiple of three.  Otherwise, one or more rules is not fully 
    // specified.
    if ((argc - 1) % RULE_SIZE) {
        printf("fail!\n");
        return 1;
    }
    
    // initialize and allocate space for an array of Rule structs, 
    // then parse the command line arguments into those structs.
    int nRules = (argc - 1) / RULE_SIZE;
    Rule* ruleList;
    ruleList = (Rule*) malloc(nRules * sizeof(Rule));
    parseArgs(ruleList, argc, argv);

    // initialize buffers for the current line 
    // and replacement set, which are passed through 
    // separate helper functions for parsing and rewriting 
    // the line
    char* curLine;
    Replacement rpSet;
    while ((curLine = getLine(stdin)) != NULL) {
        int curRule = 0;
        while (curRule < nRules) {
            resetReplacement(&rpSet);
            // if the -q flag is specified, parseLine() is asked 
            // only to find the first occurrence of the FROM string, 
            // and only one substitution is made
            if (ruleList[curRule].flag == Q) {
                // parseLine() updates rpSet, which is passed by 
                // reference
                parseLine(curLine, 0, ruleList[curRule], &rpSet);
                curLine = rewriteLine(curLine, rpSet);
            // if the -g flag is specified, matches to FROM are 
            // found globally across the line
            } else if (ruleList[curRule].flag == G) {
                while (rpSet.modified == true) {
                    parseLine(curLine, rpSet.newFinish, ruleList[curRule], 
                            &rpSet);
                    curLine = rewriteLine(curLine, rpSet);
                }
            // if the -r flag is specified, the line is rescanned 
            // starting from where the substitution started
            } else if (ruleList[curRule].flag == R) {
                while (rpSet.modified == true) {
                    parseLine(curLine, rpSet.start, ruleList[curRule], &rpSet);
                    curLine = rewriteLine(curLine, rpSet);
                }
            }
            if (rpSet.success) {
                if (ruleList[curRule].s == -1) {
                    curRule++;
                } else {
                    curRule = ruleList[curRule].s;
                }
            } else {
                if (ruleList[curRule].f == -1) {
                    curRule++;
                } else {
                    curRule = ruleList[curRule].f;
                }
            }
        }
        printf("%s", curLine);
        free(curLine);
    } 
    free(ruleList);
    return 0;
}

/** 
 * Function: isFlagSeq()
 * ~~~~~~~~~~~~~~~~~~~~~
 * Checks whether a character string is a sequence of flags.
 *
 * input
 * ~~~~~
 * - *flags: character string
 *
 * returns: true if first character is '-', else otherwise
 **/
bool isFlagSeq(char* flags) {
    return flags[0] == '-';
}

/**
 * Function: initRule()
 * ~~~~~~~~~~~~~~~~~~~~
 * Gives default values to a Rule struct.  Unless specified 
 * otherwise, Subst16 should apply the -q flag, and proceed 
 * to the next rule.  parseArgs() will overwrite these 
 * default values if the user specifies otherwise.
 *
 * input
 * ~~~~~
 * - *curRule: pointer to Rule struct 
 *
 * returns: nothing
 **/
void initRule(Rule* curRule) {
    curRule->flag = Q;
    curRule->s = -1;
    curRule->f = -1;
}

/**
 * Function: resetReplacement()
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Gives default values to a Replacement struct.  
 * 
 * input
 * ~~~~~
 * *rpSet: pointer to Replacement struct
 * returns: nothing
 **/
void resetReplacement (Replacement* rpSet) {
    rpSet->start = 0;
    rpSet->oldFinish = 0;
    rpSet->newFinish = 0;
    rpSet->modified = true;
    rpSet->success = false;
}

/** 
 * Function: parseFlagSeq()
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 * Parses a sequence of flags into a Rule struct.
 *
 * input
 * ~~~~~
 * - *flags: flag sequence (character string)
 * - *curRule: pointer to a Rule struct
 *
 * returns: nothing
 **/
void parseFlagSeq(char* flags, Rule* curRule) {
    // index of current character being considered in the 
    // flag sequence string
    // start from 1 because character 0 is '-'
    int i = 1;
    while (flags[i] != '\0') {
        if (flags[i] == 'S') {
            i++;
            int sNextRule = 0;
            // convert string representation of number to integer
            while(flags[i] != '\0' && isdigit(flags[i])) {
                sNextRule = sNextRule * 10 + ((int) flags[i] - ASCII_ZERO);
                i++;
            }
            curRule->s = sNextRule;
        } else if (flags[i] == 'F') {
            i++;
            int fNextRule = 0;
            while(flags[i] != '\0' && isdigit(flags[i])) {
                fNextRule = fNextRule * 10 + ((int) flags[i] - ASCII_ZERO);
                i++;
            }
            curRule->f = fNextRule;
        } else if (flags[i] == 'q') {
            curRule->flag = Q;
            i++;
        } else if (flags[i] == 'g') {
            curRule->flag = G;
            i++;
        } else if (flags[i] == 'r') {
            curRule->flag = R;
            i++;
        // ignore invalid flags
        } else {
            i++;
        }
    }        
}

/** 
 * Function: parseArgs()
 * ~~~~~~~~~~~~~~~~~~~~~
 * Parses an array of command line arguments into 
 * an array of Rule structs.
 *
 * input
 * ~~~~~
 * - *ruleList: pointer to array of Rules
 * - argc: number of command line arguments
 * - *argv[]: array of character strings
 *
 * returns: nothing
 **/
void parseArgs(Rule* ruleList, int argc, char* argv[]) {
    int j = 0;
    for (int i = 1; i < argc; i++) {
        initRule(&ruleList[j]);
        ruleList[j].from = argv[i++];
        ruleList[j].to = argv[i++];
        if (isFlagSeq(argv[i])){
        parseFlagSeq(argv[i], &(ruleList[j]));
        } else {
            exit(2);
        }
        j++;
    }
}

/**
 * Function: parseLine()
 * ~~~~~~~~~~~~~~~~~~~~~
 * Parses a line, looking for the first match of the FROM string 
 * following the the sarting search index.  Updates the replacement 
 * set with information necessary to rewrite the line with 
 * the proper substitution.
 *
 * input
 * ~~~~~
 * - *line: pointer to line of texte being parsed
 * - searchStart: index of line at which to start.  necessary of the -g and 
 *                  -r flags
 *  - curRule: the rule struct specifying FROM, TO, etc...
 *  - *rpSet: pointer to replacement struct, which parseLine() updates 
 *              so that rewriteLine() can use the same struct to make 
 *              the substitution
 *
 *  returns: nothing
 **/
void parseLine(char* line, int searchStart, Rule curRule, Replacement* rpSet) {
    // do not attempt to parse the line if the length of the line is shorter 
    // than the length of the replacement string
    if (strlen(line) < strlen(curRule.from)) {
        rpSet->modified = false;
        return;
    }
    // buffer for substring of the line to be compared with the FROM string`
    char* substring;
    substring = (char*) malloc(sizeof(strlen(curRule.from) + 1));
    for (int i = searchStart; 
             i < strlen(line) - stringLength(curRule.from); 
             i++) {
        for (int j = 0; j < stringLength(curRule.from); j++) {
            substring[j] = line[i + j];
        }
        substring[stringLength(curRule.from)] = '\0';
        // custom stringCompare() function which accounts for 
        // the '.' wildcard
        if (stringCompare(curRule.from, substring)) {
            // if we have a match, update fields of the Replacement struct 
            // to fully specify where and with what to perform the 
            // substitution
            rpSet->start = i;
            rpSet->oldFinish = i + stringLength(curRule.from);
            // make a copy of the TO string, in case it needs to be modified 
            // due to the '^' wildcard.  
            rpSet->insertion = (char*) malloc((strlen(curRule.to) + 1) * 
                    sizeof(char));
            strcpy(rpSet->insertion, curRule.to);
            // replaces the caret '^' wildcard with the matching FROM string
            // lastStop indicates where the last caret substitution 
            // occured, to avoid infinite looping, if a caret was 
            // part of the FROM string.
            int lastStop = 0;
            int caretInd;
            do {
                // caretIndex() gets index of first caret after lastStop, or 
                // returns -1 if there are none
                caretInd = caretIndex(rpSet->insertion, lastStop);
                lastStop = updateRule(line, curRule, rpSet, caretInd);

            } while (caretInd != -1);
            rpSet->newFinish = i + strlen(rpSet->insertion);
            rpSet->newLength = strlen(line) - stringLength(curRule.from) + 
                strlen(rpSet->insertion);
            rpSet->modified = true;
            rpSet->success = true;
            free(substring);
            // returns as soon as match has been found and 
            // replacement set is fully specified
            return;
        } 
    }
    free(substring);
    rpSet->modified = false;
}

/**
 * Function: rewriteLine()
 * ~~~~~~~~~~~~~~~~~~~~~~~
 * Rewrites an input line, following the parameters
 * listed in the Replacement struct
 *
 * input
 * ~~~~~
 * - *oldLine: character string being operated upon
 * - rpSet: Replacement struct explaining how to do the 
 *          replacement
 *
 * returns: char* newLine, a character string with the 
 * proper string substitution.
 *
 **/
char* rewriteLine(char* oldLine, Replacement rpSet) {
    if (rpSet.modified == false) {
        return oldLine;
    }
    // declare and allocate memory for newLine
    // the size is known because rpSet specifies the length 
    // of the newLine
    char* newLine;
    newLine = (char*) malloc((rpSet.newLength + 1) * sizeof(char));
    // copy from oldline to newLine up until the string 
    // substitution is to begin
    for (int i = 0; i < rpSet.start; i++) {
        newLine[i] = oldLine[i];
    }
    // copy from the insertion character string to the newLine
    for (int i = 0; i < strlen(rpSet.insertion); i++) {
        newLine[i + rpSet.start] = rpSet.insertion[i];
    }
    // copy the balance of oldLine starting from the character 
    // immediately following the end of the text that was replaced
    for (int i = rpSet.oldFinish; i < strlen(oldLine); i++) {
        newLine[i + rpSet.newFinish - rpSet.oldFinish] = oldLine[i];
    }
    newLine[rpSet.newLength] = '\0';
    free(oldLine);
    free(rpSet.insertion);
    return newLine;
}

/**
 * Function: stringCompare()
 * ~~~~~~~~~~~~~~~~~~~~~~~~~
 * Checks whether two strings are the same, with support 
 * for the '.' wildcard.
 *
 * input
 * ~~~~~
 * - *from: character string 1
 * - *substring: character string 2
 *
 * returns: true if the strings are the same, else false
 **/
bool stringCompare(char* from, char* substring) {
    int f = 0;
    int s = 0;
    while (from[f] != '\0' && substring[s] != '\0') {
        if (from[f] != '@' && from[f] != '.') {
            // characters must be the same if we are not in a 
            // wildcard sequence
            if (from[f] != substring[s]) {
                return false;
            } else {
                f++, s++;   
            }
            // handling for an escaped '.' or '@'
        } else if (from[f] == '@') {
            if (from[f + 1] == '\0') {
                return from[f] == substring[s];
            } else if (from[f + 1] != '@' && from[f + 1] != '.') {
                if (from[f] != substring[s]) {
                    return false;
                } else {
                    f++;
                    s++;
                }
            } else {
                f++;
                if (from[f] != substring[s]) {
                    return false;
                } else {
                    f++;
                    s++;
                }
            }
        // implicitly, continue moving along if the from 
        // string has the '.' wildcard and the substring 
        // has anything
        } else {
            f++;
            s++;
        }
    }
    return true;
}

/**
 * Function: stringLength()
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 * Find the length of a string, once escape sequences have been accounted for.
 *
 * input
 * ~~~~~
 * - *string: character string
 *
 * returns: (int) length of string
 **/
int stringLength (char* string) {
    // count tracks the number of characters seen so far 
    // (escape sequence and escaped character count as one char)
    int count = 0;
    // i indexes our place in the character string
    int i = 0;
    while (string[i] != '\0') {
        count++;
        // if we see escape character '@', we increment count 
        // again only if it is _not_ followed by '.' or '@'
        if (string[i++] == '@') {
            if (string[i] == '\0') {
                return count;
            } else if (string[i] != '@' && string[i] != '.') {
                count++;
                i++;
            } else {
                i++;
            }
        }
    }
    return count;
}

/**
 * Function: caretIndex()
 * ~~~~~~~~~~~~~~~~~~~~~~
 * Finds the index of the next caret after 
 * an index lastStop
 *
 * input
 * ~~~~~
 * - *word: character string to be searched
 * - lastStop: an integer index, specifying where 
 *              to start searching (customarily the index 
 *              where the previous caret was found).
 * 
 * returns: int index of caret if found, -1 otherwise
 **/
int caretIndex(char* word, int lastStop) {
    for (int i = lastStop; i < strlen(word); i++) {
        if (word[i] == '^') return i;
    }
    return -1;
}

/**
 * Function: updateRule()
 * ~~~~~~~~~~~~~~~~~~~~~~
 * Replaces caret in replacement set with the FROM string, in accordance 
 * with the behavior of the caret wildcard.
 *
 * input
 * ~~~~~
 * - *curLine: line from which FROM string will be read
 * - curRule: current rule being applied; helps specify indices of the 
 *              replacement string
 * - *rpSet: replacement struct, whose insertion field will be updated 
 *              (caret will be replaced with FROM string)
 * - caretInd: integer index of location where the caret substitution ended 
 *              in curLine, or -1 if no substitution occurred.  This helps 
 *              caretIndex() look for the next caret
 * 
 * returns: caretInd (index of first char after subst, or -1 if no subst)
 **/
int updateRule(char* curLine, Rule curRule, Replacement* rpSet, int caretInd) {
    if (caretInd == -1) {
        return -1;
    }

    char* caretSub;
    caretSub = (char*) malloc((stringLength(curRule.from) + 1) * sizeof(char));
    Replacement rpCaret;
    rpCaret.start = caretInd;
    rpCaret.oldFinish = caretInd + 1;
    rpCaret.newFinish = caretInd + stringLength(curRule.from);
    rpCaret.newLength = strlen(curRule.to) + stringLength(curRule.from) - 1;
    rpCaret.insertion = caretSub;
    for (int i = 0; i < stringLength(curRule.from); i++) {
        caretSub[i] = curLine[i + rpSet->start];
    }
    caretSub[stringLength(curRule.from)] = '\0';
    rpCaret.modified = true;
    rpSet->insertion = rewriteLine(rpSet->insertion, rpCaret);
    return rpCaret.newFinish;
}
