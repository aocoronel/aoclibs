/*
 * MIT License
 *
 * Copyright (c) 2025 Augusto Coronel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#pragma once

#include <stdlib.h>

#define PH_BOLD "\x1b[1m"
#define PH_UNDERLINE "\x1b[4m"
#define PH_RESET "\x1b[0m"
#define PH_BOLD_UNDERLINE "\x1b[1;4m"

#define PH_DESC_INDENT 10

/*
 * Define the program command details
 *
 * command, argument, description
 */
struct PProgramCommands {
  const char *cmd;
  const char *argument;
  const char *cmd_desc;
};

/*
 * Define the program flag details
 *
 * short flag, long flag, argument, description
 */
struct PProgramFlag {
  const char *short_flag;
  const char *long_flag;
  const char *argument;
  const char *desc;
};

/*
 * Define the program info
 *
 * flag count, command count, program name, program description, program usage,
 * PProgramCommands, PProgramFlag
 */
typedef struct {
  int flagc;
  int cmdc;
  const char *name;
  const char *desc;
  const char *usage;
  struct PProgramCommands *commands;
  struct PProgramFlag *flags;
} PProgramInfo;

/*
 * Prints help message
*/
void printh(PProgramInfo program_info);
