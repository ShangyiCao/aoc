#include <dict.h>
#include <set.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int i;
  int j;
} Coordinate;

void count_splits(int i, int j, int nx, int ny, const char (*grid)[nx + 1],
                  int *counter, Set *visited) {
  Set_add(visited, &(Coordinate){i, j});
  if (grid[j][i] == '^') {
    (*counter)++;
    if (i - 1 >= 0 && !Set_find(visited, &(Coordinate){i - 1, j})) {
      count_splits(i - 1, j, nx, ny, grid, counter, visited);
    }
    if (i + 1 < nx && !Set_find(visited, &(Coordinate){i + 1, j})) {
      count_splits(i + 1, j, nx, ny, grid, counter, visited);
    }
    return;
  } else {
    if (j == ny - 1) {
      return;
    } else {
      if (!Set_find(visited, &(Coordinate){i, j + 1})) {
        count_splits(i, j + 1, nx, ny, grid, counter, visited);
      }
      return;
    }
  }
}

long count_timelines(int i, int j, int nx, int ny, const char (*grid)[nx + 1],
                     Dict *record) {
  if (Dict_find(record, &(Coordinate){i, j})) {
    return *(long *)Dict_get(record, &(Coordinate){i, j});
  }
  if (grid[j][i] == '^') {
    long sum = 0;
    if (i - 1 >= 0) {
      sum += count_timelines(i - 1, j, nx, ny, grid, record);
    }
    if (i + 1 < nx) {
      sum += count_timelines(i + 1, j, nx, ny, grid, record);
    }
    Dict_add(record, &(Coordinate){i, j}, &sum);
    return sum;
  } else {
    if (j == ny - 1) {
      return 1;
    }
    long result = count_timelines(i, j + 1, nx, ny, grid, record);
    Dict_add(record, &(Coordinate){i, j}, &result);
    return result;
  }
}

int main(int argc, char **argv) {
  FILE *file = fopen(argv[1], "r");
  int capacity = 128;
  char *buffer = malloc(capacity);
  size_t length = 0;
  int ch;
  int nx = 0;
  int start = 0;
  while ((ch = fgetc(file)) != EOF) {
    if (length + 1 >= capacity) {
      capacity *= 2;
      buffer = realloc(buffer, capacity);
    }
    if (nx == 0 && ch == '\n') {
      nx = length;
    }
    if (ch == 'S') {
      start = length;
    }
    buffer[length++] = (char)ch;
  }
  buffer[length] = '\0';
  fclose(file);

  int ny = length / (nx + 1);
  char (*grid)[nx + 1] = (void *)buffer;
  int is = start % (nx + 1);
  int js = (start) / (nx + 1);

  int counter1 = 0;
  Set *visited = Set_create(nx * ny * 2, sizeof(Coordinate));
  count_splits(is, js, nx, ny, grid, &counter1, visited);
  printf("%d\n", counter1);

  Dict *record =
      Dict_create(nx * ny * 2, sizeof(Coordinate), sizeof(long), 0, 0);
  long counter2 = count_timelines(is, js, nx, ny, grid, record);
  printf("%ld\n", counter2);

  free(buffer);
  Set_free(visited);
  Dict_free(record);
  return 0;
}