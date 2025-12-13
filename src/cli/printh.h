#ifndef AOCLIBS_CLI_PRINTH_H
#define AOCLIBS_CLI_PRINTH_H

#define PRINTH_BOLD "\x1b[1m"
#define PRINTH_UNDERLINE "\x1b[4m"
#define PRINTH_RESET "\x1b[0m"
#define PRINTH_BOLD_UNDERLINE "\x1b[1;4m"

#define PRINTH_DESC_INDENT 10

/*
 * Define the program command details
 */
typedef struct CmdMetadata {
        const char *cmd;
        const char *args;
        const char *desc;
} CmdMetadata;

/*
 * Define the program flag details
 */
typedef struct OptionMetadata {
        const char *short_opt;
        const char *long_opt;
        const char *args;
        const char *desc;
} OptionMetadata;

/*
 * Define the program info
 */
typedef struct {
        int flagc;
        int cmdc;
        const char *name;
        const char *desc;
        const char *usage;
        CmdMetadata *commands;
        OptionMetadata *flags;
} ProgramInfo;

/*
 * Prints help message
*/
void printh(ProgramInfo program_info);

#endif
