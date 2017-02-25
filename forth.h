#ifndef _forth_h_
#define _forth_h_

typedef unsigned char byte;
#if defined(__i386__)
typedef long cell_t;
#elif defined(__x86_64__)
typedef long long cell_t;
#elif defined(__arm__)
typedef long cell_t;
#else
# error "unsupported arch"
#endif

#if defined(__i386__)
typedef cell_t *(*code_word_t)(cell_t *) __attribute__((fastcall));
#else
typedef cell_t *(*code_word_t)(cell_t *);
#endif

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
    DEFE(name, label, F_NORMAL, last), && label }; label:
// Define code word immediate string name.
#define DEFSI(name, label, last) \
    DEFE(name, label, F_IMMEDIATE, last), && label }; label:
// Define code word.
#define DEF(name, last) DEFS(#name, name, last)
// Define user variable.
#define DEFU(name, label, last, value) \
    DEFE(name, label, F_NORMAL, last), && douvar, CP(value) };
// Define constant.
#define DEFC(name, label, last, value) \
    DEFE(name, label, F_NORMAL, last), && docon, CP(value) };

// INLINE NORMAL WORD DEFINITION MACROS
#define W(name) CP(&name ## _entry[D_CODE])
#define L(value) W(_lit), CP(value)
#define DEFWX(name, label, flags, last) \
  DEFE(name, label, flags, last), && enter,
#define DEFW(name, last) DEFWX(#name, name, F_NORMAL, last)
#define ENDW W(exit) };

// BUILT-IN USER VARIABLES
#define U_HEAP 0
#define U_TIB 1
#define U_NTIB 2
#define U_TIN 3
#define U_DICT_HEAD 4
#define U_STATE 5

extern void forth(
    cell_t *ip, cell_t *dsp, cell_t *rsp, cell_t *user, cell_t *heap,
    void (*emit_func)(cell_t), void (*terminate_func)(cell_t));

#endif
