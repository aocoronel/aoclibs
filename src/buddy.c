#pragma once

#include "buddy.h"

size_t buddy_order_size(unsigned order) {
	return ((size_t)1) << order;
}

size_t buddy_block_count(unsigned max_order, unsigned order) {
	return ((size_t)1) << (max_order - order);
}

size_t buddy_node_index(unsigned max_order, unsigned order, size_t block_idx) {
	return (((size_t)1) << (max_order - order)) - 1 + block_idx;
}

int buddy_get_bit(uint8_t *map, size_t idx) {
	$assert_nonnull(map);
	return (map[idx >> 3] >> (idx & 7)) & 1;
}

void buddy_set_bit(uint8_t *map, size_t idx, int value) {
	$assert_nonnull(map);
	if (value)
		map[idx >> 3] |= (1u << (idx & 7));
	else
		map[idx >> 3] &= ~(1u << (idx & 7));
}

Buddy_Page *buddy_new_page(unsigned page_order) {
	size_t tree_nodes = (((size_t)1) << (page_order - Buddy_Min_Order + 1)) - 1;

	size_t map_bytes = (tree_nodes + 7) / 8;
	size_t total_size = buddy_order_size(page_order);

	size_t allocated_size = sizeof(Buddy_Page) + /* struct */
	                        map_bytes + /* split_map */
	                        map_bytes + /* free_map */
	                        total_size; /* base */

	// This is a classical place where you would have to allocate for 4 different things, check
	// if they are not NULL, free everything previous to it and then return failure.
	// We prevent this poor habit, that modern developers solve with `defer`. The true best solution
	// is to make one syscall and allocate everything into a single buffer, which is equally
	// only one syscall to free.
	// No malloc/free chase, the only place this API ever allocates is here.
	// TODO: when sanitizer is enabled, poison the base memory when returning
	uint8_t *buff = (uint8_t *)calloc(1, allocated_size);
	$catch(!buff) return NULL;

	Buddy_Page *p = (Buddy_Page *)buff;
	buff += sizeof(Buddy_Page);

	p->max_order = page_order;
	p->total_size = total_size;
	p->next = NULL;

	p->split_map = buff;
	buff += map_bytes;

	p->free_map = buff;
	buff += map_bytes;

	p->base = buff;

	for (unsigned i = 0; i < Buddy_Max_Orders; ++i)
		p->free_lists[i] = NULL;

	Buddy_Block *root = (Buddy_Block *)p->base;
	root->next = NULL;

	p->free_lists[page_order] = root;

	size_t root_idx = buddy_node_index(page_order, page_order, 0);

	buddy_set_bit(p->free_map, root_idx, 1);
	buddy_set_bit(p->split_map, root_idx, 0);

	return p;
}

void buddy_freelist_push(Buddy_Block **head, Buddy_Block *blk) {
	$assert_nonnull(head);
	$assert_nonnull(blk);
	blk->next = *head;
	*head = blk;
}

Buddy_Block *buddy_freelist_pop(Buddy_Block **head) {
	$assert_nonnull(head);
	Buddy_Block *r = *head;
	if (r) *head = r->next;
	return r;
}

int buddy_freelist_remove(Buddy_Block **head, Buddy_Block *target) {
	$assert_nonnull(head);
	$assert_nonnull(target);
	Buddy_Block *prev = NULL;
	Buddy_Block *cur = *head;

	while (cur) {
		if (cur == target) {
			if (prev)
				prev->next = cur->next;
			else
				*head = cur->next;
			return 1;
		}
		prev = cur;
		cur = cur->next;
	}

	return 0;
}

