#include "../base/attributes.h"
#include "iprint.h"
#include <ctype.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

AOCLIBS_PREFIX void aoc_iprint(const char *msg, int indent) {
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0) {
                w.ws_col = 80;
        }
        const int WIDTH = w.ws_col;

        fprintf(stderr, "%-*s", indent, "");
        int line_pos = indent;

        const char *START = msg;
        const char *END = msg;

        while (*END) {
                if (*END == '\n') {
                        fputc('\n', stderr);
                        fprintf(stderr, "%*s", indent, "");
                        line_pos = indent;
                        END++;
                        continue;
                }
                while (isspace((unsigned char)*END))
                        END++;

                if (*END == ' ') break;

                START = END;
                while (*END && !isspace((unsigned char)*END))
                        END++;

                int word_len = END - START;

                if (line_pos + word_len > WIDTH && line_pos > indent) {
                        fprintf(stderr, "\n%*s", indent, "");
                        line_pos = indent;
                }

                fprintf(stderr, "%.*s", word_len, START);
                line_pos += word_len;

                if (*END && line_pos < WIDTH) {
                        fputc(' ', stderr);
                        line_pos++;
                }
        }
        fputc(' ', stderr);
}
