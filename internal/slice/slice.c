#include "slice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

SliceHeader slice_new(size_t elem_size, size_t initial_capacity) {
  SliceHeader s;
  s.elem_size = elem_size;
  s.len = 0;
  s.cap = initial_capacity;
  s.arr = malloc(elem_size * initial_capacity);
  srand(time(NULL));
  return s;
}

void slice_free(SliceHeader* slicePtr) {
  free(slicePtr->arr);
  slicePtr->arr = NULL;
  slicePtr->len = 0;
  slicePtr->cap = 0;
}

void slice_append(SliceHeader* slicePtr, void* elem) {
  SliceHeader s = *slicePtr;
  if (s.len >= s.cap) {
    s.cap = s.cap == 0 ? 4 : s.cap * 2;
    void* new_arr = realloc(s.arr, s.cap * s.elem_size);
    if (!new_arr) {
      fprintf(stderr, "FATAL: out of memory\n");
      exit(1);
    }
    s.arr = new_arr;
  }
  char* dest = (char*)s.arr + s.len * s.elem_size;
  memcpy(dest, elem, s.elem_size);
  s.len++;
  *slicePtr = s;
}

void* slice_get(SliceHeader* slicePtr, size_t index) {
  SliceHeader s = *slicePtr;
  if (index >= s.len) {
    fprintf(stderr, "FATAL: index %zu out of range\n", index);
    exit(1);
  }
  return (char*)s.arr + index * s.elem_size;
}

SliceHeader slice_splice(SliceHeader* slicePtr, size_t from, size_t to) {
  SliceHeader original = *slicePtr;
  if (from > original.len || to > original.len || from > to) {
    fprintf(stderr, "FATAL: bad splice [%zu:%zu]\n", from, to);
    exit(1);
  }
  SliceHeader new = original;
  new.len = to - from;
  new.cap = original.cap - from;
  new.arr = (char*)original.arr + from * original.elem_size;
  return new;
}

static void generic_swap(void* a, void* b, size_t size) {
  char tmp[64];
  if (size <= 64) {
    memcpy(tmp, a, size);
    memcpy(a, b, size);
    memcpy(b, tmp, size);
  } else {
    char* t = malloc(size);
    memcpy(t, a, size);
    memcpy(a, b, size);
    memcpy(b, t, size);
    free(t);
  }
}

static long partition(SliceHeader* s, long low, long high,
                      int (*cmp)(const void*, const void*)) {
  char* base = (char*)s->arr;
  size_t es = s->elem_size;

  long pivot_idx = low + rand() % (high - low + 1);

  generic_swap(base + pivot_idx * es, base + high * es, es);

  char* pivot = base + high * es;
  long i = low;

  for (long j = low; j < high; j++) {
    if (cmp(base + j * es, pivot) <= 0) {  // cmp с двумя аргументами!
      generic_swap(base + i * es, base + j * es, es);
      i++;
    }
  }

  generic_swap(base + i * es, base + high * es, es);
  return i;
}

static void quick_sort_start(SliceHeader* s, long low, long high,
                             int (*cmp)(const void*, const void*)) {
  if (low < high) {
    long pi = partition(s, low, high, cmp);
    quick_sort_start(s, low, pi - 1, cmp);
    quick_sort_start(s, pi + 1, high, cmp);
  }
}

void slice_quick_sort(SliceHeader* slicePtr,
                      int (*cmp)(const void*, const void*)) {
  if (slicePtr->len <= 1) return;
  quick_sort_start(slicePtr, 0, (long)slicePtr->len - 1, cmp);
}
