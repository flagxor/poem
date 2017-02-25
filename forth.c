#include "forth.h"
#include "forth_asm.h"
#include "forth_boot.h"
#include "forth_util.h"

static void interp(cell_t *ip, cell_t *dsp, cell_t *rsp, cell_t *user, cell_t *heap) {
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

  DEFC("SYS_write", sys_write, cells, SYS_write);
  DEFC("SYS_exit", sys_exit, sys_write, SYS_exit);

  DEFS("+", add, sys_exit) { tos += *dsp--; NEXT; }
  DEFS("-", sub, add) { tos = (*dsp--) - tos; NEXT; }
  DEFS("*", mul, sub) { tos *= *dsp--; NEXT; }
  DEF(negate, mul) { tos = -tos; NEXT; }

  DEF(and, negate) { tos &= *dsp--; NEXT; }
  DEF(or, and) { tos |= *dsp--; NEXT; }
  DEF(xor, or) { tos ^= *dsp--; NEXT }
  DEF(invert, xor) { tos = ~tos; NEXT; }

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

  DEF(syscall, nzbranch) { tos = system_call(tos, dsp); dsp -= 6; NEXT; }

  DEF(compare, syscall) {
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
    W(load), L(F_SMUDGE), W(xor), W(swap), W(store),
    W(lbracket), L(W(_semiexit)), W(comma), ENDW

  DEF(_find_raw, semicolon) {
    --dsp; tos = find(CP(tos), (const char *) dsp[0], dsp[1], &dsp[0]); NEXT; }
  DEFW(find, _find_raw)   W(_dict_head), W(load), W(_find_raw), ENDW
  DEFW(_one_word, find)
      W(_parse_word), W(find), W(negone), W(equal),
      W(state), W(load), W(one), W(equal), W(and),
      W(nzbranch), CP(3 * sizeof(cell_t)), W(execute), W(exit), W(comma), ENDW
  DEFW(quit, _one_word)   W(_one_word), W(branch), CP(-2 * sizeof(cell_t)), ENDW

  static cell_t *code[] = { W(quit) };
start:
  if (!ip) {
    user[U_TIB] = (cell_t) boot_fs;
    user[U_NTIB] = (cell_t) boot_fs_len;
    user[U_TIN] = 0;
    user[U_HEAP] = (cell_t) heap;
    user[U_DICT_HEAD] = (cell_t) quit_entry;
    user[U_STATE] = 0;
    ip = (cell_t *) code;
  }
  NEXT;
}

int main(void) {
  cell_t dstack[16], rstack[16], user[16], heap[1024 * 100];
  interp(0, &dstack[1], rstack, user, heap);
  return 0;
}
