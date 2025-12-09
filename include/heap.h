#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HeapItem {
  void *key;
  void *value;
} HeapItem;

typedef struct Heap {
  size_t sizeOfKey;
  size_t sizeOfValue;
  size_t capacity;
  size_t len;
  HeapItem **table;
  int (*compare_value)(const void *, const void *);
  void *(*copy_key)(const void *);
  void (*free_key)(void *);
  void *(*copy_value)(const void *);
  void (*free_value)(void *);
} Heap;

Heap *Heap_create(size_t sizeOfKey, size_t sizeOfValue,
                  int (*compare_value)(const void *, const void *),
                  void *(*copy_key)(const void *), void (*free_key)(void *),
                  void *(*copy_value)(const void *),
                  void (*free_value)(void *)) {
  Heap *heap = malloc(sizeof(Heap));
  assert(heap);
  heap->sizeOfKey = sizeOfKey;
  heap->sizeOfValue = sizeOfValue;
  heap->len = 0;
  heap->capacity = 1000000;
  heap->table = calloc(heap->capacity, sizeof(HeapItem *));
  assert(compare_value);
  heap->compare_value = compare_value;
  heap->copy_key = copy_key;
  heap->free_key = free_key;
  heap->copy_value = copy_value;
  heap->free_value = free_value;
  return heap;
}

void Heap_heapifyUp(Heap *heap, int index) {
  int parent = (index - 1) / 2;
  if (index <= 0) {
    return;
  }
  if (heap->compare_value(heap->table[index]->value,
                          heap->table[parent]->value) > 0) {
    HeapItem *tmp = heap->table[index];
    heap->table[index] = heap->table[parent];
    heap->table[parent] = tmp;
    Heap_heapifyUp(heap, parent);
  }
}

void Heap_heapifyDown(Heap *heap, int index) {
  int leftChild = 2 * index + 1;
  int rightChild = 2 * index + 2;

  int max = index;
  if (leftChild < heap->len &&
      heap->compare_value(heap->table[leftChild]->value,
                          heap->table[max]->value) > 0) {
    max = leftChild;
  }
  if (rightChild < heap->len &&
      heap->compare_value(heap->table[rightChild]->value,
                          heap->table[max]->value) > 0) {
    max = rightChild;
  }

  if (max != index) {
    HeapItem *tmp = heap->table[index];
    heap->table[index] = heap->table[max];
    heap->table[max] = tmp;
    Heap_heapifyDown(heap, max);
  }
}

void Heap_add(Heap *heap, const void *key, const void *value) {
  assert(heap);
  if (heap->len == heap->capacity) {
    size_t capacity = heap->capacity * 2;
    HeapItem **table = calloc(capacity, sizeof(HeapItem *));
    for (size_t i = 0; i < heap->len; i++) {
      table[i] = heap->table[i];
    }
    free(heap->table);
    heap->table = table;
    heap->capacity = capacity;
  }
  HeapItem *item = malloc(sizeof(HeapItem));
  if (heap->copy_key) {
    item->key = heap->copy_key(key);
  } else {
    item->key = calloc(1, heap->sizeOfKey);
    memcpy(item->key, key, heap->sizeOfKey);
  }
  if (heap->copy_value) {
    item->value = heap->copy_value(value);
  } else {
    item->value = calloc(1, heap->sizeOfValue);
    memcpy(item->value, value, heap->sizeOfValue);
  }
  heap->table[heap->len++] = item;
  Heap_heapifyUp(heap, heap->len - 1);
}

HeapItem *Heap_pop(Heap *heap) {
  assert(heap);
  assert(heap->len);
  HeapItem *root = heap->table[0];
  heap->table[0] = heap->table[heap->len - 1];
  heap->len--;
  Heap_heapifyDown(heap, 0);
  return root;
}

void Heap_free(Heap *heap) {
  for (size_t i = 0; i < heap->len; i++) {
    if (heap->table[i]) {
      if (heap->free_key) {
        heap->free_key(heap->table[i]->key);
      } else {
        free(heap->table[i]->key);
      }
      if (heap->free_value) {
        heap->free_value(heap->table[i]->value);
      } else {
        free(heap->table[i]->value);
      }
      free(heap->table[i]);
    }
  }
  free(heap->table);
  free(heap);
}