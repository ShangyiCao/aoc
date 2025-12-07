#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  FILE *file = fopen("1.txt", "r");
  int capacity = 128;
  char *buffer = malloc(capacity);
  size_t length = 0;
  int ch;
  while ((ch = fgetc(file)) != EOF) {
    if (length + 1 >= capacity) {
      capacity *= 2;
      buffer = realloc(buffer, capacity);
    }
    buffer[length++] = (char)ch;
  }
  buffer[length] = '\0';
  fclose(file);

  char *p = buffer;
  long counter1 = 0;
  long counter2 = 0;
  while (*p) {
    if (isdigit(*p)) {
      long a = strtol(p, (char **)&p, 10);
      p++;
      long b = strtol(p, (char **)&p, 10);
      p++;
      for (long x = a; x <= b; x++) {
        char buffer[50];
        sprintf(buffer, "%ld", x);
        int len = strlen(buffer);
        // Part 1
        if (len % 2 == 0 && strncmp(buffer, buffer + len / 2, len / 2) == 0) {
          counter1 += x;
        }

        // Part 2
        for (int period = 1; period <= len / 2; period++) {
          if (len % period) {
            continue;
          }
          int repeat = len / period;
          bool valid = true;
          for (int i = 1; i < repeat; i++) {
            if (strncmp(buffer, buffer + i * period, period)) {
              valid = false;
              break;
            }
          }
          if (valid) {
            printf("Found %ld with repeat %d\n", x, repeat);
            counter2 += x;
            break;
          }
        }
      }
    } else {
      p++;
    }
  }
  printf("%ld\n", counter1);
  printf("%ld\n", counter2);
  free(buffer);
  return 0;
}