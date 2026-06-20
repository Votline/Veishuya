#ifndef SLICE_H
#define SLICE_H

#include <stddef.h>

typedef struct {
  void* arr;
  size_t len;
  size_t cap;
  size_t elem_size;
} SliceHeader;

SliceHeader slice_new(size_t elem_size, size_t initial_capacity);
void slice_free(SliceHeader* slicePtr);
void slice_append(SliceHeader* slicePtr, void* elem);
void* slice_get(SliceHeader* slicePtr, size_t index);
SliceHeader slice_splice(SliceHeader* slicePtr, size_t from, size_t to);
void slice_quick_sort(SliceHeader* slicePtr,
                      int (*cmp)(const void*, const void*));

#endif
