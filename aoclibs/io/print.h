#ifndef AOCLIBS_IO_PRINT_H
#define AOCLIBS_IO_PRINT_H

/*
 * Prints indented message
 *
 * If the size of the terminal cannot be obtained, fallback to 80 columns
*/
void io_print_indent(const char *msg, int indent);

#endif
