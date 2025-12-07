#include <list.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int i;
  int j;
} Point;

bool is_inside(int i, int j, int nx, int ny) {
  return 0 <= i && i < nx && 0 <= j && j < ny;
}

int main() {
  FILE *file = fopen("1.txt", "r");
  int ch;
  int capacity = 128;
  char *buffer = malloc(capacity);
  int length = 0;
  int nx = 0;
  while ((ch = fgetc(file)) != EOF) {
    if (length == capacity - 1) {
      capacity *= 2;
      buffer = realloc(buffer, capacity);
    }
    if ((char)ch == '\n' && nx == 0) {
      nx = length;
    }
    buffer[length++] = ch;
  }
  buffer[length] = '\0';
  int ny = length / (nx + 1);
  char (*lines)[nx + 1] = (void *)buffer;

  int global_count = 0;
  int it = 0;
  bool stop;
  while (!stop) {
    List *removed_indexes = List_create(sizeof(Point), 0, 0, 0);
    for (int j = 0; j < ny; j++) {
      for (int i = 0; i < nx; i++) {
        if (lines[j][i] == '@') {
          int count = 0;
          for (int ii = i - 1; ii <= i + 1; ii++) {
            for (int jj = j - 1; jj <= j + 1; jj++) {
              if (is_inside(ii, jj, nx, ny)) {
                if (lines[jj][ii] == '@') {
                  count++;
                }
              }
            }
          }
          if (count <= 4) {
            List_append(removed_indexes, &(Point){i, j});
            global_count++;
          }
        }
      }
    }
    if (it == 0) {
      printf("%d\n", global_count);
    }
    for (int i = 0; i < removed_indexes->len; i++) {
      Point *p = List_get(removed_indexes, i);
      lines[p->j][p->i] = '.';
    }
    if (removed_indexes->len == 0) {
      stop = true;
    }
    List_free(removed_indexes);
    it++;
  }
  printf("%d\n", global_count);
  return 0;
}