/**
 * Boggle.c
 *
 * Computer Science 223
 * Homework 7
 *
 * Addison Hu
 * addison.hu@yale.edu
 **/

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/c/cs223/Hwk3/getLine.h"

#define die(msg) exit(fprintf(stderr, "%s\n", msg))
#define DIRECTIONS 8
#define N_ALPHA 26

typedef struct rule {
    bool c;
    bool t;
    int nr;
    int nc;
    char* board;
} Rule;

typedef struct trie {
    int count;
    bool term;
    char* word;
    struct trie* next[N_ALPHA];
} Trie;
 
Trie* walkTrie(Trie* t, Rule* rule, int cur, bool* avlb);
void outputTrie(Trie* t, bool c);
Trie* initTrie();
Trie* addTrie(Trie* t, char* word, char* remainder);
void destroyTrie(Trie* t);
bool searchTrie(Trie* t, char* word);
int parseArgs(Rule* rule, int argc, char* argv[]);
Trie* parseInput(Trie* t);
char* filterLine(char** line);
int validMoves(int** moves, Rule* rule, int cur);

int main(int argc, char* argv[]) {
    Trie* t;
    t = initTrie();
    t = parseInput(t);
    Rule rule;
    parseArgs(&rule, argc, argv);
    int len = rule.nr * rule.nc;
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < N_ALPHA; j++) {
            if (t->next[j] != NULL &&
                (j == tolower(rule.board[i]) - 'a'
                 || rule.board[i] == '_')) {
                bool avlb[len];
                for (int k = 0; k < len; k++) {
                    avlb[k] = true;
                }
                if (rule.t) avlb[i] = false;
                t->next[j] = walkTrie(t->next[j], &rule, i, avlb);
            }
        }
    }
    outputTrie(t, rule.c);
    destroyTrie(t);
    return EXIT_SUCCESS;
}

Trie* walkTrie(Trie* t, Rule* rule, int cur, bool* avlb) {
    if (t->term) t->count++;
    int* moves = malloc(DIRECTIONS * sizeof(int));
    validMoves(&moves, rule, cur);
    for (int i = 0; i < DIRECTIONS; i++) {
        if (moves[i] != -1) {
            if (rule->board[moves[i]] == '_') {
                for (int j = 0; j < N_ALPHA; j++) {
                    if (t->next[j] != NULL && avlb[moves[i]]) {
                        if (rule->t) avlb[moves[i]] = false;
                        t->next[j] = walkTrie(t->next[j], rule, 
                                moves[i], avlb);
                        avlb[moves[i]] = true;
                    }
                }
            } else {
                int next = tolower(rule->board[moves[i]]) - 'a';
                if (t->next[next] != NULL && avlb[moves[i]]) {
                    if (rule->t) avlb[moves[i]] = false;
                    t->next[next] = walkTrie(t->next[next], rule, 
                            moves[i], avlb);
                    avlb[moves[i]] = true;
                }
            }
        }
    }
    free(moves);
    return t;
}

void outputTrie(Trie* t, bool c) {
    if (t->term) {
        if (!c && t->count > 0) printf("%s: %d\n", t->word, t->count);
        else if (c && t->count == 0) printf("%s\n", t->word);
        free(t->word);
    }
    for (int i = 0; i < N_ALPHA; i++) {
        if (t->next[i] != NULL) outputTrie(t->next[i], c);
    }
    return;
}

Trie* initTrie() {
    Trie* t = malloc(sizeof(Trie));
    t->term = false;
    t->count = 0;
    for (int i = 0; i < N_ALPHA; i++) {
        (t->next)[i] = NULL;
    }
    return t;
}

Trie* addTrie(Trie* t, char* word, char* remainder) {
    if (remainder[0] == '\0') {
        t->term = true;
        t->word = word;
    } else {
        int index = remainder[0] - 'a';
        if ((t->next)[index] == NULL) {
            (t->next)[index] = initTrie();
        }
        addTrie(t->next[index], word, ++remainder);
    }
    return t;
} 

