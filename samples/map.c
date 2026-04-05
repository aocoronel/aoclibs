#define AOCLIBS_IMPLEMENTATION
#include "printfc.h"
#include "map.h"

int main(int argc, char *argv[]) {
    Arena arena = { 0 };
    Map m = { 0 };

    int x = 20;
    int y = 30;
    int z = 100;
    map_insert(&arena, &m, &slice("dragon"), &x);
    map_insert(&arena, &m, &slice("dungeon"), &y);
    map_insert(&arena, &m, &slice("castle"), &z);
    map_insert(&arena, &m, &slice("dracula"), &z);
    map_insert(&arena, &m, &slice("dread"), &z);
    map_insert(&arena, &m, &slice("dreedful"), &z);

    Map *c = map_find(&m, &slice("dragon"));
    if (!c) {
        printfc_debug("c was null\n");
        return 0;
    }

    printf("key = %c, value = %d\n", c->key, *(int *)c->value);
    map_set(&m, &slice("dragon"), &z);
    printf("key = %c, value = %d\n", c->key, *(int *)c->value);

    fflush(stdout);
    fflush(stderr);

    map_dump(&m);

    arena_destroy(&arena);

    return 0;
}
