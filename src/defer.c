#pragma once

#include "defer.h"

void _defer_cleanup(struct Defer_Context *ctx) {
	if (ctx->fn) ctx->fn(ctx->arg);
}
