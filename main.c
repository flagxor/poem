#include "forth.h"
#include "forth_boot.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void terminate(cell_t x) {
  exit(x);
}

static void emit(cell_t x) {
  fputc(x, stdout);
}

int main(void) {
  cell_t dstack[256], rstack[256], user[256], heap[1024 * 32];
  cell_t *dsp = dstack;
  cell_t *rsp = rstack;
  cell_t *ip = 0;
  char buffer[256];
  forth(
      boot_fs, boot_fs_len,
      &ip, &dsp, &rsp, user, heap, emit, terminate);
  for (;;) {
    if (!fgets(buffer, sizeof(buffer), stdin)) {
      break;
    }
    strcat(buffer, " yield\n");
    forth(
        buffer, strlen(buffer),
        &ip, &dsp, &rsp, user, heap, emit, terminate);
  }
  return 0;
}
