#pragma once

#include "buddy.h"

size_t _buddy_order_size(unsigned order) {
	return ((size_t)1) << order;
}

size_t _buddy_block_count(unsigned max_order, unsigned order) {
	return ((size_t)1) << (max_order - order);
}

size_t _buddy_node_index(unsigned max_order, unsigned order, size_t block_idx) {
	return (((size_t)1) << (max_order - order)) - 1 + block_idx;
}

int _buddy_get_bit(uint8_t *map, size_t idx) {
	$assert_nonnull(map);
	return (map[idx >> 3] >> (idx & 7)) & 1;
}

void _buddy_set_bit(uint8_t *map, size_t idx, int value) {
	$assert_nonnull(map);
	if (value)
		map[idx >> 3] |= (1u << (idx & 7));
	else
		map[idx >> 3] &= ~(1u << (idx & 7));
}

buddy_page_t *_buddy_new_page(unsigned page_order) {
	size_t tree_nodes = (((size_t)1) << (page_order - BUDDY_MIN_ORDER + 1)) - 1;

	size_t map_bytes = (tree_nodes + 7) / 8;
	size_t total_size = _buddy_order_size(page_order);

	size_t allocated_size = sizeof(buddy_page_t) + /* struct */
							map_bytes + /* split_map */
							map_bytes + /* free_map */
							total_size; /* base */

	uint8_t *buff = (uint8_t *)calloc(1, allocated_size);
	$catch(!buff) return NULL;

	buddy_page_t *p = (buddy_page_t *)buff;
	buff += sizeof(buddy_page_t);

	p->max_order = page_order;
	p->total_size = total_size;
	p->next = NULL;

	p->split_map = buff;
	buff += map_bytes;

	p->free_map = buff;
	buff += map_bytes;

	p->base = buff;

	for (unsigned i = 0; i < BUDDY_MAX_ORDERS; ++i)
		p->free_lists[i] = NULL;

	buddy_block_t *root = (buddy_block_t *)p->base;
	root->next = NULL;

	p->free_lists[page_order] = root;

	size_t root_idx = _buddy_node_index(page_order, page_order, 0);

	_buddy_set_bit(p->free_map, root_idx, 1);
	_buddy_set_bit(p->split_map, root_idx, 0);

	return p;
}

void _buddy_free(buddy_page_t *p) {
	$assert_nonnull(p);
	free(p);
}

void _buddy_freelist_push(buddy_block_t **head, buddy_block_t *blk) {
	$assert_nonnull(head);
	$assert_nonnull(blk);
	blk->next = *head;
	*head = blk;
}

buddy_block_t *_buddy_freelist_pop(buddy_block_t **head) {
	$assert_nonnull(head);
	buddy_block_t *r = *head;
	if (r) *head = r->next;
	return r;
}

int _buddy_freelist_remove(buddy_block_t **head, buddy_block_t *target) {
	$assert_nonnull(head);
	$assert_nonnull(target);
	buddy_block_t *prev = NULL;
	buddy_block_t *cur = *head;

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

void *_buddy_alloc_page(buddy_page_t *p, size_t bytes) {
	$assert_nonnull(p);
	size_t needed = bytes + sizeof(buddy_header_t);

	unsigned order = BUDDY_MIN_ORDER;
	while (order < p->max_order && _buddy_order_size(order) < needed)
		order++;

	$catch(order > p->max_order) return NULL;

	unsigned current = order;

	while (current <= p->max_order && !p->free_lists[current])
		current++;

	$catch(current > p->max_order) return NULL;

	buddy_block_t *blk = _buddy_freelist_pop(&p->free_lists[current]);

	size_t blk_idx = ((uintptr_t)((uint8_t *)blk - p->base)) >> current;

	_buddy_set_bit(p->free_map, _buddy_node_index(p->max_order, current, blk_idx), 0);

	while (current > order) {
		current--;

		size_t half = _buddy_order_size(current);

		buddy_block_t *right = (buddy_block_t *)((uint8_t *)blk + half);

		_buddy_freelist_push(&p->free_lists[current], right);

		size_t left_idx = blk_idx << 1;
		size_t right_idx = left_idx + 1;

		_buddy_set_bit(p->split_map, _buddy_node_index(p->max_order, current + 1, blk_idx), 1);

		_buddy_set_bit(p->free_map, _buddy_node_index(p->max_order, current, right_idx), 1);

		blk_idx = left_idx;
	}

	buddy_header_t *hdr = (buddy_header_t *)blk;
	hdr->page = p;
	hdr->order = order;

	return (void *)(hdr + 1);
}

void *buddy_alloc(Buddy *a, size_t size_bytes) {
	$assert_nonnull(a);

	// Should we assert this instead?
	$catch(size_bytes == 0) return NULL;

	buddy_page_t *p = a->begin;

	while (p) {
		void *r = _buddy_alloc_page(p, size_bytes);
		if (r) return r;
		p = p->next;
	}

	unsigned needed_order = BUDDY_MIN_ORDER;
	while (_buddy_order_size(needed_order) < size_bytes + sizeof(buddy_header_t))
		needed_order++;

	unsigned page_order = BUDDY_DEFAULT_PAGE_ORDER;
	if (needed_order > page_order) page_order = needed_order;

	buddy_page_t *newp = _buddy_new_page(page_order);
	$catch(!newp) return NULL;

	if (a->end)
		a->end->next = newp;
	else
		a->begin = newp;

	a->end = newp;

	return _buddy_alloc_page(newp, size_bytes);
}

void _buddy_free_page(buddy_page_t *p, void *ptr, unsigned order) {
	$assert_nonnull(p);
	$assert_nonnull(ptr);
	uint8_t *blk = (uint8_t *)ptr;

	size_t blk_idx = ((uintptr_t)(blk - p->base)) >> order;

	while (order < p->max_order) {
		size_t buddy_idx = blk_idx ^ 1;

		size_t buddy_offset = buddy_idx << order;
		buddy_block_t *buddy = (buddy_block_t *)(p->base + buddy_offset);

		size_t buddy_node = _buddy_node_index(p->max_order, order, buddy_idx);

		if (!_buddy_get_bit(p->free_map, buddy_node)) break;

		if (!_buddy_freelist_remove(&p->free_lists[order], buddy)) break;

		_buddy_set_bit(p->free_map, buddy_node, 0);

		size_t parent_idx = blk_idx >> 1;

		_buddy_set_bit(p->split_map, _buddy_node_index(p->max_order, order + 1, parent_idx), 0);

		blk_idx = parent_idx;
		blk = p->base + (blk_idx << (order + 1));

		order++;
	}

	buddy_block_t *final_blk = (buddy_block_t *)blk;

	_buddy_freelist_push(&p->free_lists[order], final_blk);

	_buddy_set_bit(p->free_map, _buddy_node_index(p->max_order, order, blk_idx), 1);
}

void buddy_dealloc(void *ptr) {
	$assert_nonnull(ptr);

	buddy_header_t *hdr = ((buddy_header_t *)ptr) - 1;

	_buddy_free_page(hdr->page, hdr, hdr->order);
}

void buddy_free(Buddy *a) {
	$assert_nonnull(a);

	buddy_page_t *p = a->begin;

	while (p) {
		buddy_page_t *next = p->next;
		_buddy_free(p);
		p = next;
	}

	a->begin = NULL;
	a->end = NULL;
}
