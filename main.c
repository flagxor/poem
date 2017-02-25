#include "forth.h"

#include <stdio.h>
#include <stdlib.h>

static void emit(cell_t x) {
  fprintf(stderr, "BBAAAA\n");
  fputc(x, stdout);
}

static void terminate(void) {
  fprintf(stderr, "AAAA\n");
  exit(0);
}

int main(void) {
  cell_t dstack[256], rstack[256], user[256], heap[1024 * 32];
  interp(0, &dstack[1], rstack, user, heap, emit, terminate);
  return 0;
}
