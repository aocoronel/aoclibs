#define _POSIX_C_SOURCE 200809L

#include <stdatomic.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_THREAD_COUNT 64

#ifndef thread_local
#define thread_local _Thread_local
#endif

size_t THREAD_COUNT;
thread_local size_t THREAD_ID;

size_t thread_count(void) {
    return THREAD_COUNT;
}

size_t thread_id(void) {
    return THREAD_ID;
}

char *_main(int argc, char *argv[], char *env[]);

struct __main_args {
    size_t id;
    char **argv;
    char **env;
    int argc;
};

void *thread_work(void *arg) {
    struct __main_args *args = (struct __main_args *)arg;
    THREAD_ID = args->id;
    return _main(args->argc, args->argv, args->env);
}

typedef struct TRange TRange;

struct TRange {
    size_t min, max;
};

pthread_barrier_t GLOBAL_BARRIER;

// Stolen from: https://www.dgtlgrove.com/p/multi-core-by-default
#define thread_calculate_range(count, id, t_count)                                \
    ({                                                                            \
        size_t values_per_thread = (count) / (t_count);                           \
        size_t leftover_values_count = (count) % (t_count);                       \
        size_t thread_has_leftover = ((id) < leftover_values_count);              \
        size_t leftovers_before_this_thread_idx =                                 \
            (thread_has_leftover ? (id) : leftover_values_count);                 \
        size_t thread_first_value_idx =                                           \
            (values_per_thread * (id) + leftovers_before_this_thread_idx);        \
        size_t thread_opl_value_idx =                                             \
            (thread_first_value_idx + values_per_thread + !!thread_has_leftover); \
        (TRange){ thread_first_value_idx, thread_opl_value_idx };                 \
    })

TRange thread_range(size_t values_count) {
    return thread_calculate_range(values_count, thread_id(), thread_count());
}

TRange thread_range_by(size_t values_count, size_t thread_count) {
    return thread_calculate_range(values_count, thread_id(), thread_count);
}

unsigned long nproc(void) {
#ifdef _SC_NPROCESSORS_ONLN
    {
        long int nprocs = sysconf(_SC_NPROCESSORS_ONLN);
        return nprocs == 0 ? 1 : nprocs;
    }
#endif

#ifdef _SC_NPROCESSORS_CONF
    {
        long int nprocs = sysconf(_SC_NPROCESSORS_CONF);
        return nprocs == 0 ? 1 : nprocs;
    }
#endif

#if !(defined __GLIBC__ && defined __linux__) && defined HW_NCPU
    {
        int nprocs;
        size_t len = sizeof(nprocs);
        static int mib[][2] = {
#ifdef HW_NCPUONLINE
            { CTL_HW, HW_NCPUONLINE },
#endif
            { CTL_HW, HW_NCPU       }
        };
        for (int i = 0; i < countof(mib); i++) {
            if (sysctl(mib[i], countof(mib[i]), &nprocs, &len, NULL, 0) == 0 &&
                len == sizeof(nprocs) && 0 < nprocs)
                return nprocs;
        }
    }
#endif

    return 1;
}

#define thread_set(value_for_all, value_for_master) \
    is_master() ? (value_for_master) : (value_for_all)

// errno << pthread_create (EAGAIN)
int thread_create(pthread_t *restrict tid, void *(*routine)(void *), void *restrict arg) {
    return pthread_create(tid, NULL, routine, arg);
}

int barrier_init(pthread_barrier_t *restrict barrier, unsigned int count) {
    return pthread_barrier_init(barrier, NULL, count);
}

int barrier_sync_all(void) {
    return pthread_barrier_wait(&GLOBAL_BARRIER);
}

size_t task_index(atomic_size_t *task) {
    return atomic_fetch_add(task, 1);
}

bool task_done(size_t idx, size_t count) {
    return (idx >= count);
}

bool is_master(void) {
    return thread_id() == 0;
}

void sync_cstr(size_t idx, char **output) {
    static char *data = NULL;
    if (is_master()) {
        data = *output;
    }
    barrier_sync_all();
    *output = data;
}

void sync_ptr(size_t idx, void **output) {
    static void *data = NULL;
    if (is_master()) {
        data = *output;
    }
    barrier_sync_all();
    *output = data;
}

void sync_int(size_t idx, int *output) {
    static int data = 0;
    if (is_master()) {
        data = *output;
    }
    barrier_sync_all();
    *output = data;
}

void sync_usize(size_t idx, size_t *output) {
    static size_t data = 0;
    if (is_master()) {
        data = *output;
    }
    barrier_sync_all();
    *output = data;
}

#define dynamic_task(count, ...)                                              \
    do {                                                                      \
        static _Atomic size_t _task_counter;                                  \
        _task_counter = 0;                                                    \
        barrier_sync_all();                                                   \
        for (size_t it = task_index(&_task_counter); !task_done(it, (count)); \
             it = task_index(&_task_counter)) {                               \
            __VA_ARGS__;                                                      \
        }                                                                     \
    } while (0);

#define uniform_task(count, ...)                              \
    do {                                                      \
        TRange _range = thread_range((count));                \
        for (size_t it = _range.min; it < _range.max; it++) { \
            __VA_ARGS__                                       \
        }                                                     \
    } while (0)

#define main()                                                       \
    main(int argc, char *argv[], char *env[]) {                      \
        struct __main_args args[MAX_THREAD_COUNT];                   \
        pthread_t tid[MAX_THREAD_COUNT];                             \
                                                                     \
        THREAD_COUNT = nproc();                                      \
        barrier_init(&GLOBAL_BARRIER, THREAD_COUNT);                 \
                                                                     \
        for (size_t i = 0; i < THREAD_COUNT; i++) {                  \
            args[i] = (struct __main_args){ i, argv, env, argc };    \
            int ret = thread_create(&tid[i], thread_work, &args[i]); \
            if (ret != 0) {                                          \
                fprintf(stderr, "Failed to create thread %zu\n", i); \
            }                                                        \
        }                                                            \
                                                                     \
        int ok = 0;                                                  \
        for (size_t i = 0; i < THREAD_COUNT; i++) {                  \
            void *status;                                            \
            pthread_join(tid[i], &status);                           \
            if (status) {                                            \
                fprintf(stderr, "fatal: %s\n", (char *)status);      \
                ok = 1;                                              \
            }                                                        \
        }                                                            \
        return ok;                                                   \
    }                                                                \
    char *_main(int argc, char *argv[], char *env[])

int main() {
    // Ranged-tasks
    int x[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    uniform_task(10, {
        printf("t[%zu]: %zu=%d\n", thread_id(), it, x[it]);
        NULL;
    });
    barrier_sync_all();

    // Syncing data
    char *my_string = thread_set(NULL, "unique");
    sync_cstr(0, &my_string);
    assert(strcmp(my_string, "unique") == 0);

    // Individual tasks
    dynamic_task(4, {
        printf("Hello! [%zu]\n", it);
        NULL;
    });

    printf("done\n");

    return NULL;
}
