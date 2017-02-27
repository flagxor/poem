#ifndef _forth_h_
#define _forth_h_

#include <stdint.h>

typedef unsigned char byte;
typedef intptr_t cell_t;
typedef cell_t *(*code_word_t)(cell_t *);

// DICTIONARY LAYOUT
#define D_NEXT 0
#define D_NAME 1
#define D_NAME_SIZE 2
#define D_FLAGS 3
#define D_CODE 4
#define D_REST 5
#define D_CODE_TO_REST (D_REST - D_CODE)

// DICTIONARY FLAGS
#define F_NORMAL 0
#define BIT(i) (1 << (i))
#define F_IMMEDIATE BIT(0)
#define F_SMUDGE BIT(1)
#define F_TAIL BIT(2)

// INTERPRETER WORDS
#define NEXT { w = CP(*ip++); goto *CP(*w); }
#define DUP { *++dsp = tos; }
#define DROP { tos = *dsp--; }

// Force to (cell_t *).
#define CP(value) ((cell_t *) (value))
// Generic dictionary definition start.
#define DEFE(name, label, flags, last) \
  static cell_t *label ## _entry[] = \
  { CP(last ## _entry), CP(name), \
    CP(sizeof(name) - 1), CP(flags)
// Define code word with string name.
#define DEFS(name, label, last) \
    DEFE(name, label, F_NORMAL, last), (cell_t*) && label }; label:
// Define code word immediate string name.
#define DEFSI(name, label, last) \
    DEFE(name, label, F_IMMEDIATE, last), (cell_t*) && label }; label:
// Define code word.
#define DEF(name, last) DEFS(#name, name, last)
// Define user variable.
#define DEFU(name, label, last, value) \
    DEFE(name, label, F_NORMAL, last), (cell_t*) && douvar, CP(value) };
// Define constant.
#define DEFC(name, label, last, value) \
    DEFE(name, label, F_NORMAL, last), (cell_t*) && docon, CP(value) };

// INLINE NORMAL WORD DEFINITION MACROS
#define W(name) CP(&name ## _entry[D_CODE])
#define L(value) W(_lit), CP(value)
#define DEFWX(name, label, flags, last) \
  DEFE(name, label, flags, last), (cell_t*) && enter,
#define DEFW(name, last) DEFWX(#name, name, F_NORMAL, last)
#define ENDW W(exit) };

// BUILT-IN USER VARIABLES
#define U_HEAP 0
#define U_TIB 1
#define U_NTIB 2
#define U_TIN 3
#define U_DICT_HEAD 4
#define U_STATE 5

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


