#pragma once

#include "base.h"
#include "da.h"
#include "io.h"
#include "slices.h"
#include "map.h"
#include "arena.h"
#include <stdio.h>

internal inline int _map_bs(Map *m, unsigned char k) {
    int left = 0, right = m->len;

    while (left < right) {
        int mid = left + (right - left) / 2;
        if ((unsigned char)m->data[mid].key < k) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return left;
}

internal inline void _map_insert(Arena *arena, Map *m, Slice *s, void *key, size_t cursor) {
    if (cursor == s->len) {
        m->value = key;
        return;
    }

    unsigned char k = (unsigned char)s->data[cursor];

    int pos = _map_bs(m, k);

    if (pos < m->len && (unsigned char)m->data[pos].key == k) {
        return _map_insert(arena, &m->data[pos], s, key, cursor + 1);
    }

    int left = 0, right = m->len;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if ((unsigned char)m->data[mid].key < k)
            left = mid + 1;
        else
            right = mid;
    }

    dar_reserve(arena, m, m->len + 1);
    memmove(&m->data[left + 1], &m->data[left], (m->len - left) * sizeof(Map));

    Map c = { 0 };
    c.key = k;

    m->data[left] = c;
    m->len++;

    _map_insert(arena, &m->data[left], s, key, cursor + 1);
}

void map_insert(Arena *arena, Map *m, Slice *s, void *key) {
    _map_insert(arena, m, s, key, 0);
}

internal inline void _map_dump(Map *m, int indent, rc *buff, int depth) {
    da_reserve(buff, depth + 1);
    foreach (m, i) {
        buff->data[depth] = m->data[i].key;
        buff->data[depth + 1] = '\0';

        fputn(stderr, indent, ' ');
        fprintf(stderr, "%c -> %s\n", m->data[i].key, buff->data);

        _map_dump(&m->data[i], indent + 3, buff, depth + 1);
    }
}

void map_dump(Map *m) {
    rc buff = { 0 };
    _map_dump(m, 0, &buff, 0);
    da_free(&buff);
}

internal inline Map *null _map_find(Map *m, Slice *s, size_t cursor) {
    if (cursor == s->len) return m;
    unsigned char k = (unsigned char)s->data[cursor];

    int pos = _map_bs(m, k);

    if (pos < m->len && (unsigned char)m->data[pos].key == k) {
        return _map_find(&m->data[pos], s, cursor + 1);
    }
    return NULL;
}

Map *null map_find(Map *m, Slice *s) {
    return _map_find(m, s, 0);
}

bool map_set(Map *m, Slice *s, void *value) {
    Map *c = map_find(m, s);
    if (!c) return false;

    c->value = value;
    return true;
}

bool map_delete(Map *m, Slice *s) {
    Map *c = map_find(m, s);
    if (!c) return false;

    c->value = NULL;
    return true;
}

