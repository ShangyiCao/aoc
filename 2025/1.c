#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  FILE *file = fopen(argv[1], "r");
  char c;
  int d;
  int x0 = 50;
  int count1 = 0;
  int count2 = 0;
  while (fscanf(file, "%c%d\n", &c, &d) == 2) {
    switch (c) {
    case 'R': {
      x0 += d;
      count2 += x0 / 100;
      break;
    }
    case 'L': {
      if (x0 - d <= 0) {
        if (x0 == 0) {
          count2 += (d - x0) / 100;
        } else {
          count2 += (d - x0) / 100 + 1;
        }
      }
      x0 -= d;
      break;
    }
    }
    x0 = (x0 % 100 + 100) % 100;
    count1 += (x0 == 0);
  }
  printf("%d\n", count1);
  printf("%d\n", count2);
  fclose(file);
  return 0;
}
