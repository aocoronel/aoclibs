#ifndef AOCLIBS_CLI_PROGRAM_INFO_H
#define AOCLIBS_CLI_PROGRAM_INFO_H

#define clarg(i, a, d, c, t) { .id = i, .name = a, .desc = d, .completion = c, .type = t }

enum CLIArgumentType {
        ReqArg,
        NReqArg,
};

/*
 * Define the program argument details
*/
typedef struct {
        const char *id;
        const char *name;
        const char *desc;
        const char *completion;
        enum CLIArgumentType type;
} CLIArgument;

/*
 * Define the program command details
*/
typedef struct {
        const char *cmd;
        const CLIArgument *args;
        const char *desc;
} CLICommand;

/*
 * Define the program flag details
*/
typedef struct {
        const char *short_opt;
        const char *long_opt;
        const CLIArgument *args;
        const char *desc;
} CLIOption;

/*
 * Define the program info
*/
typedef struct {
        int flagc; // Flag count
        int cmdc; // Command count
        int argc; // Argument count
        const char *name;
        const char *desc;
        const char *usage;
        CLIArgument *args;
        CLICommand *commands;
        CLIOption *flags;
} CLIProgram;

#endif
