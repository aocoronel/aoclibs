#include <aoclibs/debug/heap.h>
#include <aoclibs/tunit.h>
#include <stdlib.h>

TEST(debug_heap) {
        int *dog = malloc(10 * sizeof(int));
        free(dog);
        int *cat = malloc(10 * sizeof(int));
        cat = realloc(cat, 100 * sizeof(int));
        cat = NULL;
        char *horse = realloc(cat, 10);
        int *p = malloc(10);
        free(p);
        debug_memory_summary(TUNIT_LOG_FILE);
        TASSERT(debug_count_leaks() == 2, "this should have two leaks");
}
