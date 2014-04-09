#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <glib.h>
#include <errno.h>
#include <assert.h>


// The following size is the number of bits used
#define TABLE_BITS 20
#define TABLE_SIZE (1 << TABLE_BITS)
#define MASKED_ENTRY(x) { x & (TABLE_SIZE - 1) }




int
main(void)
{
    char s1[100], s2[100];
    /* Create table of adjacency lists */
    GList **table;
    table = calloc(TABLE_SIZE, sizeof(GList *));
    while (scanf("%s %s\n", s1, s2) >= 0) {
        uint32_t v1 = atoi(s1) & (TABLE_SIZE - 1);
        uint32_t v2 = atoi(s2) & (TABLE_SIZE - 1);
        //printf("%d %d\n", v1, v2);
        table[v1] = g_list_append(table[v1], v2);
/* DEBUG
   GList * list = table[v1];
   for (printf("%d:", v1) ; list!= NULL; list=list->next)
   printf("%d ", (uint32_t *) list->data);
   printf("\n");
*/
    }
    return 0;

/* output transitive arcs */
    for (uint32_t v=0; v < TABLE_SIZE; v++) {
        printf("v=%d\n", v);
        for (GList * lw = table[v]; lw != NULL; lw = lw->next) {
            uint32_t w = *((uint32_t *) lw->data);
            bool transitive = FALSE;
            for (GList * lz = table[w]; lz != NULL; lz = lz->next) {
                uint32_t z = *((uint32_t *) lz->data);
                if (g_list_find(table[v], &z) != NULL) {
                    transitive = TRUE;
                }
            }
            if (transitive) {
                printf("%d %d\n", v, w);
            }
        }
    }
}
