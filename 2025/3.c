#include <stdio.h>
#include <stdlib.h>

long power(int base, int exponent) {
  long result = 1;
  for (int i = 0; i < exponent; i++) {
    result *= base;
  }
  return result;
}

long find_combo(char *line, int nx, int digits) {
  int maxes[digits];
  for (int i = 0; i < digits; i++) {
    maxes[i] = -1;
  }
  int indexes[digits];
  for (int i = 0; i < digits; i++) {
    indexes[i] = -1;
  }
  for (int k = 0; k < digits; k++) {
    int i = 0;
    if (k > 0) {
      i = indexes[k - 1] + 1;
    }
    while (i < nx - (digits - k) + 1) {
      int x = line[i] - '0';
      if (x > maxes[k]) {
        maxes[k] = x;
        indexes[k] = i;
      }
      i++;
    }
  }
  long result = 0;
  for (int i = 0; i < digits; i++) {
    result += maxes[i] * power(10, digits - i - 1);
  }
  return result;
}

int main(int argc, char **argv) {
  FILE *file = fopen(argv[1], "r");
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
  const char (*lines)[nx + 1] = (void *)buffer;
  long counter1 = 0;
  long counter2 = 0;
  for (int j = 0; j < ny; j++) {
    counter1 += find_combo(lines[j], nx, 2);
    counter2 += find_combo(lines[j], nx, 12);
  }
  printf("%ld\n", counter1);
  printf("%ld\n", counter2);
  free(buffer);
  return 0;
}