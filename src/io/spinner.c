#include <aoclibs/io/spinner.h>
#include <stdint.h>

#define SPINNER_NUM_SYMBOLS (sizeof(symbols) / sizeof(symbols[0]))

const char *get_spinner(void) {
  static const char *symbols[] = {"⠁", "⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂"};
  static int_fast8_t counter = 0;
  const char *result = symbols[counter];
  counter = (counter + 1) % SPINNER_NUM_SYMBOLS;
  return result;
}
