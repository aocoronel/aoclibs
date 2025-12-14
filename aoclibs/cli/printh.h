#ifndef AOCLIBS_CLI_PRINTH_H
#define AOCLIBS_CLI_PRINTH_H

#define PRINTH_DESC_INDENT 10
#define PRINTH_CMD_BUFFER 256

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
        int flagc; // Flag count
        int cmdc; // Command count
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