static void forth(
    const void *cmd, cell_t cmd_len,
    cell_t **ipp, cell_t **dspp, cell_t **rspp, cell_t *user, cell_t *heap,
    void (*emit_func)(cell_t), void (*terminate_func)(cell_t)) {
  cell_t *ip = *ipp;
  cell_t *dsp = *dspp;
  cell_t *rsp = *rspp;
  cell_t tos = *dsp--;
  cell_t *w, *x;

  goto start;

  enter: *++rsp = (cell_t) ip; ip = w + D_CODE_TO_REST; NEXT;
  docon: DUP; tos = w[D_CODE_TO_REST]; NEXT;
  douvar: DUP; tos = (cell_t) &user[w[D_CODE_TO_REST]]; NEXT;
  dovar: DUP; tos = (cell_t) (w + D_CODE_TO_REST); NEXT;
  docode: DUP; dsp = ((code_word_t) (w + D_CODE_TO_REST))(dsp); DROP; NEXT;

  DEFE("", tail, F_TAIL, tail) };

  DEFC("0", zero, tail, 0);
  DEFC("1", one, zero, 1);
  DEFC("-1", negone, one, -1);
  DEFC("2", two, negone, 2);
  DEFC("-2", negtwo, two, -2);

  DEFS("1+", oneadd, negtwo) { ++tos; NEXT; }
  DEFS("1-", onesub, oneadd) { --tos; NEXT; }
  DEFS("2+", twoadd, onesub) { tos += 2; NEXT; }
  DEFS("2-", twosub, twoadd) { tos -= 2; NEXT; }
  DEFS("4+", fouradd, twosub) { tos += 4; NEXT; }
  DEFS("4-", foursub, fouradd) { tos -= 4; NEXT; }
  DEFS("8+", eightadd, foursub) { tos += 8; NEXT; }
  DEFS("8-", eightsub, eightadd) { tos -= 8; NEXT; }

  DEFS("2*", twomul, eightsub) { tos <<= 1; NEXT; }
  DEFS("2/", twodiv, twomul) { tos >>= 1; NEXT; }
  DEFS("4*", fourmul, twodiv) { tos <<= 2; NEXT; }
  DEFS("4/", fourdiv, fourmul) { tos >>= 2; NEXT; }
  DEFS("8*", eightmul, fourdiv) { tos <<= 3; NEXT; }
  DEFS("8/", eightdiv, eightmul) { tos >>= 3; NEXT; }

  DEFC("cell", cellw, eightdiv, sizeof(cell_t));
  DEFS("cell+", celladd, cellw) { tos += sizeof(cell_t); NEXT; }
  DEFS("cell-", cellsub, celladd) { tos -= sizeof(cell_t); NEXT; }
  DEFS("cells", cells, cellsub) { tos *= sizeof(cell_t); NEXT; }

  DEFS("+", add, cells) { tos += *dsp--; NEXT; }
  DEFS("-", sub, add) { tos = (*dsp--) - tos; NEXT; }
  DEFS("*", mul, sub) { tos *= *dsp--; NEXT; }
  DEF(negate, mul) { tos = -tos; NEXT; }

  DEFS("and", andop, negate) { tos &= *dsp--; NEXT; }
  DEFS("or", orop, andop) { tos |= *dsp--; NEXT; }
  DEFS("xor", xorop, orop) { tos ^= *dsp--; NEXT }
  DEF(invert, xorop) { tos = ~tos; NEXT; }

  DEFS("<", less, invert) { tos = (*dsp--) < tos ? -1 : 0; NEXT; }
  DEFS("=", equal, less) { tos = (*dsp--) == tos ? -1 : 0; NEXT; }
  DEFS("<>", nequal, equal) { tos = (*dsp--) != tos ? -1 : 0; NEXT; }
  DEFS(">", greater, nequal) { tos = (*dsp--) > tos ? -1 : 0; NEXT; }
  DEFS("<=", lequal, greater) { tos = (*dsp--) <= tos ? -1 : 0; NEXT; }
  DEFS(">=", gequal, lequal) { tos = (*dsp--) >= tos ? -1 : 0; NEXT; }

  DEFS("0<", zless, gequal) { tos = tos < 0 ? -1 : 0; NEXT; }
  DEFS("0=", zequal, zless) { tos = tos == 0 ? -1 : 0; NEXT; }
  DEFS("0>", zgreater, zequal) { tos = tos > 0 ? -1 : 0; NEXT; }

  DEFS("@", load, zgreater) { tos = *(cell_t *) tos; NEXT; }
  DEFS("c@", cload, load) { tos = *(byte *) tos; NEXT; }
  DEFS("!", store, cload) { *(cell_t *) tos = *dsp--; DROP; NEXT; }
  DEFS("c!", cstore, store) { *(byte *) tos = *dsp--; DROP; NEXT; }
  DEFS("+!", add_store, cstore) { *(cell_t *) tos += *dsp--; DROP; NEXT; }

  DEFS(">r", push, add_store) { *++rsp = tos; DROP; NEXT; }
  DEFS("r>", pop, push) { DUP; tos = *rsp--; NEXT; }
  DEFS("r@", rload, pop) { DUP; tos = *rsp; NEXT; }
  DEF(rdrop, rload) { --rsp; NEXT; }

  DEFS("2dup", twodup, rdrop) { DUP; tos = dsp[-1]; DUP; tos = dsp[-1]; NEXT; }
  DEF(dup, twodup) { DUP; NEXT; }
  DEF(drop, dup) { DROP; NEXT; }
  DEF(over, drop) { DUP; tos = dsp[-1]; NEXT; }
  DEF(swap, over) { x = (cell_t *) *dsp; *dsp = tos; tos = (cell_t) x; NEXT; }
  DEF(pick, swap) { DUP; tos = dsp[-tos]; NEXT; }

  DEFS("rp@", rpload, pick) { DUP; tos = (cell_t) rsp; NEXT; }
  DEFS("sp@", spload, rpload) { DUP; tos = (cell_t) dsp; NEXT; }
  DEFS("rp!", rpstore, spload) { rsp = (cell_t *) tos; DROP; NEXT; }
  DEFS("sp!", spstore, rpstore) { dsp = (cell_t *) tos; DROP; NEXT; }

  DEFU("tib", tib, spstore, U_TIB);
  DEFU("#tib", ntib, tib, U_NTIB);
  DEFU(">in", tin, ntib, U_TIN);
  DEFU("state", state, tin, U_STATE);
  DEFU("_heap", _heap, state, U_HEAP);
  DEFU("_dict_head", _dict_head, _heap, U_DICT_HEAD);
  DEFC("_dovar", _dovar, _dict_head, && dovar);
  DEFC("_douvar", _douvar, _dovar, && douvar);
  DEFC("_docon", _docon, _douvar, && docon);
  DEFC("_docode", _docode, _docon, && docode);

  DEF(cmove, _docode) {
    cmove((const char *) dsp[-1], (char *) dsp[0], tos); DROP; DROP; NEXT };
  DEFS("cmove>", cmoved, cmove) {
    cmoved((const char *) dsp[-1], (char *) dsp[0], tos); DROP; DROP; NEXT };

  DEF(here, cmoved) { DUP; tos = user[U_HEAP]; NEXT; }
  DEFS("c,", ccomma, here) {
    *(char *) user[U_HEAP] = (char) tos; ++user[U_HEAP]; DROP; NEXT; }
  DEFS(",", comma, ccomma) {
    *CP(user[U_HEAP]) = tos; user[U_HEAP] += sizeof(cell_t); DROP; NEXT; }
  DEFS("s,", scomma, comma) {
    cmove((const char *) *dsp, (char *) user[U_HEAP], tos);
    user[U_HEAP] += tos; DROP; DROP; NEXT; }
  DEF(align, scomma) {
      user[U_HEAP] = (user[U_HEAP] + sizeof(cell_t) - 1) &
                     ~(sizeof(cell_t) - 1); NEXT; }

  DEF(_lit, align) { DUP; tos = *ip++; NEXT; }
  DEF(__lit, _lit) { DUP; tos = (cell_t) W(_lit); NEXT; }
  DEF(exit, __lit) { ip = (cell_t *) *rsp--; NEXT; }
  DEF(_semiexit, exit) { ip = (cell_t *) *rsp--; NEXT; }
  DEF(execute, _semiexit) { *++rsp = (cell_t) ip; w = CP(tos); DROP; goto *CP(*w); }

  DEF(branch, execute) { ip = CP(*ip + (cell_t) ip); NEXT; }
  DEFS("0branch", zbranch, branch) {
    if (!tos) { ip = CP(*ip + (cell_t) ip); } else { ++ip; } DROP; NEXT; }
  DEFS("0<>branch", nzbranch, zbranch) {
    if (tos) { ip = CP(*ip + (cell_t) ip); } else { ++ip; } DROP; NEXT; }

  DEF(emit, nzbranch) { emit_func(tos); DROP; NEXT; }
  DEF(terminate, emit) { terminate_func(tos); DROP; NEXT; }
  DEF(yield, terminate) { DUP; *dspp = dsp; *rspp = rsp; *ipp = ip; return; }

  DEF(compare, yield) {
    dsp -= 3; tos = compare((const char *) dsp[1], dsp[2],
                            (const char *) dsp[3], tos); NEXT; }

  DEF(_parse_word, compare) {
    DUP; DUP; tos = parse_word(
        (char *) user[U_TIB], user[U_NTIB], &user[U_TIN], &dsp[0]);
    NEXT; }

  DEFSI("[", lbracket, _parse_word) { user[U_STATE] = 0; NEXT; }
  DEFS("]", rbracket, lbracket) { user[U_STATE] = 1; NEXT; }

  DEFW(_record_word, rbracket)
     W(here), W(_parse_word), W(swap), W(over), W(scomma), W(align), ENDW
  DEFW(_new_word, _record_word)
     W(_record_word), W(here), W(push),
     W(_dict_head), W(load), W(comma), W(swap), W(comma), W(comma),
     W(comma), W(comma), W(pop), W(_dict_head), W(store),
     ENDW
  DEFWX(":", colon, F_NORMAL, _new_word)
     L(&& enter), L(F_SMUDGE), W(_new_word), W(rbracket), ENDW
  DEFWX(";", semicolon, F_IMMEDIATE, colon)
    W(_dict_head), W(load), L(D_FLAGS - D_NEXT), W(cells), W(add), W(dup),
    W(load), L(F_SMUDGE), W(xorop), W(swap), W(store),
    W(lbracket), L(W(_semiexit)), W(comma), ENDW

  DEF(_find_raw, semicolon) {
    --dsp; tos = find(CP(tos), (const char *) dsp[0], dsp[1], &dsp[0]); NEXT; }
  DEFW(find, _find_raw)   W(_dict_head), W(load), W(_find_raw), ENDW
  DEFW(_one_word, find)
      W(_parse_word), W(find), W(negone), W(equal),
      W(state), W(load), W(one), W(equal), W(andop),
      W(nzbranch), CP(3 * sizeof(cell_t)), W(execute), W(exit), W(comma), ENDW
  DEFW(quit, _one_word)   W(_one_word), W(branch), CP(-2 * sizeof(cell_t)), ENDW

  static cell_t *code[] = { W(quit) };
start:
  user[U_TIB] = (cell_t) cmd;
  user[U_NTIB] = cmd_len;
  user[U_TIN] = 0;
  if (!ip) {
    user[U_HEAP] = (cell_t) heap;
    user[U_DICT_HEAD] = (cell_t) quit_entry;
    user[U_STATE] = 0;
  }
  ip = (cell_t *) code;
  NEXT;
}

#endif
