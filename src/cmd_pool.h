/* cmd_pool.h -- O(1) fixed-block allocator for queued command objects.
 * NFR-1: no dynamic allocation in steady state; pool sized to worst-case
 * queue depth (8). Intrusive free list, no malloc. (Topic 4.) */
#ifndef CMD_POOL_H
#define CMD_POOL_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define CMD_POOL_BLOCKS 8          /* worst-case queue depth (NFR-1) */

void  cmd_pool_init(void);
void *cmd_pool_alloc(void);         /* returns a block, or NULL if exhausted */
bool  cmd_pool_free(void *blk);     /* returns false on invalid/double free  */
uint8_t cmd_pool_available(void);   /* free blocks remaining */

#endif
