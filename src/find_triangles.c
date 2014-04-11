#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <glib.h>
#include <errno.h>
#include <assert.h>
#include "find_triangles.h"
// The following size is the number of bits used
#define TABLE_BITS 10
#define TABLE_SIZE (1 << TABLE_BITS)
#define MASKED_ENTRY(x) { x & (TABLE_SIZE - 1) }

#define DEBUG 0
#define debug_print(fmt, ...)                                           \
    do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)

void
alist_add(adj_list* orig, uint32_t elem) {
    if (orig->count >= 16 && !(orig->count & (orig->count - 1))) {
        uint32_t *new = malloc(2 * orig->count * sizeof(uint32_t));
        if (new == NULL)
            exit(EXIT_FAILURE);
        uint32_t *old = orig->list;
        for (uint32_t i=0; i < orig->count; i++) {
            new[i] = old[i];
        }
        free(orig->list);
        orig->list = new;
    }
    (orig->list)[orig->count] = elem;
    orig->count++;
}

int cmp(const void *a, const void *b){
    const uint32_t *x = a, *y = b;
    if(*x > *y)
        return 1;
    else
        return(*x < *y) ? -1: 0;
}


/* sort and remove all duplicates from an array */
uint32_t *
acompact(uint32_t count, uint32_t *array) {
    qsort(array, count, sizeof(uint32_t), cmp);
    uint32_t distinct = 1;
    for (uint32_t i=1; i<count; i++)
        if (array[i-1] != array[i])
            distinct++;
    uint32_t *new = malloc(distinct * sizeof(uint32_t));
    if (new == NULL)
        exit(EXIT_FAILURE);
    distinct = 1;
    new[0]=array[0];
    for (uint32_t i=1; i<count; i++)
        if (array[i-1] != array[i])
            new[distinct++] = array[i];
    free(array);
    return(new);
}


int
main(void)
{
    char s1[100], s2[100];
    /* Create table of adjacency lists */
    adj_list **table;
    table = calloc(TABLE_SIZE, sizeof(adj_list *));
    if (table == NULL)
        exit(EXIT_FAILURE);
    /* init adjacency lists */
    for (uint32_t v=0; v < TABLE_SIZE; v++) {
        table[v] = malloc(sizeof(adj_list));
        if (table[v] == NULL)
            exit(EXIT_FAILURE);
        table[v]->list = calloc(16, sizeof(uint32_t));
        table[v]->count = 0;
        if (table[v]->list == NULL)
            exit(EXIT_FAILURE);
    }
    while (scanf("%s %s\n", s1, s2) >= 0) {
        uint32_t v1 = atol(s1) & (TABLE_SIZE - 1);
        uint32_t v2 = atol(s2) & (TABLE_SIZE - 1);
        debug_print("%d %d\n", v1, v2);
        alist_add(table[v1], v2);
    }

/* compact and order adjacency list */
    for (uint32_t v=0; v < TABLE_SIZE; v++)
        table[v]->list = acompact(table[v]->count, table[v]->list);


/* output transitive arcs */
    for (uint32_t v=0; v < TABLE_SIZE; v++) {
        debug_print("v=%d, degree=%d\n", v, table[v]->count);
        uint32_t * adj_v = table[v]->list;
        uint32_t outdegree_v = table[v]->count;
        for (uint32_t i_w = 0; i_w<outdegree_v; i_w++) {
            uint32_t w = adj_v[i_w];
            uint32_t z;
            bool transitive = FALSE;
            for (uint32_t i_z = 0; i_z<outdegree_v; i_z++) {
                z = adj_v[i_z];
                debug_print("Testing v=%d w=%d z=%d \n", v, w, z);
                if (z != w && bsearch(&w, table[z]->list, table[z]->count, sizeof(uint32_t), cmp) != NULL) {
                    for (uint32_t i=0; i<table[z]->count; i++)
                        debug_print("%9d;%d\n", i, (table[z]->list)[i]);
                    transitive = TRUE;
                }
            }
            if (transitive) {
                printf("%d %d via %d\n", v, w, z);
            }
        }
    }
}
