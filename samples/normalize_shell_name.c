#include <aoclibs/cli/normalize_shell_name.h>
#include <stdio.h>

int main(void) {
    char funcname[64];
    const char *TEST_CASES[] = {
        "id|url|tag",
        "<TAG>",
        "[TAG]",
        "field=id URL TAG NOTE TITLE",
        "ENTER || TAG || TITLE && NOTE",
        "path/to/directory",
        "%cache",
        "email@go.com",
        "!STRING",
        "^message$",
        "AND * NOT * NEXT",
        "try-next",
        NULL
    };

    for (int i = 0; TEST_CASES[i]; i++) {
        cli_normalize_shell_name(funcname, TEST_CASES[i], sizeof(funcname));
        printf("%-30s -> %s\n", TEST_CASES[i], funcname);
    }

    return 0;
}
