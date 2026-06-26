#pragma once

#include "stack.h"

Stack *stack_init(size_t size) {
	size_t total = sizeof(Stack) + sizeof(size_t) + size;

	char *base = (char *)malloc(total);
	$catch(!base) return NULL;

	Stack *s = (Stack *)base;
	s->base_alloc = base;

	size_t *header = (size_t *)(base + sizeof(Stack));
	*header = 0;

	s->buff = header;
	s->used_size = 0;
	s->size = size;

	return s;
}

void *stack_alloc(Stack *s, size_t size) {
	$assert_nonnull(s);

	size_t needed = size + sizeof(size_t);

	$catch(s->used_size + needed > s->size) return NULL;

	char *begin = (char *)s->base_alloc + sizeof(Stack) + sizeof(size_t);
	char *alloc_ptr = begin + s->used_size;

	size_t *new_header = (size_t *)(alloc_ptr + size);
	*new_header = size;

	s->used_size += needed;
	s->buff = new_header;

	return alloc_ptr;
}

void stack_rewind(Stack *s, void *mark) {
	$assert_nonnull(s);
	$assert_nonnull(mark);
	while (1) {
		if (s->buff == mark) break;
		if (s->used_size == 0) {
			break;
		}
		stack_free(s);
	}
}

void *stack_mark(Stack *s) {
	return s->buff;
}

void stack_free(Stack *s) {
	$assert_nonnull(s);

	size_t *header = (size_t *)s->buff;
	size_t last_size = *header;

	$catch(last_size == 0) return;

	size_t total = last_size + sizeof(size_t);
	$assert(s->used_size >= total);

	s->used_size -= total;

	char *begin = (char *)s->base_alloc + sizeof(Stack) + sizeof(size_t);

	if (s->used_size == 0) {
		s->buff = (size_t *)((char *)s->base_alloc + sizeof(Stack));
	} else {
		char *prev_header_pos = begin + s->used_size - sizeof(size_t);
		s->buff = (size_t *)prev_header_pos;
	}
}
