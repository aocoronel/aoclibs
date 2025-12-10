#include <aoclibs/io/get_spinner.h>
#include <stdint.h>

#define SPINNER_NUM_SYMBOLS (sizeof(SYMBOLS) / sizeof(SYMBOLS[0]))

const char *io_get_spinner(void) {
        static const char *SYMBOLS[] = {
                "⠁", "⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂"
        };
        static int_fast8_t counter = 0;
        const char *RESULT = SYMBOLS[counter];
        counter = (counter + 1) % SPINNER_NUM_SYMBOLS;
        return RESULT;
}
