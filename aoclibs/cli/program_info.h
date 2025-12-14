#ifndef AOCLIBS_CLI_PROGRAM_INFO_H
#define AOCLIBS_CLI_PROGRAM_INFO_H

/*
 * Define the program command details
 */
typedef struct {
        const char *cmd;
        const char *args;
        const char *desc;
} CmdMetadata;

/*
 * Define the program flag details
 */
typedef struct {
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

#endif
