#include <aoclibs/error.h>

#define E(name, msg) [name] = msg,
const char *const ErrNo[] = {
#include <aoclibs/__errno.h>
};
#undef E

#define E(name, msg) const Err Err##name = { name, ErrNo[name] };
#include <aoclibs/__errno.h>
#undef E
