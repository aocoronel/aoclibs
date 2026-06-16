#ifndef AOCBLIS_BUDDY_H_H
#define AOCBLIS_BUDDY_H_H

#include "base.h"

// Buddy Allocator
//
// This works similarly to the Arena allocator. Whenever the backing buffer the buddy allocator uses
// is out of space, we just create a new page.

#define BUDDY_MIN_ORDER 3 // 2^3 = 8 bytes
#define BUDDY_DEFAULT_PAGE_ORDER 15 // 2^15 = 32768 bytes
#define BUDDY_MAX_ORDERS 32

typedef struct buddy_block {
	struct buddy_block *next;
} buddy_block_t;

typedef struct buddy_page {
	struct buddy_page *next;
	uint8_t *base;
	uint8_t *split_map;
	uint8_t *free_map;
	buddy_block_t *free_lists[BUDDY_MAX_ORDERS];
	unsigned max_order;
	size_t total_size;
} buddy_page_t;

typedef struct {
	buddy_page_t *page;
	unsigned order;
} buddy_header_t;

typedef struct {
	buddy_page_t *begin;
	buddy_page_t *end;
} Buddy;

buddy_block_t *_buddy_freelist_pop(buddy_block_t **head);
buddy_page_t *_buddy_new_page(unsigned page_order);
int _buddy_freelist_remove(buddy_block_t **restrict head, buddy_block_t *restrict target);
int _buddy_get_bit(uint8_t *map, size_t idx);
size_t _buddy_block_count(unsigned max_order, unsigned order);
size_t _buddy_node_index(unsigned max_order, unsigned order, size_t block_idx);
size_t _buddy_order_size(unsigned order);
void *_buddy_alloc_page(buddy_page_t *p, size_t bytes);
void _buddy_free(buddy_page_t *p);
void _buddy_free_page(buddy_page_t *restrict p, void *restrict ptr, unsigned order);
void _buddy_freelist_push(buddy_block_t **restrict head, buddy_block_t *restrict blk);
void _buddy_set_bit(uint8_t *map, size_t idx, int value);

// Public API
void *buddy_alloc(Buddy *a, size_t size_bytes);
void buddy_dealloc(void *ptr);
void buddy_free(Buddy *a);

#endif

// int main(void) {
//     Buddy a = { 0 };
//
//     void *p4 = buddy_alloc(&a, 513);
//     void *p1 = buddy_alloc(&a, 64);
//     void *p2 = buddy_alloc(&a, 128);
//     void *p3 = buddy_alloc(&a, 300);
//
//     buddy_dealloc(p2);
//     buddy_dealloc(p1);
//     buddy_dealloc(p3);
//     buddy_dealloc(p4);
//
//     buddy_free(&a);
//
//     return 0;
// }
