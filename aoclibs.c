#include "aoclibs.h"

#include "./base/arena.c"
#include "./base/assert.c"
#include "./base/panic.c"
#include "./base/str.c"
#include "./base/vec.c"

#ifdef AOCLIBS_ALL
#include "./modules/iprint.c"
#include "./modules/cli.c"
#include "./modules/spinner.c"
#include "./modules/file.c"
#else

#ifdef AOCLIBS_SPINNER
#include "./modules/spinner.c"
#endif

#ifdef AOCLIBS_CLI
#include "./modules/cli.c"
#endif

#ifdef AOCLIBS_FILE
#include "./modules/file.c"
#endif

#endif
