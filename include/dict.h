#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

#define LOAD_FACTOR 0.75
#define DELETED ((void *)-1)

typedef struct DictItem {
    void *key;
    void *value;
} DictItem;

typedef struct Dict {
    size_t sizeOfKey;
    size_t sizeOfValue;
    size_t len;
    size_t capacity;
    DictItem **table;
    void *(*copy_value)(const void *);
    void (*free_value)(void *);
} Dict;

Dict *Dict_create(size_t capacity, size_t sizeOfKey, size_t sizeOfValue,
                  void *(*copy_value)(const void *),
                  void (*free_value)(void *)) {
    Dict *dict = malloc(sizeof(Dict));
    assert(dict);
    dict->sizeOfKey = sizeOfKey;
    dict->sizeOfValue = sizeOfValue;
    dict->len = 0;
    assert(capacity);
    dict->capacity = capacity;
    dict->table = calloc(dict->capacity, sizeof(DictItem *));
    dict->copy_value = copy_value;
    dict->free_value = free_value;
    return dict;
}

void Dict_add(Dict *dict, const void *key, const void *value) {
    assert(dict);
    if (dict->len == dict->capacity * LOAD_FACTOR) {
        printf("Warning: rehashing dictionary!\n");
        size_t capacity = dict->capacity * 2;
        DictItem **table = calloc(capacity, sizeof(DictItem *));
        for (int i = 0; i < dict->capacity; i++) {
            if (dict->table[i] != NULL && dict->table[i] != DELETED) {
                size_t index =
                    hash(dict->table[i]->key, dict->sizeOfKey, capacity);
                while (table[index]) {
                    index = (index + 1) % capacity;
                }
                table[index] = dict->table[i];
            }
        }
        free(dict->table);
        dict->table = table;
        dict->capacity = capacity;
    }
    size_t index = hash(key, dict->sizeOfKey, dict->capacity);
    while (dict->table[index]) {
        if (dict->table[index] == DELETED) {
            index = (index + 1) % dict->capacity;
            continue;
        }
        if (memcmp(dict->table[index]->key, key, dict->sizeOfKey) == 0) {
            if (dict->free_value) {
                dict->free_value(dict->table[index]->value);
            } else {
                free(dict->table[index]->value);
            }
            dict->table[index]->value = NULL;
            if (dict->copy_value) {
                dict->table[index]->value = dict->copy_value(value);
            } else {
                dict->table[index]->value = malloc(dict->sizeOfValue);
                memcpy(dict->table[index]->value, value, dict->sizeOfValue);
            }
            return;
        }
        index = (index + 1) % dict->capacity;
    }

    DictItem *item = malloc(sizeof(DictItem));
    assert(item);
    item->key = copy_hashable(key, dict->sizeOfKey);
    if (dict->copy_value) {
        item->value = dict->copy_value(value);
    } else {
        item->value = calloc(1, dict->sizeOfValue);
        memcpy(item->value, value, dict->sizeOfValue);
    }
    dict->table[index] = item;
    dict->len++;
}

void *Dict_get(const Dict *dict, const void *key) {
    assert(dict);
    void *value = NULL;
    size_t index = hash(key, dict->sizeOfKey, dict->capacity);
    while (dict->table[index]) {
        if (dict->table[index] == DELETED) {
            index = (index + 1) % dict->capacity;
            continue;
        }
        if (memcmp(dict->table[index]->key, key, dict->sizeOfKey) == 0) {
            value = dict->table[index]->value;
            break;
        }
        index = (index + 1) % dict->capacity;
    }
    assert(value);
    return value;
}

void **Dict_keys(const Dict *dict) {
    void **keys = malloc(dict->len * sizeof(void *));
    size_t index = 0;
    for (int i = 0; i < dict->capacity; i++) {
        if (dict->table[i] != NULL && dict->table[i] != DELETED) {
            keys[index++] = dict->table[i]->key;
        }
    }
    return keys;
}

void **Dict_values(const Dict *dict) {
    void **values = malloc(dict->len * sizeof(void *));
    size_t index = 0;
    for (int i = 0; i < dict->capacity; i++) {
        if (dict->table[i] != NULL && dict->table[i] != DELETED) {
            values[index++] = dict->table[i]->value;
        }
    }
    return values;
}

DictItem **Dict_items(const Dict *dict) {
    DictItem **items = malloc(dict->len * sizeof(DictItem *));
    size_t index = 0;
    for (int i = 0; i < dict->capacity; i++) {
        if (dict->table[i] != NULL && dict->table[i] != DELETED) {
            items[index] = dict->table[i];
            index++;
        }
    }
    return items;
}

void *Dict_return(const void *src) { return src; }

void *Dict_copy(const void *src) {
    assert(src);
    Dict *srcDict = (Dict *)src;
    Dict *destDict =
        Dict_create(srcDict->capacity, srcDict->sizeOfKey, srcDict->sizeOfValue,
                    srcDict->copy_value, srcDict->free_value);
    for (size_t i = 0; i < srcDict->capacity; i++) {
        if (srcDict->table[i] != NULL && srcDict->table[i] != DELETED) {
            Dict_add(destDict, srcDict->table[i]->key,
                     srcDict->table[i]->value);
        }
    }
    return destDict;
}

void Dict_free(void *dict_void) {
    Dict *dict = (Dict *)dict_void;
    for (size_t i = 0; i < dict->capacity; i++) {
        if (dict->table[i] != NULL && dict->table[i] != DELETED) {
            free(dict->table[i]->key);
            if (dict->free_value) {
                dict->free_value(dict->table[i]->value);
            } else {
                free(dict->table[i]->value);
            }
            free(dict->table[i]);
        }
    }
    free(dict->table);
    free(dict);
}

bool Dict_find(const Dict *dict, const void *key) {
    assert(dict);
    size_t index = hash(key, dict->sizeOfKey, dict->capacity);
    while (dict->table[index]) {
        if (dict->table[index] == DELETED) {
            index = (index + 1) % dict->capacity;
            continue;
        }
        if (memcmp(dict->table[index]->key, key, dict->sizeOfKey) == 0) {
            return true;
        }
        index = (index + 1) % dict->capacity;
    }
    return false;
}

void Dict_remove(Dict *dict, const void *key) {
    assert(dict);
    size_t index = hash(key, dict->sizeOfKey, dict->capacity);
    while (dict->table[index] != NULL && dict->table[index] != DELETED) {
        if (memcmp(dict->table[index]->key, key, dict->sizeOfKey) == 0) {
            free(dict->table[index]->key);
            free(dict->table[index]->value);
            free(dict->table[index]);
            dict->table[index] = DELETED;
            dict->len--;
            return;
        }
        index = (index + 1) % dict->capacity;
    }
    return;
}