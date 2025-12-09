#include <aoclibs/mem/str.h>
#include <aoclibs/io/print.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

void io_print_indent(const char *msg, int indent) {
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0) {
                w.ws_col = 80;
        }
        int width = w.ws_col;

        fprintf(stderr, "%-*s", indent, "");
        int line_pos = indent;

        const char *start = msg;
        const char *end = msg;

        while (*end) {
                while (isspace((unsigned char)*end))
                        end++;

                if (*end == ' ') break;

                start = end;
                while (*end && !isspace((unsigned char)*end))
                        end++;

                int word_len = end - start;

                if (line_pos + word_len > width && line_pos > indent) {
                        fprintf(stderr, " %*s", indent, "");
                        line_pos = indent;
                }

                fprintf(stderr, "%.*s", word_len, start);
                line_pos += word_len;

                if (*end && line_pos < width) {
                        fputc(' ', stderr);
                        line_pos++;
                }
        }
        fputc(' ', stderr);
}
