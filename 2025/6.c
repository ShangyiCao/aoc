#include <ctype.h>
#include <list.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

long compute_result(long **array, char *operators, int rows_len,
                    int *len_per_row, int results_len, bool row_wise) {
  long *results = malloc(sizeof(long) * results_len);
  for (int i = 0; i < results_len; i++) {
    results[i] = (operators[i] == '+') ? 0 : 1;
  }

  if (row_wise) {
    for (int i = 0; i < rows_len; i++) {
      for (int j = 0; j < len_per_row[i]; j++) {
        if (operators[j] == '+') {
          results[j] += array[i][j];
        } else {
          results[j] *= array[i][j];
        }
      }
    }
  } else {
    for (int i = 0; i < rows_len; i++) {
      for (int j = 0; j < len_per_row[i]; j++) {
        if (operators[i] == '+') {
          results[i] += array[i][j];
        } else {
          results[i] *= array[i][j];
        }
      }
    }
  }

  long counter = 0;
  for (int i = 0; i < results_len; i++) {
    counter += results[i];
  }
  free(results);
  return counter;
}

int main(int argc, char **argv) {
  FILE *file = fopen(argv[1], "r");
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
  int rows_len = 0;
  int rows_capacity = 1024;
  long **rows = malloc(sizeof(long *) * rows_capacity);
  int *len_per_row = malloc(sizeof(int) * rows_capacity);
  int *capacity_per_row = malloc(sizeof(int) * rows_capacity);
  for (int i = 0; i < rows_capacity; i++) {
    len_per_row[i] = 0;
    capacity_per_row[i] = 1024;
  }

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
    long *row = malloc(sizeof(long) * capacity_per_row[rows_len]);
    if (rows_len == rows_capacity) {
      rows_capacity *= 2;
      rows = realloc(rows, rows_capacity);
      len_per_row = realloc(len_per_row, rows_capacity);
      capacity_per_row = realloc(capacity_per_row, rows_capacity);
    }
    rows[rows_len] = row;

    bool start_new_line = false;
    while (isdigit(*p)) {
      if (len_per_row[rows_len] == capacity_per_row[rows_len]) {
        capacity_per_row[rows_len] *= 2;
        row = realloc(row, capacity_per_row[rows_len]);
      }
      row[len_per_row[rows_len]++] = strtol(p, (char **)&p, 10);
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
        rows_len++;
        break;
      }
    }
  }
  int operators_len = 0;
  int operators_capacity = 1024;
  char *operators = malloc(operators_capacity);
  while (*p != 0) {
    if (*p == '+' || *p == '*') {
      if (operators_len == operators_capacity) {
        operators_capacity *= 2;
        operators = realloc(operators, operators_capacity);
      }
      operators[operators_len++] = *p;
    }
    p++;
  }

  printf("%ld\n", compute_result(rows, operators, rows_len, len_per_row,
                                 operators_len, true));

  long **columns = malloc(sizeof(long *) * operators_len);
  int *len_per_column = malloc(sizeof(int) * operators_len);
  int *capacity_per_column = malloc(sizeof(int) * operators_len);
  for (int i = 0; i < operators_len; i++) {
    capacity_per_column[i] = 10;
    long *column = malloc(sizeof(long) * capacity_per_column[i]);
    columns[i] = column;
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
      if (column_id == operators_len) {
        break;
      }
      i++;
    }
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
    long *column = columns[column_id];
    if (len_per_column[column_id] == capacity_per_column[column_id]) {
      capacity_per_column[column_id] *= 2;
      column = realloc(column, capacity_per_column[column_id]);
    }
    column[len_per_column[column_id]++] = num;
    i++;
  }

  printf("%ld\n", compute_result(columns, operators, operators_len,
                                 len_per_column, operators_len, false));

  free(buffer);
  for (int i = 0; i < rows_len; i++) {
    free(rows[i]);
  }
  free(rows);
  for (int i = 0; i < operators_len; i++) {
    free(columns[i]);
  }
  free(columns);
  free(operators);
  return 0;
}