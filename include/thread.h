#ifndef AOC_THREAD_H_
#define AOC_THREAD_H_

#include "base.h"

#include <execinfo.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

// thread.h is a tiny multi-threading framework that lets you write code almost like single-threaded
// programs, and shall be enabled with -DTHREAD.
//
// Based on the ideas presented in here: https://www.dgtlgrove.com/p/multi-core-by-default
//
// When enabled, the main() entry point is initialized with all threads. No thread creation is required.

// Assume modern computers can't go more than 64 CPUs
#define MAX_THREAD_COUNT 64

#ifdef THREAD
// Synchronize all threads
#define ckp pthread_barrier_wait(&GLOBAL_BARRIER);

// Synchronize variables
#define $broadcast(output, val)                                                \
	do {                                                                       \
		static_assert(sizeof(*(output)) <= MAX_ALIGNMENT, "output too large"); \
		if_thrd0 {                                                             \
			__typeof__((val)) _tmp = (val);                                    \
			broadcast_variable_thrd0(&_tmp, (output), sizeof(*(output)));      \
		}                                                                      \
		else {                                                                 \
			broadcast_variable(NULL, (output), sizeof(*(output)));             \
		}                                                                      \
	} while (0)

// main() is fake
#define main(...)                                                         \
	_main(int argc, char *argv[], char *env[]);                           \
	void *thread_call_main(void *arg);                                    \
	int main(int argc, char *argv[], char *env[]) {                       \
		struct __main_args {                                              \
			size_t id;                                                    \
			char **argv;                                                  \
			char **env;                                                   \
			int argc;                                                     \
		};                                                                \
		signal(SIGSEGV, debug_signal_handler);                            \
		signal(SIGINT, debug_signal_handler);                             \
		signal(SIGFPE, debug_signal_handler);                             \
		signal(SIGILL, debug_signal_handler);                             \
		signal(SIGBUS, debug_signal_handler);                             \
		signal(SIGSYS, debug_signal_handler);                             \
		struct __main_args args[MAX_THREAD_COUNT];                        \
		pthread_t tid[MAX_THREAD_COUNT];                                  \
                                                                          \
		THREAD_COUNT = nproc();                                           \
		barrier_init(&GLOBAL_BARRIER, THREAD_COUNT);                      \
                                                                          \
		for (size_t i = 0; i < THREAD_COUNT; i++) {                       \
			args[i] = (struct __main_args){ i, argv, env, argc };         \
			int ret = thread_create(&tid[i], thread_call_main, &args[i]); \
			if (ret != 0) {                                               \
				fprintf(stderr, "Failed to create thread %zu\n", i);      \
			}                                                             \
		}                                                                 \
                                                                          \
		for (size_t i = 0; i < THREAD_COUNT; i++) {                       \
			void *status;                                                 \
			pthread_join(tid[i], &status);                                \
			if (status) abort();                                          \
		}                                                                 \
		return 0;                                                         \
	}                                                                     \
	int _main(int argc, char *argv[], char *env[])
#else
#define ckp
#define $broadcast(output, val) *(output) = (val)
#define main(...)                                   \
	_main(int argc, char *argv[], char *env[]);     \
	int main(int argc, char *argv[], char *env[]) { \
		signal(SIGSEGV, debug_signal_handler);      \
		signal(SIGINT, debug_signal_handler);       \
		signal(SIGFPE, debug_signal_handler);       \
		signal(SIGILL, debug_signal_handler);       \
		signal(SIGBUS, debug_signal_handler);       \
		signal(SIGSYS, debug_signal_handler);       \
		return _main(argc, argv, env);              \
	}                                               \
	int _main(int argc, char *argv[], char *env[])
#endif

// Thread 0 is special, and is responsible for critical codepath
#ifdef THREAD
#define if_thrd0 if (is_thrd0())
#define ckp_thrd0 \
	ckp;          \
	if (is_thrd0())
#define $thread(...) __VA_ARGS__
#else
#define if_thrd0 if (true)
#define ckp_thrd0 if (true)
#define $thread(...)
#endif

