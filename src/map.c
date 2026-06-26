#pragma once

#include "map.h"

int map_binary_search(struct _Map *map, const unsigned char k) {
	$assert_nonnull(map);
	int left = 0, right = (int)map->len;
	while (left < right) {
		int mid = left + (right - left) / 2;
		if ((unsigned char)map->data[mid].key < k) {
			left = mid + 1;
		} else {
			right = mid;
		}
	}
	return left;
}

void _map_dump(struct _Map *restrict m, rc *restrict buff, const int indent, const int depth) {
	$assert_nonnull(m);
	$assert_nonnull(buff);
	da_reserve(buff, depth + 1);
	$range(0, m->len, i) {
		buff->data[depth] = m->data[i].key;
		buff->data[depth + 1] = '\0';

		fputn(stderr, indent, ' ');
		fprintf(stderr, "%c -> %s\n", m->data[i].key, buff->data);

		_map_dump(&m->data[i], buff, indent + 3, depth + 1);
	}
}

struct _Map *_map_prepare(Arena *restrict arena,
						  struct _Map *restrict map,
						  const Slice slice,
						  const size_t sizeof_value) {
	$assert_nonnull(arena);
	$assert_nonnull(map);
	struct _Map *curr = map;
	size_t cursor = 0;

	while (cursor < (slice).len) {
		unsigned char k = (unsigned char)(slice).data[cursor];

		int pos = map_binary_search(curr, k);

		if (pos < (int)curr->len && (unsigned char)curr->data[pos].key == k) {
			curr = &curr->data[pos];
			cursor++;
			continue;
		}

		_dar_reserve(arena, curr, curr->len + 1, sizeof_value);

		memmove(&curr->data[pos + 1], &curr->data[pos], (curr->len - pos) * sizeof(curr->data[0]));

		struct _Map next = { 0 };
		next.key = k;

		curr->data[pos] = next;
		curr->len++;

		curr = &curr->data[pos];
		cursor++;
	}
	return curr;
}

struct _Map *_map_get(struct _Map *map, const Slice slice) {
	$assert_nonnull(map);
	struct _Map *result = map;
	size_t cursor = 0;

	while (result && cursor < (slice).len) {
		unsigned char k = (unsigned char)(slice).data[cursor];

		int pos = map_binary_search(result, k);

		if (pos < (int)result->len && (unsigned char)result->data[pos].key == k) {
			result = &result->data[pos];
			cursor++;
		} else {
			result = NULL;
		}
	}

	return (result && result->occupied) ? result : NULL;
}

#ifdef TUNIT
typedef struct TUnitMap {
	size_t len;
	size_t cap;
	struct TUnitMap *data;

	bool occupied;
	char key;
	int value;
} TUnitMap;

TEST(map_test) {
	TUnitMap map = { 0 };
	Arena arena = { 0 };

	map_insert(&arena, &map, $slice("hello"), 20);

	TUnitMap *m = map_get(&map, $slice("hello"));
	$tassert(m->value == 20, "wrong value");

	map_set(&map, $slice("hello"), 23);
	$tassert(m->value == 23, "wrong value");

	TUnitMap *m2 = map_prepare(&arena, &map, $slice("hello2"));
	$tassert(m2->occupied == false, "should not be occupied");

	// rc buff = { 0 };
	// map_dump(&map, &buff);
	// if (!map_delete(&map, $slice("hello"))) {
	//     printf("Failed to delete!\n");
	// }
	arena_destroy(&arena);
}
#endif
