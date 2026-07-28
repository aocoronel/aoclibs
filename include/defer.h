#ifndef AOC_DEFER_H_
#define AOC_DEFER_H_

#define $$defer_cat(a, b) a##b
#define $defer_cat(a, b) $$defer_cat(a, b)

struct Defer_Context {
    void (*fn)(void *);
    void *arg;
};

AOCDEF void defer_fn(struct Defer_Context *ctx);

#define defer(fn, ptr)                               \
    struct Defer_Context $defer_cat(_defer_var_, __LINE__) \
        __attribute__((cleanup(defer_fn))) = { (void (*)(void *))(fn), (void *)(ptr) }

#ifdef AOC_IMPLEMENTATION
AOCDEF void _defer_cleanup(struct Defer_Context *ctx) {
    if (ctx->fn) ctx->fn(ctx->arg);
}
#endif

#endif // AOC_DEFER_H_