// I put several hours in this thing, but I couldn't find any good solution for dynamic dispatch
// No matter using global variables, static variables, atomics, states or whatever, the implementation
// is never safe for recursion and nesting, which causes deadlocks
//
// This is only guaranteed to work properly if you use this were you are sure it will never recurse
//
// For simplicity I left it as a global variable, because it doesn't matter
#if defined(THREAD) && defined(AOC_ENABLE_C11_FEATURES)
extern $atomic(size_t) GLOBAL_TASK;
size_t global_task_init($atomic(size_t) * task);
#define $dtask(count)                                              \
	for (size_t it = global_task_init(&GLOBAL_TASK); it < (count); \
	     it = atomic_fetch_add(&GLOBAL_TASK, 1))
#else
#define $dtask(count) for (size_t it = 0; it < (count); it++)
#endif

// Use this instead of dtask(), if you need to scale. You have to synchronize the threads if necessary.
// The disavantage of this one compared to dtask() is that some ranges may be slower than others,
// potentially making some threads have to wait.
#define $task(range) for (size_t it = (range)->begin; it < (range)->end; it++)

struct Range {
	size_t begin, end;
};

#define MAX_ALIGNMENT 32
extern unsigned char GLOBAL_BROADCAST[MAX_ALIGNMENT];

// Number of CPUs. Use thread_count()
extern size_t THREAD_COUNT;

#ifdef __TINYC__
extern pthread_key_t THREAD_ID;
#else
extern thread_local size_t THREAD_ID;
#endif

extern pthread_barrier_t GLOBAL_BARRIER;

const size_t thread_count(void);
const size_t thread_id(void);

// Stolen from: https://www.dgtlgrove.com/p/multi-core-by-default
const Range thread_range(size_t count);

unsigned long nproc(void);

// Short-hand for pthread_create(), omitting the attribute
//
// errno << pthread_create (EAGAIN)
int thread_create(pthread_t *restrict tid, void *(*routine)(void *), void *restrict arg);

// Short-hand for pthread_barrier_init(), omitting the attribute
int barrier_init(pthread_barrier_t *restrict barrier, unsigned int count);

bool is_thrd0(void);

void broadcast_variable_thrd0(void *val, void *output, size_t size);
void broadcast_variable(void *val, void *output, size_t size);

void debug_signal_handler(int sig);

#ifdef AOC_IMPLEMENTATION
#include "thread.c"
#endif // AOC_IMPLEMENTATION

#if 0
#include "arena.h"
int main(int argc, char **argv) {
    { // Tasks
        Range range = thread_range(101);
        size_t local_sum = 0;

        $task(&range) {
            local_sum += it;
        }

        $dtask(101) {
            local_sum += it;
        }

        $dtask(101) {
            local_sum += it;
        }

        static $atomic(size_t) sum = 0;
        atomic_fetch_add(&sum, local_sum);

        ckp_thrd0 {
            // (50 * 101) * 3
            $assert(sum == 15150);
        }
    }

    { // Syncing data
        int my_int;
        $broadcast(&my_int, 34);
        $assert(my_int == 34);

        int my_int2;
        $broadcast(&my_int2, 34);
        $assert(my_int2 == 34);

        struct Slice {
            const char *data;
            size_t len;
        };
        struct Slice my_slice1;
        $broadcast(&my_slice1, ((struct Slice){ "dog", 4 }));
        $assert(my_slice1.len == 4);
        $assert(strcmp(my_slice1.data, "dog") == 0);
        struct Slice my_slice2;
        $broadcast(&my_slice2, ((struct Slice){ "bear", 5 }));
        $assert(my_slice2.len == 5);
        $assert(strcmp(my_slice2.data, "bear") == 0);
    }

    { // Thread-safe Arena
        static Arena tbuff = { 0 };

        // Return same buffer for all threads
        // one malloc
        // This function perform ckp
        int *y = (int *)arena_alloc(&tbuff, sizeof(int));
        int *z = (int *)arena_alloc(&tbuff, sizeof(int));
        if (!y || !z) return 1;
        y[thread_id()] = 69;
        z[thread_id()] = 96;

        ckp;

        // verify that all threads share the same buffer, and they wrote 69 and 96
        for (size_t i = 0; i < thread_count(); i++) {
            $assert(y[i] == 69);
            $assert(z[i] == 96);
        }

        ckp_thrd0 {
            arena_destroy(&tbuff);
            printf("done\n");
        }
    }

    return 0;
}
#endif

#endif // AOC_THREAD_H_
