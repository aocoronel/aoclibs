/*
 * AOCLIBS - Augusto Coronel's C Libraries
 *
 * This library is hosted on Codeberg at https://codeberg.org/aocoronel/aoclibs,
 * and is licensed under the Unlicense License (Public Domain) or MIT License.
*/

/*
 * This project is licensed under Unlicense and MIT. Pick one:
 *
 * =============================================================================
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <https://unlicense.org/>
 * =============================================================================
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
 * =============================================================================
*/

// === Config ===

// Disable colors
// #define FEATURE_DISABLE_COLORS

// === Function Definitions ===

// Enabled with AOCLIBS_IMPLEMENTATION

#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L

#include "./include/libc.h"
#include "./include/base.h"

// Vendor
#include "./vendor/rapidhash.h"

// Standalones
#include "./include/colors.h"
#include "./include/arena.h"
#include "./include/buddy.h"
#include "./include/stack.h"
#include "./include/pp.h"
#include "./include/cstr.h"
#include "./include/fquery.h"

#include "./include/printfc.h"

#include "./include/io.h"

// Dynamic Arrays
#include "./include/da.h"
#include "./include/rc.h"

#include "./include/file.h"
#include "./include/fork.h"
#include "./include/map.h"
#include "./include/hmap.h"

// Libraries
#include "./libs/heap_trace.h" // HEAP_DEBUG
#include "./libs/tunit.h" // TUNIT
#include "./libs/crown.h" // AOCLIBS_CROWN
#include "./libs/debug.h" // AOCLIBS_DEBUG
#include "./libs/spinner.h" // AOCLIBS_SPINNER
#include "./libs/ini.h" // AOCLIBS_SPINNER
