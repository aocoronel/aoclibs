#include <aoclibs/printfc.h>

int main(void) {
        printfc_fatal("an irrecoverable error occurred!\n");
        printfc_info("using default configuration\n");
        printfc_error("file not found\n");
        printfc_warn("your software is out-of-date\n");
        printfc_critical("the system is compromised!\n");
        PDBG("debug message, ran if NDEBUG is not set\n");
        PTRACE("trace message, ran if NDEBUG is not set\n");
        return 0;
}
