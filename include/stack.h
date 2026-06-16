#ifndef AOCLIBS_STACK_H_
#define AOCLIBS_STACK_H_

#include "base.h"

typedef struct {
	void *base_alloc;
	void *buff;
	size_t used_size;
	size_t size;
} Stack;

#define stack_new(s, type) stack_alloc((s), sizeof(type))
Stack *stack_init(size_t size);

void *stack_alloc(Stack *s, size_t size);
void stack_free(Stack *s);
void stack_rewind(Stack *restrict s, void *restrict mark);
void *stack_mark(Stack *s);
void stack_free(Stack *s);

#define stack_deinit(s)        \
	do {                       \
		free((s)->base_alloc); \
		(s) = NULL;            \
	} while (0)

#endif

// int main(void) {
//     Stack *ptr = stack_init(1 << 20);
//     assert(ptr != NULL);
//
//     void *mark = stack_mark(ptr);
//     int *p = stack_new(ptr, int);
//     int *p1 = stack_new(ptr, int);
//
//     assert(p);
//     assert(p1);
//
//     *p = 10;
//
//     printf("%d\n", *p);
//
//     stack_rewind(ptr, mark);
//     assert(ptr->used_size == 0);
//
//     stack_deinit(ptr);
//     assert(!ptr);
//
//     return 0;
// }
