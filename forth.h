#ifndef _forth_h_
#define _forth_h_

#if defined(__APPLE__)
# include <unistd.h>
#else
#if defined(__i386__) || defined(__arm__)
# define SYS_write 4
# define SYS_exit 1
#elif defined(__x86_64__)
# define SYS_write 1
# define SYS_exit 60
#else
# error "unsupported arch"
#endif
#endif

typedef unsigned char byte;
#if defined(__i386__)
typedef long cell;
#elif defined(__x86_64__)
typedef long long cell;
#elif defined(__arm__)
typedef long cell;
#else
# error "unsupported arch"
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

// Force to (cell *).
#define CP(value) ((cell *) (value))
// Generic dictionary definition start.
#define DEFE(name, label, flags, last) \
  static cell *label ## _entry[] = \
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

#endif
