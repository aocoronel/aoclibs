#ifndef AOCLIBS_COLORS_H_
#define AOCLIBS_COLORS_H_

#ifndef CONFIG_TTY_COLORS
#define CONFIG_TTY_COLORS 1
#endif // CONFIG_TTY_COLORS

// TTY Colors

#if CONFIG_TTY_COLORS == 0
#define COLOR_RESET ""

#define COLOR_BOLD ""
#define COLOR_UNDERLINE ""
#define COLOR_BOLD_UNDERLINE ""

#define COLOR_BLACK ""
#define COLOR_RED ""
#define COLOR_GREEN ""
#define COLOR_YELLOW ""
#define COLOR_BLUE ""
#define COLOR_MAGENTA ""
#define COLOR_CYAN ""
#define COLOR_WHITE ""

#define COLOR_BLACK_BRIGHT ""
#define COLOR_RED_BRIGHT ""
#define COLOR_GREEN_BRIGHT ""
#define COLOR_YELLOW_BRIGHT ""
#define COLOR_BLUE_BRIGHT ""
#define COLOR_MAGENTA_BRIGHT ""
#define COLOR_CYAN_BRIGHT ""
#define COLOR_WHITE_BRIGHT ""
#elif CONFIG_TTY_COLORS == 1
#define COLOR_RESET "\x1b[0m"

#define COLOR_BOLD "\x1b[1m"
#define COLOR_UNDERLINE "\x1b[4m"
#define COLOR_BOLD_UNDERLINE "\x1b[1;4m"

#define COLOR_BLACK "\x1b[30m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_WHITE "\x1b[37m"

#define COLOR_BLACK_BRIGHT "\x1b[90m"
#define COLOR_RED_BRIGHT "\x1b[91m"
#define COLOR_GREEN_BRIGHT "\x1b[92m"
#define COLOR_YELLOW_BRIGHT "\x1b[93m"
#define COLOR_BLUE_BRIGHT "\x1b[94m"
#define COLOR_MAGENTA_BRIGHT "\x1b[95m"
#define COLOR_CYAN_BRIGHT "\x1b[96m"
#define COLOR_WHITE_BRIGHT "\x1b[97m"
#endif // CONFIG_TTY_COLORS

#endif // AOCLIBS_COLORS_H_
