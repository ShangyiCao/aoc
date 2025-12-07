#pragma once
#include <stdlib.h>

size_t hash(const char *hashable, size_t sizeOfHashable, size_t capacity) {
    size_t hashValue = 5381;
    for (size_t i = 0; i < sizeOfHashable; i++) {
        hashValue = (hashValue << 5) + hashable[i];
    }
    return hashValue % capacity;
}

char *copy_hashable(const void *src, size_t sizeOfHashable) {
    char *dest = malloc(sizeOfHashable);
    memcpy(dest, src, sizeOfHashable);
    return dest;
}