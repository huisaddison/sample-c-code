#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>

typedef struct heap {
    int* data;
    int count;
    int maxlen;
} Heap;

typedef struct doubleheap {
    Heap MaxH;
    Heap MinH;
} DoubleHeap;

int swap(Heap* h, int k, int j) {
    int* d = h->data;
    int x; /* Placeholder */
    x = d[k];
    d[k] = d[j];
    d[j] = x;
    return 1;
}

bool cmp(int i, int j, bool mx) {
    if (mx) {
        return i > j;
    } else {
        return i < j;
    }
}

Heap init(int n) {
    Heap h;
    h.data = malloc(sizeof(int) * (n/2+1));
    h.count = 0;
    h.maxlen = n;
    return h;
}

int peek(Heap* h) {
    return h->data[0];
}

int pop(Heap* h, bool mx) {
    int* d = h->data;
    int k = 0;
    int ret = d[k];
    d[k] = d[--h->count]; // Decrement and read
    // Sift down
    while (true) {
        if (2*k+1 < h->count && 2*k+2 < h->count) {
            int j;
            if (cmp(d[2*k+1], d[2*k+2], mx)) {
                j = 2*k+1;
            } else {
                j = 2*k+2;
            }
            if (cmp(d[j], d[k], mx)) {
                swap(h, k, j);
            } else {
                break;
            }
            k = j;
        } else if (2*k+1 < h->count) {
            if (cmp(d[2*k+1], d[k], mx)) {
                swap(h, k, 2*k+1);
            } else {
                break;
            }
            k = 2*k+1;
        } else {
            break;
        }
    }
    return ret;
}

int push(Heap* h, int x, bool mx) {
    if (h->count == h->maxlen) {
        return 0;
    }
    int* d = h->data;
    int k = h->count;
    d[h->count++] = x; // Write and increment
    // Bubble up
    while (k > 0) {
        int j = (k - 1) / 2;     /* Get parent index */
        if (cmp(d[k], d[j], mx)) {
            swap(h, j, k);
            k = j;
        } else {
            break;
        }
    }
    return 1;
}

int destroy(Heap* h) {
    free(h->data);
    return 1;
}

int pushD(DoubleHeap* dh, int x) {
    if ((dh->MaxH).count == 0 &&
        (dh->MinH).count == 0) {
        push(&(dh->MaxH), x, true);
    } else if ((dh->MaxH).count == 0) {
        int mn = peek(&(dh->MinH));
        if (x > mn) {
            push(&(dh->MinH), x, false);
            int j = pop(&(dh->MinH), false);
            push(&(dh->MaxH), j, true);         // Take care of stuff below this
        } else {
            push(&(dh->MaxH), x, true);
        }
    } else if ((dh->MinH).count == 0) {
        int mx = peek(&(dh->MaxH));
        if (x >= mx) {
            push(&(dh->MinH), x, false);
        } else {
            push(&(dh->MaxH), x, true);
            int j = pop(&(dh->MaxH), true);
            push(&(dh->MinH), j, false);
        }
    } else {
        int mx = peek(&(dh->MaxH));
        int mn = peek(&(dh->MinH));
        if ((dh->MaxH).count > (dh->MinH).count) {
            if ((mx <= x) && (x <= mn)) {
                push(&(dh->MinH), x, false);
            } else if (x < mx) {
                push(&(dh->MaxH), x, true);
                int j = pop(&(dh->MaxH), true);
                push(&(dh->MinH), j, false);
            } else {
                push(&(dh->MinH), x, false);
            }
        } else if ((dh->MaxH).count < (dh->MinH).count) {
            if ((mx <= x) && (x <= mn)) {
                push(&(dh->MaxH), x, true);
            } else if (x < mx) {
                push(&(dh->MaxH), x, true);
            } else {
                push(&(dh->MinH), x, false);
                int j = pop(&(dh->MinH), false);
                push(&(dh->MaxH), j, true);
            }
        } else {
            if ((mx <= x) && (x <= mn)) {
                push(&(dh->MaxH), x, true);
            } else if (x < mx) {
                push(&(dh->MaxH), x, true);
            } else {
                push(&(dh->MinH), x, false);
            }
        }
    }
    return 1;
}

DoubleHeap initD(int n) {
    DoubleHeap dh;
    dh.MinH = init(n);
    dh.MaxH = init(n);
    return dh;
}

int destroyD(DoubleHeap* dh) {
    destroy(&(dh->MaxH));
    destroy(&(dh->MinH));
    return 1;
}

double median(DoubleHeap* dh) {
    double db;
    if ((dh->MaxH).count > (dh->MinH).count) {
        db = (double) peek(&(dh->MaxH));
    } else if ((dh->MaxH).count < (dh->MinH).count) {
        db = (double) peek(&(dh->MinH));
    } else {
        db = (((double) peek(&(dh->MaxH)) / 2) +
              ((double) peek(&(dh->MinH)) / 2));
    }
    return db;
}

int main(){
    int n;
    scanf("%d",&n);
    DoubleHeap dh = initD(n);
    int *a = malloc(sizeof(int) * n);
    for(int a_i = 0; a_i < n; a_i++){
        scanf("%d",&a[a_i]);
        pushD(&dh, a[a_i]);
        printf("%.1f\n", median(&dh));
    }
    free(a);
    destroyD(&dh);
    return 0;
}

