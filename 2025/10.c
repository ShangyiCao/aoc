#include <assert.h>
#include <ctype.h>
#include <glpk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long parse_goal1(const char *token) {
  char *p = token;
  p++;
  unsigned long x = 0;
  while (*p == '.' || *p == '#') {
    if (*p == '#') {
      x |= 1 << (p - token - 1);
    }
    p++;
  }
  return x;
}

void parse_goal2(const char *token, int *goal2) {
  char *p = token;
  p++;
  int i = 0;
  while (true) {
    if (isdigit(*p)) {
      goal2[i++] = strtol(p, &p, 10);
    }
    if (*p == '}') {
      break;
    }
    p++;
  }
}

void parse_button1(char *token, int goal_len, unsigned long *buttons,
                   int *buttons_len, int *buttons_capacity) {
  unsigned long x = 0;
  char *p = token;
  while (*p) {
    if (isdigit(*p)) {
      x |= 1 << strtol(p, &p, 10);
    }
    if (*p == '(' || *p == ',') {
      p++;
      continue;
    }
    if (*p == ')') {
      break;
    }
  }
  if (*buttons_len == *buttons_capacity) {
    *buttons_capacity *= 2;
    buttons = realloc(buttons, *buttons_capacity * sizeof(*buttons));
  }

  buttons[(*buttons_len)++] = x;
  return;
}

void parse_button2(char *token, int goal_len, int (*buttons)[goal_len],
                   int *buttons_len, int *buttons_capacity) {
  char *p = token;
  if (buttons_len == buttons_capacity) {
    *buttons_capacity *= 2;
    buttons = realloc(buttons, *buttons_capacity * sizeof(*buttons));
  }
  int *button = buttons[(*buttons_len)++];
  for (int i = 0; i < goal_len; i++) {
    button[i] = 0;
  }
  while (*p) {
    if (isdigit(*p)) {
      long id = strtol(p, &p, 10);
      assert(id < goal_len);
      button[id] = 1;
    }
    if (*p == '(' || *p == ',') {
      p++;
      continue;
    }
    if (*p == ')') {
      return;
    }
  }
}

int power(int base, int exponent) {
  int result = 1;
  for (int i = 0; i < exponent; i++) {
    result *= base;
  }
  return result;
}

long bfs(unsigned long goal, int goal_len, unsigned long *buttons,
         int buttons_len) {
  int max = power(2, goal_len);
  long *depth = calloc(max, sizeof(long));
  bool *visited = calloc(max, sizeof(bool));
  int queue_capacity = 100000;
  unsigned long *queue = calloc(queue_capacity, sizeof(unsigned long));
  int front = 0, back = 0;

  queue[back++] = 0;
  visited[0] = true;
  depth[0] = 0;
  while (front < back) {
    unsigned long u = queue[front++];
    if (u == goal) {
      long result = depth[u];
      free(queue);
      free(visited);
      free(depth);
      return result;
    }
    for (int i = 0; i < buttons_len; i++) {
      unsigned long v = u ^ buttons[i];
      if (!visited[v]) {
        visited[v] = true;
        depth[v] = depth[u] + 1;
        assert(back < queue_capacity);
        queue[back++] = v;
      }
    }
  }
}

int main(int argc, char **argv) {
  FILE *file = fopen(argv[1], "r");
  char line[1024];
  long count1 = 0;
  long count2 = 0;

  while (fgets(line, sizeof(line), file)) {

    line[strcspn(line, "\n")] = '\0';
    char *token = strtok(line, " ");
    unsigned long goal1 = parse_goal1(token);
    int goal_len = strlen(token) - 2;

    int buttons1_len = 0;
    int buttons1_capacity = 1024;
    unsigned long *buttons1 = malloc(sizeof(*buttons1) * buttons1_capacity);

    int buttons2_len = 0;
    int buttons2_capacity = 1024;
    int (*buttons2)[goal_len] = calloc(buttons2_capacity, sizeof(*buttons2));

    while ((token = strtok(0, " "))) {
      if (*token == '{') {
        break;
      }

      parse_button1(token, goal_len, buttons1, &buttons1_len,
                    &buttons1_capacity);
      parse_button2(token, goal_len, buttons2, &buttons2_len,
                    &buttons2_capacity);
    }

    count1 += bfs(goal1, goal_len, buttons1, buttons1_len);
    free(buttons1);

    int *goal2 = malloc(sizeof(int) * goal_len);
    parse_goal2(token, goal2);

    glp_prob *prob = glp_create_prob();
    glp_term_out(GLP_OFF);
    glp_set_obj_dir(prob, GLP_MIN);
    glp_add_cols(prob, buttons2_len);
    for (int i = 0; i < buttons2_len; i++) {
      glp_set_col_kind(prob, i + 1, GLP_IV);
      glp_set_col_bnds(prob, i + 1, GLP_LO, 0.0, 0.0);
      glp_set_obj_coef(prob, i + 1, 1.0);
    }

    glp_add_rows(prob, goal_len);
    for (int i = 0; i < goal_len; i++) {
      glp_set_row_bnds(prob, i + 1, GLP_FX, goal2[i], goal2[i]);
    }

    int *ia = malloc((goal_len * buttons2_len + 1) * sizeof(int));
    int *ja = malloc((goal_len * buttons2_len + 1) * sizeof(int));
    double *ar = malloc((goal_len * buttons2_len + 1) * sizeof(double));

    int k = 1;
    for (int j = 0; j < buttons2_len; j++) {
      for (int i = 0; i < goal_len; i++) {
        ia[k] = i + 1;
        ja[k] = j + 1;
        ar[k] = buttons2[j][i];
        k++;
      }
    }
    glp_load_matrix(prob, k - 1, ia, ja, ar);

    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.presolve = GLP_ON;
    glp_intopt(prob, &parm);
    for (int i = 1; i <= buttons2_len; i++) {
      count2 += glp_mip_col_val(prob, i);
    }
    glp_delete_prob(prob);
    free(goal2);
    free(buttons2);
    free(ia);
    free(ja);
    free(ar);
  }
  fclose(file);
  printf("%ld\n", count1);
  printf("%ld\n", count2);

  return 0;
}