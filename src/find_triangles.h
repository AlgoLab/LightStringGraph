typedef struct {
    uint32_t count;
    uint32_t * list;
} adj_list;

void
alist_add(adj_list*, uint32_t);

int
cmp(const void *, const void *);

void
acompact(adj_list*);
