#pragma once

#include "thread.h"

#if defined(THREAD) && defined(AOC_ENABLE_C11_FEATURES)
$atomic(size_t) GLOBAL_TASK;
#endif

unsigned char GLOBAL_BROADCAST[MAX_ALIGNMENT];

size_t THREAD_COUNT;

#ifdef __TINYC__
pthread_key_t THREAD_ID;
#else
thread_local size_t THREAD_ID;
#endif

pthread_barrier_t GLOBAL_BARRIER;

const size_t thread_count(void) {
#ifdef THREAD
	return THREAD_COUNT;
#else
	return 1;
#endif
}

const size_t thread_id(void) {
#ifdef THREAD
#ifdef __TINYC__
	return *pthread_getspecific(THREAD_ID);
#else
	return THREAD_ID;
#endif
#else
	return 0;
#endif
}

const Range thread_range(size_t count) {
#ifdef THREAD
	const size_t id = thread_id();
	const size_t t_count = thread_count();
	const size_t values_per_thread = (count) / (t_count);
	const size_t leftover_values_count = (count) % (t_count);
	const size_t thread_has_leftover = ((id) < leftover_values_count);
	const size_t leftovers_before_this_thread_idx =
	    (thread_has_leftover ? (id) : leftover_values_count);
	const size_t thread_first_value_idx =
	    (values_per_thread * (id) + leftovers_before_this_thread_idx);
	const size_t thread_opl_value_idx =
	    (thread_first_value_idx + values_per_thread + !!thread_has_leftover);
	return (const Range){ thread_first_value_idx, thread_opl_value_idx };
#else
	return (const Range){ 0, count };
#endif
}

int thread_create(pthread_t *restrict tid, void *(*routine)(void *), void *restrict arg) {
	return pthread_create(tid, NULL, routine, arg);
}

int barrier_init(pthread_barrier_t *restrict barrier, unsigned int count) {
	return pthread_barrier_init(barrier, NULL, count);
}

bool is_thrd0(void) {
	return thread_id() == 0;
}

#if defined(THREAD) && defined(AOC_ENABLE_C11_FEATURES)
size_t global_task_init($atomic(size_t) * task) {
	ckp;
	atomic_store(&GLOBAL_TASK, 0);
	ckp;
	return atomic_fetch_add(task, 1);
}
#endif

void broadcast_variable_thrd0(void *val, void *output, size_t size) {
	ckp; // wait all
	memcpy(GLOBAL_BROADCAST, val, size);
	ckp; // read
	memcpy(output, GLOBAL_BROADCAST, size);
}

void broadcast_variable(void *val, void *output, size_t size) {
	ckp; // wait all
	ckp; // read
	memcpy(output, GLOBAL_BROADCAST, size);
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

void debug_signal_handler(int sig) {
	fprintf(stderr, "%s\n", strsignal(sig));
	void *array[128];
	size_t size = backtrace(array, 128);
	backtrace_symbols_fd(array, size, fileno(stderr));
	abort();
}

int _main(int argc, char *argv[], char *env[]);
void *thread_call_main(void *arg) {
	struct __main_args {
		size_t id;
		char **argv;
		char **env;
		int argc;
	};
	struct __main_args *args = (struct __main_args *)arg;
#ifdef __TINYC__
	pthread_key_create(&THREAD_ID, free);
	size_t *id = malloc(sizeof(size_t));
	*id = args->id;
	pthread_setspecific(THREAD_ID, id);
#else
	THREAD_ID = args->id;
#endif
	int ret = _main(args->argc, args->argv, args->env);
	return (void *)(uintptr_t)ret;
}

