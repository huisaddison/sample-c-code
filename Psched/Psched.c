/**
 * Psched.c
 * 
 * Applies a number of processor scheduling algorithms.  Options include:
 *      * -opt: optimal assignment via backtracking
 *      * -lw: least workload
 *      * -lwd: least workload decreasing
 *      * -bw: best workload
 *      * -bwd: best workload decreasing
 * The user may specify the number of processors, a series of integer-numbers
 * corresponding to the workload of each task, and a series of flags 
 * corresponding to the algorithms that she wishes to have applied.
 *
 * Computer Science 223
 * Homework 2
 *
 * Addison Hu
 * addison.hu@yale.edu
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool isFlag(char* input);
void sort(int values[], int sortedvalues[], int n);
void applyFlag(char* flag, int tasks[], int nTasks, int nProcs);
int argmin(int values[], int n);
int argmbw(int processors[], int n, int task, int curMax);
int max(int values[], int n);
int sum(int values[], int n);
bool lowerSame(int values[], int j);
int optCore(int tasks[], int nTasks, int nProcs);
int optBT(int tasks[], int nTasks, int curTask, int processors[], int nProcs, 
    int vsf, int lowBnd, int prevTask, int prevProc);
int lwCore(int tasks[], int nTasks, int nProcs);
int bwCore(int tasks[], int nTasks, int nProcs);
void opt(int tasks[], int nTasks, int nProcs);
void lw(int tasks[], int nTasks, int nProcs);
void lwd(int tasks[], int nTasks, int nProcs);
void bw(int tasks[], int nTasks, int nProcs);
void bwd(int tasks[], int nTasks, int nProcs);

int main(int argc, char* argv[]) {
    // Program only useful if number of processors, tasks, 
    // and scheduling algorithm(s) is specified. 
    if (argc < 2) {
        return 1;
    }
    // Convert the first command line argument to an integer
    // and store it
    int nProcs = atoi(argv[1]);

    // Initializing arrays for tasks and flags
    int tasks[argc];
    int nTasks = 0;
    char* flags[argc];
    int nFlags = 0;

    // Consider command-line arguments sequentially
    for (int i = 2; i < argc; i++) {
        // If not a valid flag, use atoi() to convert it
        // to an integer.
        // This way, our program handles invalid flags/strings
        // by having atoi() convert them to 0, and tasks of 
        // work 0 have no effect on the output of our program.
        // Store these tasks as an array of integers.
        if (!isFlag(argv[i])) {
            tasks[nTasks] = atoi(argv[i]);
            nTasks++;
        // Otherwise, the input is a valid flag, and store it
        // in an arary of flags.
        } else {
            flags[nFlags] = argv[i];
            nFlags++;
        }
    }
    // Quit immediately if there are no tasks to assign, 
    // or if there are the user specified zero processors.
    // Also, if the first argument was not an int, our program
    // would interpret that as zero processors and also quit.
    if (nTasks == 0 || nProcs == 0) {
        return 1;
    }
    // Apply the algorithms specified by the user via flags
    for (int i = 0; i < nFlags; i++) {
        applyFlag(flags[i], tasks, nTasks, nProcs);
    }
    return 0;
}

// Returns a bool indicating whether an input is a valid flag
bool isFlag(char* input) {
    if    (strcmp(input, "-opt") == 0 || strcmp(input, "-lw") == 0
        || strcmp(input, "-lwd") == 0 || strcmp(input, "-bw") == 0
        || strcmp(input, "-bwd") == 0) {
        return true;
    } else {
        return false;
    }
}

// Given an array of values, and another array of the same 
// size, places sorted array of values in the second value.
// We do not sort in place because the program needs to 
// remember the initial ordering of the tasks.
// Note: here, we employ bubble sort.
void sort(int values[], int sortedvalues[], int n) {
    // Duplicate values[] to sortedvalues[]
    for (int k = 0; k < n; k++) {
        sortedvalues[k] = values[k];
    }
    // Sort sortedvalues[] descending
    int i = 0;
    for (i = 1; i < n; i++) {
        int element = sortedvalues[i];
        int j = i;
        while ((sortedvalues[j] > sortedvalues[j-1]) && (j > 0)) {
            sortedvalues[j] = sortedvalues[j-1];
            sortedvalues[j-1] = element;
            j--;
        }
    }
    return;
}

// Given the flag as a string, an array of tasks, etc., applies 
// the algorithm using a number of functions, one for each
// algorithm
void applyFlag(char* flag, int tasks[], int nTasks, int nProcs) {
    if (strcmp(flag, "-opt") == 0) {
        opt(tasks, nTasks, nProcs);
    } else if (strcmp(flag, "-lw") == 0) {
        lw(tasks, nTasks, nProcs);
    } else if (strcmp(flag, "-bw") == 0) {
        bw(tasks, nTasks, nProcs);
    } else if (strcmp(flag, "-lwd") == 0) {
        lwd(tasks, nTasks, nProcs);
    } else {
        bwd(tasks, nTasks, nProcs);
    }
}

// Returns index of the minimum element of an array
int argmin(int values[], int n) {
    int min = values[0];
    int argmin = 0;
    for (int i = 1; i < n; i++) {
        if (values[i] < min) {
            min = values[i];
            argmin = i;
        }
    }
    return argmin;
}

// Modified version of argmin() which returns the index,
// based on the best workload strategy, that the next 
// task should be added to.  Returns the argmin() if 
// best workload is not helpful
int argmbw(int processors[], int n, int task, int curMax) {
    int bestWork = 0;
    int bwIndex = -1;
    for (int i = 0; i < n; i++) {
        if ((processors[i] + task <= curMax) 
         && (processors[i] + task > bestWork)) {
            bestWork = processors[i] + task;
            bwIndex = i;
        }
    }
    if (bwIndex != -1) {
        return bwIndex;
    } else {
        return argmin(processors, n);
    }
}

// Returns the maximum value in an array
int max(int values[], int n) {
    int max = values[0];
    for (int i = 0; i < n; i++) {
        if (values[i] > max) {
            max = values[i];
        }
    }
    return max;
}

// Returns sum of values in array
int sum(int values[], int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += values[i];
    }
    return sum;
}

// Given an array and index j, determines whether 
// there is an entry in the array with index less than
// j, but with the same value.
bool lowerSame(int values[], int j) {
    bool sameWork = false;
    int i = 0;
    while ((i < j) && !sameWork) {
        if (values[i] == values[j]) {
            sameWork = true;
        }
        i++;
    }
    return sameWork;
}

// The core algorithm for the backtracking algorithm.  
// Contains mostly initialization.
// Uses optBT(), the recursive algorithm
int optCore(int tasks[], int nTasks, int nProcs) {
    int processors[nProcs];
    memset(processors, 0, sizeof(processors));
    int curTask = 0;
    int vsf = lwCore(tasks, nTasks, nProcs);
    int lowBnd = sum(tasks, nTasks) / nProcs + (sum(tasks, nTasks) %
         nProcs != 0);
    int prevTask = 0;
    int prevProc = 0;
    return optBT(tasks, nTasks, curTask, processors, nProcs, vsf, lowBnd,
        prevTask, prevProc);
}

// The backtracking algorithm
int optBT(int tasks[], int nTasks, int curTask, int processors[], int nProcs, 
    int vsf, int lowBnd, int prevTask, int prevProc) {
    // Quits if we've assigned all the tasks, and returns the best value
    if (curTask == nTasks) {
        if (max(processors, nProcs) < vsf) {
            return max(processors, nProcs);
        } else {
            return vsf;
        }
    }
    // Considers all the branches
    for (int i = 0; i < nProcs; i++) {
        // Applies heuristics D & E
        if ((!(tasks[curTask] == prevTask) || !(i < prevProc)) 
            && !lowerSame(processors, i)) {
            // Try going down the current branch
            processors[i] += tasks[curTask];
            // Continue down the branch if we haven't exceeded 
            // the best value so far
            if (max(processors, nProcs) < vsf) {
                vsf = optBT(tasks, nTasks, curTask + 1, processors, nProcs, vsf, 
                    lowBnd, tasks[curTask], i);
            }
            // If we've achieved the lower bound, quit immediately
            if (vsf == lowBnd) {
                return vsf;
            }
            processors[i] -= tasks[curTask];
        }
    }
    return vsf;
}

// Applies the least workload algorithm
// Return the maximum workload over the processors
// that the algorithm assigns
int lwCore(int tasks[], int nTasks, int nProcs) {
    int processors[nProcs];
    memset(processors, 0, sizeof(processors));
    for (int i = 0; i < nTasks; i++) {
        processors[argmin(processors, nProcs)] += tasks[i];
    }
    return max(processors, nProcs);
}

// Applies the best workload algorithm
// Returns maximum workload over the processors
// that the algorithm assigns
int bwCore(int tasks[], int nTasks, int nProcs) {
    int curMax = 0;
    int curProc;
    int processors[nProcs];
    memset(processors, 0, sizeof(processors));
    for (int i = 0; i < nTasks; i++) {
        curProc = argmbw(processors, nProcs, tasks[i], curMax);
        processors[curProc] += tasks[i];
        if (processors[curProc] > curMax) {
            curMax = processors[curProc];
        }
    }
    return max(processors, nProcs); 
}

// Wrapper functions that call the core 
// algorithms and print the algorithm results
// to the console
void opt(int tasks[], int nTasks, int nProcs) {
    int sortedTasks[nTasks];
    sort(tasks, sortedTasks, nTasks);
    printf("-opt %d\n", optCore(sortedTasks, nTasks, nProcs));
}

void lw(int tasks[], int nTasks, int nProcs) {
    printf("-lw  %d\n", lwCore(tasks, nTasks, nProcs));
}

void lwd(int tasks[], int nTasks, int nProcs) {
    int sortedTasks[nTasks];
    sort(tasks, sortedTasks, nTasks);
    printf("-lwd %d\n", lwCore(sortedTasks, nTasks, nProcs));
}

void bw(int tasks[], int nTasks, int nProcs) {
    printf("-bw  %d\n", bwCore(tasks, nTasks, nProcs));
}

void bwd(int tasks[], int nTasks, int nProcs) {
    int sortedTasks[nTasks];
    sort(tasks, sortedTasks, nTasks);
    printf("-bwd %d\n", bwCore(sortedTasks, nTasks, nProcs));
}
