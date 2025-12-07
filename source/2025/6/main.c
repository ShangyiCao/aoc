#include <ctype.h>
#include <list.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

long compute_result(List *array, List *operators, bool row_wise) {
  List *results = List_create(sizeof(long), 0, 0, 0);
  for (int i = 0; i < operators->len; i++) {
    long result;
    char *operator = List_get(operators, i);
    assert(*operator == '+' || *operator == '*');
    if (*operator == '+') {
      result = 0;
    } else {
      result = 1;
    }
    List_append(results, &result);
  }

  if (row_wise) {
    for (int i = 0; i < array->len; i++) {
      List *row = List_get(array, i);
      for (int j = 0; j < row->len; j++) {
        char *operator = List_get(operators, j);
        long *result = List_get(results, j);
        long *num = List_get(row, j);
        if (*operator == '+') {
          *result += *num;
        } else {
          *result *= *num;
        }
      }
    }
  } else {
    for (int i = 0; i < array->len; i++) {
      List *column = List_get(array, i);
      long *result = List_get(results, i);
      char *operator = List_get(operators, i);
      for (int j = 0; j < column->len; j++) {
        long *num = List_get(column, j);
        if (*operator == '+') {
          *result += *num;
        } else {
          *result *= *num;
        }
      }
    }
  }

  long counter = 0;
  for (int i = 0; i < results->len; i++) {
    long result = *(long *)List_get(results, i);
    counter += result;
  }
  List_free(results);
  return counter;
}

int main() {
  FILE *file = fopen("1.txt", "r");
  int capacity = 128;
  char *buffer = malloc(capacity);
  size_t length = 0;
  int ch;
  int nx = 0;
  while ((ch = fgetc(file)) != EOF) {
    if (length + 1 >= capacity) {
      capacity *= 2;
      buffer = realloc(buffer, capacity);
    }
    buffer[length++] = (char)ch;
    if (nx == 0 && ch == '\n') {
      nx = length - 1;
    }
  }
  buffer[length] = '\0';
  int ny = length / nx - 1;
  fclose(file);

  char *p = buffer;
  bool stop = false;
  List *rows = List_create(sizeof(List), List_compare, List_copy, List_free);
  while (!stop) {
    while (!isdigit(*p)) {
      p++;
      if (*p == '+' || *p == '*') {
        stop = true;
        break;
      }
    }
    if (stop) {
      break;
    }
    List_append(rows, List_create(sizeof(long), 0, 0, 0));
    List *row = List_get(rows, rows->len - 1);
    bool start_new_line = false;
    while (isdigit(*p)) {
      long a = strtol(p, (char **)&p, 10);
      List_append(row, &a);
      do {
        if (*p == '\n') {
          start_new_line = true;
          break;
        }
        p++;
      } while (!isdigit(*p));
      if (stop) {
        break;
      }
      if (start_new_line) {
        break;
      }
    }
  }
  List *operators = List_create(sizeof(char), 0, 0, 0);
  while (*p != 0) {
    if (*p == '+' || *p == '*') {
      List_append(operators, p);
    }
    p++;
  }

  printf("%ld\n", compute_result(rows, operators, true));

  List *columns = List_create(sizeof(List), List_compare, List_copy, List_free);
  for (int i = 0; i < operators->len; i++) {
    List *column = List_create(sizeof(long), 0, 0, 0);
    List_append(columns, column);
  }

  const char (*grid)[nx + 1] = (void *)buffer;
  int column_id = 0;
  int i = 0;
  while (true) {
    bool new_column = true;
    for (int j = 0; j < ny; j++) {
      if (isdigit(grid[j][i])) {
        new_column = false;
      }
    }
    if (new_column) {
      column_id++;
      if (column_id == columns->len) {
        break;
      }
      i++;
    }
    List *column = List_get(columns, column_id);
    long num = 0;
    int j = 0;
    while (j < ny) {
      char ch = grid[j][i];
      if (ch == ' ') {
        if (num == 0) {
          j++;
        } else {
          break;
        }
      }
      if (grid[j][i] != ' ') {
        num = num * 10 + grid[j][i] - '0';
      }
      j++;
    }
    List_append(column, &num);
    i++;
  }

  printf("%ld\n", compute_result(columns, operators, false));

  free(buffer);
  List_free(rows);
  List_free(columns);
  List_free(operators);
  return 0;
}