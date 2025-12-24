#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int check(char (*presents)[3][3], int presents_len, int m, int n,
          int *num_presents) {
  int sum1 = 0;
  for (int k = 0; k < presents_len; k++) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        sum1 += (presents[k][i][j] == '#' ? 1 : 0) * num_presents[k];
      }
    }
  }
  if (sum1 <= m * n) {
    return 1;
  }

  int sum2 = 0;
  for (int k = 0; k < presents_len; k++) {
    for (int j = 0; j < 3; j++) {
      sum2 += num_presents[k];
    }
  }
  if (sum2 > (m / 3) * (n / 3)) {
    return 0;
  }
  printf("too complicated :(\n");
  return 0;
}

int main(int argc, char **argv) {
  FILE *file = fopen(argv[1], "r");

  int presents_capacity = 10;
  int presents_len = 0;
  char (*presents)[3][3] = malloc(sizeof(*presents) * presents_capacity);
  char buffer[128];
  while (fgets(buffer, sizeof(buffer), file)) {
    if (buffer[0] == '\n' || buffer[strlen(buffer) - 2] == ':') {
      continue;
    }
    if (isdigit(buffer[0])) {
      break;
    }

    if (presents_len == presents_capacity) {
      presents_capacity *= 2;
      presents = realloc(presents, sizeof(*presents) * presents_capacity);
    }
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        presents[presents_len][i][j] = buffer[j];
      }
      fgets(buffer, sizeof(buffer), file);
    }
    presents_len++;
  }

  int count = 0;
  do {
    char *token = strtok(buffer, "x");
    long m = strtol(token, 0, 10);
    token = strtok(0, ": ");
    long n = strtol(token, 0, 10);
    int *num_presents = malloc(sizeof(int) * presents_len);
    for (int i = 0; i < presents_len; i++) {
      token = strtok(0, " ");
      num_presents[i] = strtol(token, 0, 10);
    }
    count += check(presents, presents_len, m, n, num_presents);
    free(num_presents);

  } while (fgets(buffer, sizeof(buffer), file));
  printf("%d\n", count);
  free(presents);
  fclose(file);
  return 0;
}