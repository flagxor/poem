#ifndef _forth_util_h_
#define _forth_util_h_

#include "forth.h"

static cell_t compare(const char *a, cell_t a_n, const char *b, cell_t b_n) {
  while (a_n > 0 && b_n > 0) {
    if (*a < *b) return -1;
    if (*a > *b) return 1;
    ++a; ++b; --a_n; --b_n;
  }
  if (a_n < b_n) return -1;
  if (a_n > b_n) return 1;
  return 0;
}

static void cmove(const char *src, char *dst, cell_t n) {
  while (n) {
    *dst++ = *src++;
    --n;
  }
}

static void cmoved(const char *src, char *dst, cell_t n) {
  src += n;
  dst += n;
  while (n) {
    *--dst = *--src;
    --n;
  }
}

static cell_t find(cell_t *vocab, const char *s, cell_t n, cell_t *xt) {
  *xt = 0;
  for (; !(vocab[D_FLAGS] & F_TAIL); vocab = CP(vocab[D_NEXT])) {
    if (vocab[D_FLAGS] & F_SMUDGE) { continue; }
    if (compare((char *) vocab[D_NAME], vocab[D_NAME_SIZE], s, n) == 0) {
      *xt = (cell_t) &vocab[D_CODE];
      return (vocab[D_FLAGS] & F_IMMEDIATE) ? 1 : -1;
    }
  }
  return 0;
}

static int is_space(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

static cell_t parse_word(char *buffer, cell_t n, cell_t *at, cell_t *ret) {
  // Skip until not a space.
  for (;*at < n && is_space(buffer[*at]); ++*at);
  *ret = (cell_t) (buffer + *at);
  for (;*at < n && !is_space(buffer[*at]); ++*at);
  return &buffer[*at] - (char *) *ret;
}

#endif
