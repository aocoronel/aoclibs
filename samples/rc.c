#include <stdio.h>
#define AOCLIBS_STRIP_PREFIX
#include "rc.h"
#include "da.h"

char buff[100];
rc stack_cstr1 = rcs_bnew(buff, 0);

int main(int argc, char *argv[]) {
    rc heap_cstr = rc_new(100);

    char *heap_buff = malloc(100);
    rc heap_cstr_3 = rc_bnew(heap_buff, 0, 100);

    rc stack_cstr_2 = rcs_new(100);

    rc_cat(&heap_cstr, "hello, world!", 13);
    rc_append(&heap_cstr, "hello, world!", 13);

    printf("%s\n", heap_cstr.data);

    if rcs_cat (&stack_cstr_2, "hello, world!", 13) else printf("failed to append\n");

    printf("%s\n", stack_cstr_2.data);

    return 0;
}
