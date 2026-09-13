#include "cmd_pool.h"

/* Each block must hold at least a pointer (for the intrusive free list) and be
 * big enough for a parsed command object. Sized generously here. */
#define BLOCK_SIZE 32

typedef struct block { struct block *next; } block_t;

static uint8_t  pool[CMD_POOL_BLOCKS][BLOCK_SIZE] __attribute__((aligned(8)));
static block_t *free_head;
static uint8_t  free_count;
static bool     initialized;

void cmd_pool_init(void) {
    free_head = NULL;
    /* thread the intrusive free list through all blocks */
    for (int i = CMD_POOL_BLOCKS - 1; i >= 0; i--) {
        block_t *b = (block_t *)pool[i];
        b->next = free_head;
        free_head = b;
    }
    free_count = CMD_POOL_BLOCKS;
    initialized = true;
}

void *cmd_pool_alloc(void) {
    if (!initialized || free_head == NULL) return NULL;  /* exhausted */
    block_t *b = free_head;
    free_head = free_head->next;   /* pop head */
    free_count--;
    return (void *)b;
}

bool cmd_pool_free(void *blk) {
    if (!initialized || blk == NULL) return false;
    uint8_t *p = (uint8_t *)blk;
    /* validate: in range and on a block boundary (Topic 4 pool validation) */
    if (p < pool[0] || p >= pool[0] + sizeof(pool)) return false;   /* foreign */
    if ((size_t)(p - pool[0]) % BLOCK_SIZE != 0) return false;      /* not aligned */
    block_t *b = (block_t *)blk;
    b->next = free_head;           /* push head */
    free_head = b;
    free_count++;
    return true;
}

uint8_t cmd_pool_available(void) { return free_count; }
