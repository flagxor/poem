#include "forth.h"

#include <stdio.h>
#include <stdlib.h>

static void terminate(cell_t x) {
  exit(x);
}

static void emit(cell_t x) {
  fputc(x, stdout);
}

int main(void) {
  cell_t dstack[256], rstack[256], user[256], heap[1024 * 32];
  forth(0, &dstack[1], rstack, user, heap, emit, terminate);
  return 0;
}
