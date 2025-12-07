#include <assert.h>
#include <list.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  long a;
  long b;
} Range;

void merge_ranges(Range *range1, Range *range2) {
  Range range;
  if (range1->a <= range2->a) {
    assert(range1->b >= range2->a);
    range.a = range1->a;
    if (range1->b <= range2->b) {
      range.b = range2->b;
    } else {
      range.b = range1->b;
    }
  } else if (range1->a <= range2->b) {
    range.a = range2->a;
    if (range1->b <= range2->b) {
      range.b = range2->b;
    } else {
      range.b = range1->b;
    }
  } else {
    assert(false);
  }
  range1->a = range.a;
  range1->b = range.b;
  return;
}

int main() {
  FILE *file = fopen("1.txt", "r");
  List *ranges = List_create(sizeof(Range), 0, 0, 0);
  List *ingredients = List_create(sizeof(long), 0, 0, 0);
  long a;
  long b;
  while (fscanf(file, "%ld-%ld\n", &a, &b) == 2) {
    List_append(ranges, &(Range){a, b});
  }
  long c;
  while (fscanf(file, "%ld\n", &c) == 1) {
    List_append(ingredients, &c);
  }

  long counter1 = 0;
  for (int i = 0; i < ingredients->len; i++) {
    long ingredient = *(long *)List_get(ingredients, i);
    for (int j = 0; j < ranges->len; j++) {
      Range *range = List_get(ranges, j);
      if (ingredient >= range->a && ingredient <= range->b) {
        counter1++;
        break;
      }
    }
  }
  printf("%ld\n", counter1);

  bool stop = false;
  while (!stop) {
    List *removed = List_create(sizeof(bool), 0, 0, 0);
    for (int i = 0; i < ranges->len; i++) {
      List_append(removed, &(bool){false});
    }
    List *merged_ranges = List_create(sizeof(Range), 0, 0, 0);
    for (int i = 0; i < ranges->len; i++) {
      if (*(bool *)List_get(removed, i) == true) {
        continue;
      }
      Range *range1 = List_get(ranges, i);
      for (int j = i + 1; j < ranges->len; j++) {
        if (*(bool *)List_get(removed, j) == true) {
          continue;
        }
        Range *range2 = List_get(ranges, j);
        if (range1->b < range2->a || range1->a > range2->b) {
        } else {
          merge_ranges(range1, range2);
          bool *flag = List_get(removed, j);
          *flag = true;
        }
      }
      List_append(merged_ranges, range1);
    }
    if (ranges->len == merged_ranges->len) {
      stop = true;
    }
    List_free(ranges);
    List_free(removed);
    ranges = merged_ranges;
  }

  long counter2 = 0;
  for (int i = 0; i < ranges->len; i++) {
    Range *range = List_get(ranges, i);
    counter2 += range->b - range->a + 1;
  }
  printf("%ld\n", counter2);

  List_free(ranges);
  List_free(ingredients);
  fclose(file);
  return 0;
}