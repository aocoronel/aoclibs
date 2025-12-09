#include <aoclibs/debug/printfc.h>

int main(int argc, char *argv[]) {
        printfc(FATAL, "an irrecoverable error occurred!\n");
        printfc(INFO, "using default configuration\n");
        printfc(ERROR, "file not found\n");
        printfc(WARN, "your software is out-of-date\n");
        printfc(CRITICAL, "the system is compromised!\n");
        pdbg("debug message, ran if NDEBUG is not set\n");
        ptrace("trace message, ran if NDEBUG is not set\n");
        return 0;
}
