#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  long x;
  long y;
} Coordinate;

typedef struct {
  Coordinate a;
  Coordinate b;
} Pair;

typedef enum { HORIZONTAL, VERTICAL } ORIENTATION;

long distance(Pair pair) {
  return (labs(pair.a.x - pair.b.x) + 1) * (labs(pair.a.y - pair.b.y) + 1);
}

int compare_distance(const void *p1, const void *p2) {
  Pair pair1 = *(const Pair *)p1;
  Pair pair2 = *(const Pair *)p2;
  long d1 = distance(pair1);
  long d2 = distance(pair2);
  if (d1 > d2) {
    return -1;
  }
  if (d1 < d2) {
    return 1;
  }
  return 0;
}

ORIENTATION orientation(const Pair *edge) {
  if (edge->a.y == edge->b.y) {
    return HORIZONTAL;
  }
  assert(edge->a.x == edge->b.x);
  return VERTICAL;
}

bool intersects(const Pair *edge1, const Pair *edge2) {
  if (orientation(edge1) == orientation(edge2)) {
    return false;
  }
  if (orientation(edge1) == HORIZONTAL) {
    return edge1->a.y > edge2->a.y && edge1->a.y < edge2->b.y &&
           edge2->a.x > edge1->a.x && edge2->a.x < edge1->b.x;
  } else {
    return intersects(edge2, edge1);
  }
}

bool on_edge(const Coordinate *point, const Pair *edge) {
  if (orientation(edge) == HORIZONTAL) {
    return point->y == edge->a.y && point->x >= edge->a.x &&
           point->x <= edge->b.x;
  } else {
    return point->x == edge->a.x && point->y >= edge->a.y &&
           point->y <= edge->b.y;
  }
}

bool ray_casting(const Coordinate *point, const Pair *domain_edges,
                 int domain_edges_len) {
  for (int i = 0; i < domain_edges_len; i++) {
    if (on_edge(point, &domain_edges[i])) {
      return true;
    }
  }
  int count = 0;
  for (int i = 0; i < domain_edges_len; i++) {
    if (orientation(&domain_edges[i]) != VERTICAL) {
      continue;
    }
    if ((point->x < domain_edges[i].a.x) && (point->y > domain_edges[i].a.y) &&
        (point->y <= domain_edges[i].b.y)) {
      count++;
    }
  }
  return count % 2;
}

Pair make_pair(Coordinate a, Coordinate b) {
  if (a.x == b.x) {
    return a.y < b.y ? (Pair){a, b} : (Pair){b, a};
  } else {
    return a.x < b.x ? (Pair){a, b} : (Pair){b, a};
  }
}

int main(int argc, char **argv) {
  FILE *file = fopen(argv[1], "r");
  int vertices_len = 0;
  int vertices_capacity = 1024;
  Coordinate *vertices = malloc(sizeof(Coordinate) * vertices_capacity);
  long a, b;
  while (fscanf(file, "%ld,%ld\n", &a, &b) == 2) {
    if (vertices_len == vertices_capacity) {
      vertices_capacity *= 2;
      vertices = realloc(vertices, sizeof(Coordinate) * vertices_capacity);
    }
    vertices[vertices_len++] = (Coordinate){a, b};
  }
  fclose(file);

  int pairs_capacity = vertices_len * (vertices_len - 1) / 2;
  int pairs_len = 0;
  Pair *pairs = malloc(sizeof(Pair) * pairs_capacity);
  for (int i = 0; i < vertices_len; i++) {
    for (int j = i + 1; j < vertices_len; j++) {
      pairs[pairs_len++] = make_pair(vertices[i], vertices[j]);
    }
  }
  qsort(pairs, pairs_len, sizeof(Pair), compare_distance);
  printf("%ld\n", distance(pairs[0]));

  Pair *domain_edges = malloc(sizeof(Pair) * vertices_len);
  int domain_edges_len = 0;
  for (int i = 0; i < vertices_len; i++) {
    int j = (i + 1) % vertices_len;
    domain_edges[domain_edges_len++] = make_pair(vertices[i], vertices[j]);
  }

  for (int i = 0; i < pairs_len; i++) {
    Coordinate a = pairs[i].a;
    Coordinate b = pairs[i].b;
    Coordinate c = {a.x, b.y};
    Coordinate d = {b.x, a.y};
    Coordinate rectangle_vertices[4] = {a, b, c, d};
    Pair rectangle_edges[4] = {make_pair(a, c), make_pair(a, d),
                               make_pair(b, c), make_pair(b, d)};
    bool intersection = false;
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < domain_edges_len; k++) {
        if (intersects(&(rectangle_edges[j]), &(domain_edges[k]))) {
          intersection = true;
          break;
        }
      }
      if (intersection) {
        break;
      }
    }
    if (!intersection) {
      bool corners_all_inside = true;
      for (int j = 0; j < 4; j++) {
        if (!ray_casting(&rectangle_vertices[j], domain_edges,
                         domain_edges_len)) {
          corners_all_inside = false;
          break;
        }
      }
      if (corners_all_inside) {
        printf("%ld\n", distance(pairs[i]));
        break;
      }
    }
  }

  free(domain_edges);
  free(vertices);
  free(pairs);
  return 0;
}