void destroyTrie(Trie* t) {
    for (int i = 0; i < N_ALPHA; i++) {
        if (t->next[i] != NULL) destroyTrie(t->next[i]);
    }
    free(t);
    return;
}

bool searchTrie(Trie* t, char* word) {
    if (word[0] == '\0') {
        return true;
    } else {
        int index = word[0] - 'a';
        if ((t->next)[index] == NULL) {
            return false;
        } else {
            return searchTrie((t->next)[index], ++word);
        }
    }
}

int parseArgs(Rule* rule, int argc, char* argv[]) {
    int cur = 1;
    rule->c = rule->t = false;
    if (argc < 4 || argc > 6) {
        die("Boggle: usage: Boggle [-c] [-t] nRows nCols board");
    }
    // -c and then -t _in that order_
    if (argc == 6) {
        if (strcmp(argv[cur++], "-c") || strcmp(argv[cur++], "-t")) {
            die("Boggle: usage: Boggle [-c] [-t] nRows nCols board");
        } else {
            rule->c = true;
            rule->t = true;
        }
    // either -c or -t
    } else if (argc == 5) {
        if (!strcmp(argv[cur], "-c")) {
            rule->c = true;
        } else if (!strcmp(argv[cur], "-t")) {
            rule->t = true;
        } else {
            die("Boggle: usage: Boggle [-c] [-t] nRows nCols board");
        }
        cur++;
    }
    if ((rule->nr = atoi(argv[cur++])) < 1) {
        die("Boggle: invalid nr");
    }
    if ((rule->nc = atoi(argv[cur++])) < 1) {
        die("Boggle: invalid nc");
    }
    if (strlen(rule->board = argv[cur]) != rule->nr * rule->nc) {
        die("Boggle: board is not nRows x nCols");
    }
    int len = rule->nr * rule->nc;
    for (int i = 0; i < len; i++) {
        if (!isalpha(rule->board[i]) && rule->board[i] != '_') {
            die("Boggle: board must be alphabetic or blank");
        }
    }
    return true;
}

Trie* parseInput(Trie* t) {
    FILE* fp = stdin;
    char* line;
    while ((line = getLine(fp))) {
        if ((line = filterLine(&line)) != NULL) {
            t = addTrie(t, line, line);
        }
    }
    return t;
}

char* filterLine(char** line) {
    int len;
    if (!(len = strlen(*line) - 1)) {
        free(*line);
        return NULL;
    }
    char* newl = malloc((len + 1) * sizeof(char));
    for (int i = 0; i < len; i++) {
        if (islower((*line)[i])) {
            newl[i] = (*line)[i];
        } else if (isupper((*line)[i])) {
            newl[i] = tolower((*line)[i]);
        } else {
            free(*line);
            free(newl);
            return NULL;
        }
    }
    free(*line);
    newl[len] = '\0';
    return newl;
}

int validMoves(int** moves, Rule* rule, int cur) {
    bool u, d, l, r;
    int* move = *moves;
    /* Can we move up? */
    *move++ = (u = cur / rule->nc > 0) ? cur - rule->nc : -1;
    /* Can we move down? */
    *move++ = (d = cur / rule->nc < rule->nr - 1) ? cur + rule->nc : -1;
    /* Can we move left? */
    *move++ = (l = cur % rule->nc > 0) ? cur - 1: -1;
    /* Can we move right? */
    *move++ = (r = cur % rule->nc < (rule->nc - 1)) ? cur + 1: -1;
    /* Up-left? */
    *move++ = (u && l) ?  cur - rule->nc - 1 : -1;
    /* Up-right? */
    *move++ = (u && r) ?  cur - rule->nc + 1 : -1;
    /* Down-left? */
    *move++ = (d && l) ?  cur + rule->nc - 1 : -1;
    /* Down-right? */
    *move   = (d && r) ?  cur + rule->nc + 1 : -1;
    return 1;
}

