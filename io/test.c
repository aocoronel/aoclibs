#include <a_io_printfc.h>

int main(int argc, char *argv[]) {
        printfc(FATAL, "a terrible error\n");
        printfc(DEBUG, "%d < %d\n", 10, 20);
        printfc(INFO, "using default configuration\n");
        printfc(WARN, "your software is out-of-date\n");
        printfc(ERROR, "file not found\n");
        PDEBUG("debug message, ran if NDEBUG is not set\n");
        return 0;
}
