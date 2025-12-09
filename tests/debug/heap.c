#include <assert.h>
#include <stdlib.h>
#include <a_debug_heap.h>

int main(int argc, char *argv[]) {
        int *dog = malloc(10 * sizeof(int));
        free(dog);
        int *cat = malloc(10 * sizeof(int));
        cat = realloc(cat, 100 * sizeof(int));
        cat = NULL;
        char *horse = realloc(cat, 10);
        int *p = malloc(10);
        free(p);
        print_memory_summary();
        assert(count_leaks() == 2);
        return EXIT_SUCCESS;
}