// TODO: unpoison requested memory
void *buddy_alloc_page(Buddy_Page *p, size_t bytes) {
	$assert_nonnull(p);
	size_t needed = bytes + sizeof(Buddy_Header);

	unsigned order = Buddy_Min_Order;
	while (order < p->max_order && buddy_order_size(order) < needed)
		order++;

	$catch(order > p->max_order) return NULL;

	unsigned current = order;

	while (current <= p->max_order && !p->free_lists[current])
		current++;

	$catch(current > p->max_order) return NULL;

	Buddy_Block *blk = buddy_freelist_pop(&p->free_lists[current]);

	size_t blk_idx = ((uintptr_t)((uint8_t *)blk - p->base)) >> current;

	buddy_set_bit(p->free_map, buddy_node_index(p->max_order, current, blk_idx), 0);

	while (current > order) {
		current--;

		size_t half = buddy_order_size(current);

		Buddy_Block *right = (Buddy_Block *)((uint8_t *)blk + half);

		buddy_freelist_push(&p->free_lists[current], right);

		size_t left_idx = blk_idx << 1;
		size_t right_idx = left_idx + 1;

		buddy_set_bit(p->split_map, buddy_node_index(p->max_order, current + 1, blk_idx), 1);

		buddy_set_bit(p->free_map, buddy_node_index(p->max_order, current, right_idx), 1);

		blk_idx = left_idx;
	}

	Buddy_Header *hdr = (Buddy_Header *)blk;
	hdr->page = p;
	hdr->order = order;

	return (void *)(hdr + 1);
}

void *buddy_alloc(Buddy *b, size_t size_bytes) {
	$assert_nonnull(b);
	$assert(size_bytes > 0, "invalid allocation size");

	Buddy_Page *p = b->begin;

	while (p) {
		void *r = buddy_alloc_page(p, size_bytes);
		if (r) return r;
		p = p->next;
	}

	unsigned needed_order = Buddy_Min_Order;
	while (buddy_order_size(needed_order) < size_bytes + sizeof(Buddy_Header))
		needed_order++;

	unsigned page_order = Buddy_Default_Page_Order;
	if (needed_order > page_order) page_order = needed_order;

	Buddy_Page *newp = buddy_new_page(page_order);
	$catch(!newp) return NULL;

	if (b->end)
		b->end->next = newp;
	else
		b->begin = newp;

	b->end = newp;

	return buddy_alloc_page(newp, size_bytes);
}

void buddy_free_page(Buddy_Page *p, void *ptr, unsigned order) {
	$assert_nonnull(p);
	$assert_nonnull(ptr);
	uint8_t *blk = (uint8_t *)ptr;

	size_t blk_idx = ((uintptr_t)(blk - p->base)) >> order;

	while (order < p->max_order) {
		size_t buddy_idx = blk_idx ^ 1;

		size_t buddy_offset = buddy_idx << order;
		Buddy_Block *buddy = (Buddy_Block *)(p->base + buddy_offset);

		size_t buddy_node = buddy_node_index(p->max_order, order, buddy_idx);

		if (!buddy_get_bit(p->free_map, buddy_node)) break;

		if (!buddy_freelist_remove(&p->free_lists[order], buddy)) break;

		buddy_set_bit(p->free_map, buddy_node, 0);

		size_t parent_idx = blk_idx >> 1;

		buddy_set_bit(p->split_map, buddy_node_index(p->max_order, order + 1, parent_idx), 0);

		blk_idx = parent_idx;
		blk = p->base + (blk_idx << (order + 1));

		order++;
	}

	Buddy_Block *final_blk = (Buddy_Block *)blk;

	buddy_freelist_push(&p->free_lists[order], final_blk);

	buddy_set_bit(p->free_map, buddy_node_index(p->max_order, order, blk_idx), 1);
}

void buddy_dealloc(void *ptr) {
	$assert_nonnull(ptr);

	Buddy_Header *hdr = ((Buddy_Header *)ptr) - 1;

	buddy_free_page(hdr->page, hdr, hdr->order);
}

void buddy_free(Buddy *b) {
	$assert_nonnull(b);

	Buddy_Page *p = b->begin;

	while (p) {
		Buddy_Page *next = p->next;
		$assert(next, "double free attempt");
		free(p);
		p = next;
	}

	b->begin = NULL;
	b->end = NULL;
}
