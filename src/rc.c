#pragma once

#include <stdlib.h>
#include <string.h>
#include "rc.h"
#include "base.h"
#include "cstr.h"
#include "file.h"
#include <stdio.h>
#include <string.h>

Slices split(rc *f, char delim) {
    Slice cur = { .data = f->data, .len = f->len };
    Slices ss = { 0 };

    while (cur.len > 0) {
        const char *pos = memchr(cur.data, delim, cur.len);

        if (!pos) {
            // last segment
            da_append(&ss, cur.data, cur.len);
            break;
        }

        size_t length = pos - cur.data;

        da_append(&ss, cur.data, length);

        // move forward past delimiter
        cur.data = pos + 1;
        cur.len -= length + 1;
    }

    return ss;
}
