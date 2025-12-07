#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

#define LOAD_FACTOR 0.75

typedef struct Set {
    size_t sizeOfData;
    size_t len;
    size_t capacity;
    void **table;
} Set;

Set *Set_create(size_t capacity, size_t sizeOfData) {
    Set *set = calloc(1, sizeof(Set));
    assert(set);
    set->sizeOfData = sizeOfData;
    set->len = 0;
    assert(capacity);
    set->capacity = capacity;
    set->table = calloc(set->capacity, sizeof(void *));
    return set;
}

void Set_add(Set *set, void *data) {
    assert(set);
    if (set->len >= set->capacity * LOAD_FACTOR) {
        printf("Warning: rehashing set!\n");
        size_t capacity = set->capacity * 2;
        void **table = calloc(capacity, sizeof(void *));
        for (int i = 0; i < set->capacity; i++) {
            if (set->table[i]) {
                size_t index = hash(set->table[i], set->sizeOfData, capacity);
                while (table[index]) {
                    index = (index + 1) % capacity;
                }
                table[index] = set->table[i];
            }
        }
        free(set->table);
        set->table = table;
        set->capacity = capacity;
    }
    size_t index = hash(data, set->sizeOfData, set->capacity);
    while (set->table[index]) {
        if (memcmp(set->table[index], data, set->sizeOfData) == 0) {
            return;
        }
        index = (index + 1) % set->capacity;
    }
    void *item = copy_hashable(data, set->sizeOfData);
    set->table[index] = item;
    set->len++;
}

bool Set_find(const Set *set, void *data) {
    size_t index = hash(data, set->sizeOfData, set->capacity);
    while (set->table[index]) {
        if (memcmp(set->table[index], data, set->sizeOfData) == 0) {
            return true;
        }
        index = (index + 1) % set->capacity;
    }
    return false;
}

void **Set_items(const Set *set) {
    void **items = calloc(set->len, sizeof(void *));
    assert(items);
    size_t index = 0;
    for (int i = 0; i < set->capacity; i++) {
        if (set->table[i]) {
            items[index++] = set->table[i];
        }
    }
    return items;
}

void Set_free(void *set_void) {
    Set *set = (Set *)set_void;
    for (int i = 0; i < set->capacity; i++) {
        if (set->table[i]) {
            free(set->table[i]);
        }
    }
    free(set->table);
    free(set);
}

void *Set_return(const void *src) { return src; }

void *Set_copy(const void *src) {
    assert(src);
    Set *srcSet = (Set *)src;
    Set *destSet = Set_create(srcSet->capacity, srcSet->sizeOfData);
    for (size_t i = 0; i < srcSet->capacity; i++) {
        if (srcSet->table[i]) {
            Set_add(destSet, srcSet->table[i]);
        }
    }
    return destSet;
}