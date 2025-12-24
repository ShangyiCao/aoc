#include <dict.h>
#include <list.h>
#include <set.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long dfs(Dict *graph, char *start, char *end, Set *visited, Dict *record) {
  if (memcmp(start, end, sizeof(char) * 3) == 0) {
    return 1;
  }

  if (Dict_find(record, start)) {
    return *(long *)Dict_get(record, start);
  }

  List *neighbours = Dict_get(graph, start);
  long sum = 0;
  for (int i = 0; i < neighbours->len; i++) {
    char *neighbour = List_get(neighbours, i);
    if (Set_find(visited, neighbour)) {
      continue;
    }
    Set *new_visited = Set_copy(visited);
    Set_add(new_visited, neighbour);
    sum += dfs(graph, neighbour, end, new_visited, record);
    Set_free(new_visited);
  }
  Dict_add(record, start, &sum);
  return sum;
}

int main(int argc, char **argv) {
  FILE *file = fopen(argv[1], "r");
  char line[128];
  Dict *graph =
      Dict_create(1000, sizeof(char) * 3, sizeof(List), List_copy, List_free);

  while (fgets(line, sizeof(line), file)) {
    char *token = strtok(line, ": ");
    char *node = token;
    List *neighbours = List_create(sizeof(char) * 3, 0, 0, 0);
    while ((token = strtok(0, " \n"))) {
      List_append(neighbours, token);
    }
    Dict_add(graph, node, neighbours);
    List_free(neighbours);
  }
  List *neighbours = List_create(sizeof(char) * 3, 0, 0, 0);
  Dict_add(graph, "out", neighbours);
  fclose(file);

  Set *visited = Set_create(1000, sizeof(char) * 3);
  Dict *record = Dict_create(1000, sizeof(char) * 3, sizeof(long), 0, 0);
  long count1 = dfs(graph, "you", "out", visited, record);
  Set_free(visited);
  Dict_free(record);
  printf("%ld\n", count1);

  long count2 = 0;

  char start[3][3] = {"svr", "fft", "dac"};
  char end[3][3] = {"fft", "dac", "out"};
  long sum = 1;
  for (int i = 0; i < 3; i++) {
    Set *visited = Set_create(1000, sizeof(char) * 3);
    Dict *record = Dict_create(1000, sizeof(char) * 3, sizeof(long), 0, 0);
    sum *= dfs(graph, start[i], end[i], visited, record);
    Set_free(visited);
    Dict_free(record);
  }
  count2 += sum;

  char swap[3][3];
  memcpy(swap, start, sizeof(start));
  memcpy(start, end, sizeof(start));
  memcpy(end, swap, sizeof(start));

  sum = 1;
  for (int i = 0; i < 3; i++) {
    Set *visited = Set_create(1000, sizeof(char) * 3);
    Dict *record = Dict_create(1000, sizeof(char) * 3, sizeof(long), 0, 0);
    sum *= dfs(graph, start[i], end[i], visited, record);
    Set_free(visited);
    Dict_free(record);
  }
  count2 += sum;
  printf("%ld\n", count2);

  Dict_free(graph);

  return 0;
}