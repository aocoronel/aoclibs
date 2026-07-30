#include "thread.h"
#include "arena.h"
#include "alloc.h"
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

	{ // Thread-safe heap

		// This interface adds thread-safety to allocations, no matter how they are implemented.
		// This will perform the allocation in thrd0 with size * N threads, and redistribute the
		// same buffer across all threads. Each thread knows what parts of the buffer they own.
		// Since all threads have access to memory from other threads, reducing the results is zero
		// cost
		//
		// If you need to share simple data instead, use $broadcast(), which doesn't perform allocations
		//
		// Nicely enough, the interface gives you heap leak detection and easy allocation strategy switch
		//
		// Unfortunately, this isn't Odin or Zig, so just because you don't use a heap allocator,
		// doesn't mean your program doesn't heap allocate. LibC is full of implicit mallocs
		// Be aware that the leak detection is exclusive to this allocator, and standalone mallocs
		// are not catched. This doesn't replace Valgrind, but unlike it, it tells exactly where in
		// the source code the allocation that was never freed happened
		$define_allocator(.general = { LIBC_MALLOC });

		int *y = alloc(sizeof(int));
		int *z = alloc(sizeof(int));
		if (!y || !z) return 1;
		y[thread_id()] = 69;
		z[thread_id()] = 96;

		ckp;

		// verify that all threads share the same buffer, and they wrote 69 and 96
		// Tip: if assertions ever fail on probability, it's likely to be a sync issue
		for (size_t i = 0; i < thread_count(); i++) {
			$assert(y[i] == 69);
			$assert(z[i] == 96);
		}

		ckp;

		y = resize(y, sizeof(int) * 2);

		// Data still available after reallocation
		for (size_t i = 0; i < thread_count(); i++) {
			$assert(y[i] == 69);
		}

		ckp_thrd0 {
			dealloc(y);
			dealloc(z);
			printf("done\n");
			heap_trace_summary(stderr);
		}
	}

	return 0;
}
