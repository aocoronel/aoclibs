#ifndef AOC_BUDDY_H_
#define AOC_BUDDY_H_

#include "base.h"

// Buddy Allocator
//
// This works similarly to the Arena allocator. Whenever the backing buffer the buddy allocator uses
// is out of space, we just create a new page.

enum {
	Buddy_Min_Order = 3, // 2^3 = 8 bytes
	Buddy_Default_Page_Order = 15, // 2^15 = 32768 bytes
	Buddy_Max_Orders = 32,
};

struct Buddy_Block {
	Buddy_Block *next;
};

struct Buddy_Page {
	struct Buddy_Page *next;
	uint8_t *base;
	uint8_t *split_map;
	uint8_t *free_map;
	Buddy_Block *free_lists[Buddy_Max_Orders];
	unsigned max_order;
	size_t total_size;
};

struct Buddy_Header {
	Buddy_Page *page;
	unsigned order;
};

struct Buddy {
	Buddy_Page *begin;
	Buddy_Page *end;
};

AOCDEF Buddy_Block *buddy_freelist_pop(Buddy_Block **head);
AOCDEF Buddy_Page *buddy_new_page(unsigned page_order);
AOCDEF int buddy_freelist_remove(Buddy_Block **restrict head, Buddy_Block *restrict target);
AOCDEF int buddy_get_bit(uint8_t *map, size_t idx);
AOCDEF size_t buddy_block_count(unsigned max_order, unsigned order);
AOCDEF size_t buddy_node_index(unsigned max_order, unsigned order, size_t block_idx);
AOCDEF size_t buddy_order_size(unsigned order);
AOCDEF void *buddy_alloc_page(Buddy_Page *p, size_t bytes);
AOCDEF void buddy_free_page(Buddy_Page *restrict p, void *restrict ptr, unsigned order);
AOCDEF void buddy_freelist_push(Buddy_Block **restrict head, Buddy_Block *restrict blk);
AOCDEF void buddy_set_bit(uint8_t *map, size_t idx, int value);

// Public API
AOCDEF void *buddy_alloc(Buddy *b, size_t size_bytes);
AOCDEF void buddy_dealloc(void *ptr);
AOCDEF void buddy_free(Buddy *b);

// TEST_BEGIN
#ifdef TUNIT
SKIP_TEST(buddy_alloc) {
	Buddy a = { 0 };

	void *p4 = buddy_alloc(&a, 513);
	void *p1 = buddy_alloc(&a, 64);
	void *p2 = buddy_alloc(&a, 128);
	void *p3 = buddy_alloc(&a, 300);

	buddy_dealloc(p2);
	buddy_dealloc(p1);
	buddy_dealloc(p3);
	buddy_dealloc(p4);

	// buddy_free(&a);
}
#endif
// TEST_END

#endif // AOC_BUDDY_H_
