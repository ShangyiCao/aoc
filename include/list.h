#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

typedef struct ListItem {
    void* data;
    struct ListItem* prev;
    struct ListItem* next;
} ListItem;

typedef struct List {
    ListItem* head;
    ListItem* tail;
    size_t len;
    size_t sizeOfData;
    bool (*compare_data)(const void*, const void*);
    void* (*copy_data)(const void*);
    void (*free_data)(void*);
} List;

List* List_create(size_t sizeOfData,
                  bool (*compare_data)(const void*, const void*),
                  void* (*copy_data)(const void*), void (*free_data)(void*)) {
    List* list = malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
    list->sizeOfData = sizeOfData;
    list->compare_data = compare_data;
    list->copy_data = copy_data;
    list->free_data = free_data;
    return list;
}

void List_append(List* list, void* data) {
    assert(list && data);
    ListItem* item = calloc(1, sizeof(ListItem));
    if (list->copy_data) {
        item->data = list->copy_data(data);
    } else {
        item->data = calloc(1, list->sizeOfData);
        memcpy(item->data, data, list->sizeOfData);
    }
    item->next = NULL;
    item->prev = list->tail;
    if (list->head == NULL) {
        assert(list->tail == NULL);
        list->head = item;
        list->tail = item;
    } else {
        list->tail->next = item;
        list->tail = item;
    }
    list->len++;
}

ListItem* List_get_item(const List* list, int index) {
    assert(list);
    if (index >= 0) {
        assert(index < list->len);
        if (index < list->len / 2 + list->len % 2) {
            ListItem* item = list->head;
            for (int i = 0; i < index; i++) {
                item = item->next;
            }
            return item;
        } else {
            return List_get_item(list, index - list->len);
        }
    } else {
        assert(-index <= list->len);
        if (-index <= list->len / 2 + list->len % 2) {
            ListItem* item = list->tail;
            for (int i = -1; i > index; i--) {
                item = item->prev;
            }
            return item;
        } else {
            return List_get_item(list, list->len + index);
        }
    }
}

void* List_get(const List* list, int index) {
    ListItem* item = List_get_item(list, index);
    return item->data;
}

void* List_return(const void* src) { return src; }

void* List_copy(const void* src) {
    assert(src);
    List* srcList = (List*)src;
    List* destList = List_create(srcList->sizeOfData, srcList->compare_data,
                                 srcList->copy_data, srcList->free_data);
    ListItem* item = srcList->head;
    while (item) {
        List_append(destList, item->data);
        item = item->next;
    }
    return destList;
}

void List_free(void* list_void) {
    assert(list_void);
    List* list = (List*)list_void;
    ListItem* current = list->head;
    while (current) {
        ListItem* next = current->next;
        if (list->free_data) {
            list->free_data(current->data);
        } else {
            free(current->data);
        }
        free(current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
    free(list);
}

void* List_pop(List* list, int index) {
    ListItem* current = List_get_item(list, index);
    ListItem* prev = current->prev;
    ListItem* next = current->next;
    if (prev) {
        prev->next = next;
    } else {
        list->head = next;
    }
    if (next) {
        next->prev = prev;
    } else {
        list->tail = prev;
    }
    void* res = NULL;
    if (list->copy_data) {
        res = list->copy_data(current->data);
    } else {
        res = calloc(1, list->sizeOfData);
        memcpy(res, current->data, list->sizeOfData);
    }
    if (list->free_data) {
        list->free_data(current->data);
    } else {
        free(current->data);
    }
    free(current);
    list->len--;
    return res;
}

bool List_find(const List* list, const void* data) {
    assert(list);
    ListItem* item = list->head;
    if (list->compare_data) {
        while (item && list->compare_data(data, item->data) != 0) {
            item = item->next;
        }
    } else {
        while (item && memcmp(data, item->data, list->sizeOfData) != 0) {
            item = item->next;
        }
    }
    if (item != NULL) {
        return true;
    }
    return false;
}

bool List_compare(const void* list_void1, const void* list_void2) {
    const List* list1 = (const List*)list_void1;
    const List* list2 = (const List*)list_void2;
    assert(list1->len == list2->len);
    ListItem* item1 = list1->head;
    ListItem* item2 = list2->head;
    for (size_t i = 0; i < list1->len; i++) {
        if (list1->compare_data) {
            if (!list1->compare_data(item1->data, item2->data)) {
                return false;
            }
        } else {
            if (memcmp(item1->data, item2->data, list1->sizeOfData)) {
                return false;
            }
        }
        item1 = item1->next;
        item2 = item2->next;
    }
    return true;
}

void** List_items(const List* list) {
    assert(list);
    void** list_items = malloc(list->len * sizeof(void*));
    ListItem* item = list->head;
    for (size_t i = 0; i < list->len; i++) {
        list_items[i] = item->data;
        item = item->next;
    }
    return list_items;
}