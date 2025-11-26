#include <a_mem_fn_strcmp.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
        int err = a_strcmp("dog", "dog");
        printf("%d", err);
        return 0;
}
