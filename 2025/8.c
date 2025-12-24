#include <dict.h>
#include <heap.h>
#include <list.h>
#include <set.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  long x;
  long y;
  long z;
} Coordinate;

typedef struct {
  Coordinate a;
  Coordinate b;
} Pair;

long distance(Pair pair) {
  Coordinate p1 = pair.a;
  Coordinate p2 = pair.b;
  return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) +
         (p1.z - p2.z) * (p1.z - p2.z);
}

int compare_distance(const void *p1, const void *p2) {
  Pair pair1 = *(const Pair *)p1;
  Pair pair2 = *(const Pair *)p2;
  long d1 = distance(pair1);
  long d2 = distance(pair2);
  if (d1 > d2) {
    return 1;
  }
  if (d1 < d2) {
    return -1;
  }
  return 0;
}

int compare_descend(const void *d1, const void *d2) {
  long a = *(const long *)d1;
  long b = *(const long *)d2;
  if (a > b) {
    return -1;
  }
  if (a < b) {
    return 1;
  }
  return 0;
}

void add_to_graph(Dict *graph, Coordinate a, Coordinate b) {
  if (Dict_find(graph, &a)) {
    List *neighbours = Dict_get(graph, &a);
    List_append(neighbours, &b);
  } else {
    List *neighbours = List_create(sizeof(Coordinate), 0, 0, 0);
    List_append(neighbours, &b);
    Dict_add(graph, &a, neighbours);
  }
}

long dfs(Coordinate *p, Dict *graph, Set *visited) {
  Set_add(visited, p);
  long count = 1;
  List *neighbours = Dict_get(graph, p);
  for (int i = 0; i < neighbours->len; i++) {
    Coordinate *neighbour = List_get(neighbours, i);
    if (!Set_find(visited, neighbour)) {
      count += dfs(neighbour, graph, visited);
    }
  }
  return count;
}

void find_circuits(Dict *graph, long *circuit_sizes) {
  Coordinate **coordinates = Dict_keys(graph);
  Set *visited = Set_create(100000, sizeof(Coordinate));
  int circuit_sizes_len = 0;
  for (int i = 0; i < graph->len; i++) {
    if (!Set_find(visited, coordinates[i])) {
      circuit_sizes[circuit_sizes_len++] = dfs(coordinates[i], graph, visited);
    }
  }
  free(coordinates);
  Set_free(visited);
}

int main(int argc, char **argv) {
  FILE *file = fopen(argv[1], "r");
  int coordinates_capacity = 1024;
  int coordinates_len = 0;
  Coordinate *coordinates = malloc(sizeof(Coordinate) * coordinates_capacity);

  long a, b, c;
  while (fscanf(file, "%ld,%ld,%ld\n", &a, &b, &c) == 3) {
    if (coordinates_len == coordinates_capacity) {
      coordinates_capacity *= 2;
      coordinates = realloc(coordinates, coordinates_capacity);
    }
    coordinates[coordinates_len++] = (Coordinate){a, b, c};
  }

  int n_pairs = coordinates_len * (coordinates_len - 1) / 2;
  Pair *pairs = malloc(sizeof(Pair) * n_pairs);
  int pairs_len = 0;
  for (int i = 0; i < coordinates_len; i++) {
    for (int j = i + 1; j < coordinates_len; j++) {
      Pair pair = {coordinates[i], coordinates[j]};
      pairs[pairs_len++] = pair;
    }
  }

  qsort(pairs, n_pairs, sizeof(Pair), compare_distance);

  Dict *graph = Dict_create(coordinates_len * 2, sizeof(Coordinate),
                            sizeof(List), List_copy, List_free);
  bool stop = true;
  int it = 0;
  for (int i = 0; i < n_pairs; i++) {
    Pair pair = pairs[i];
    add_to_graph(graph, pair.a, pair.b);
    add_to_graph(graph, pair.b, pair.a);
    if (graph->len == coordinates_len) {
      long *circuit_sizes = calloc(graph->len, sizeof(long));
      find_circuits(graph, circuit_sizes);
      qsort(circuit_sizes, graph->len, sizeof(long), compare_descend);
      if (circuit_sizes[0] == coordinates_len) {
        printf("%ld\n", pair.a.x * pair.b.x);
        stop = true;
      }
      free(circuit_sizes);
      if (stop) {
        break;
      }
    }
    it++;
    if (it == 1000) {
      long *circuit_sizes = calloc(graph->len, sizeof(long));
      find_circuits(graph, circuit_sizes);
      qsort(circuit_sizes, graph->len, sizeof(long), compare_descend);
      printf("%ld\n", circuit_sizes[0] * circuit_sizes[1] * circuit_sizes[2]);
      free(circuit_sizes);
    }
  }
  free(pairs);
  free(coordinates);
  Dict_free(graph);
  return 0;
}