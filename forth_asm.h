#ifndef _forth_asm_h_
#define _forth_asm_h_

#include "forth.h"

static cell system_call(cell tos, cell *dsp) {
#if __i386__
# ifdef __APPLE__
  asm volatile(
      "push %7; push %6; push %5; push %4; push %3; push %2;"
      "sub $4, %%esp; int $0x80; add $28, %%esp;"
      : "=a"(tos) : "0"(tos),
      "g"(dsp[0]), "g"(dsp[-1]), "g"(dsp[-2]),
      "g"(dsp[-3]), "g"(dsp[-4]), "g"(dsp[-5])
      : "ebx", "ecx", "edx", "memory");
# else
  asm volatile(
      "push %%esi; push %%edi; push %%ebp;"
      "push %5; push %6; push %7;"
      "pop %%edi; pop %%esi; pop %%ebp;"
      "int $0x80;"
      "pop %%ebp; pop %%edi; pop %%esi;"
      : "=a"(tos) : "0"(tos),
      "b"(dsp[0]), "c"(dsp[-1]), "d"(dsp[-2]),
      "g"(dsp[-3]), "g"(dsp[-4]), "g"(dsp[-5])
      : "memory");
# endif
#elif __x86_64__
  asm volatile (
      "mov %2, %%rdi; mov %3, %%rsi; mov %4, %%rdx;"
      "mov %5, %%r10; mov %6, %%r8; mov %7, %%r9;"
#ifdef __APPLE__
      "add $0x2000000, %%eax;"
#endif
      "syscall;"
      : "=a"(tos) : "0" (tos),
      "D"(dsp[0]), "S"(dsp[-1]), "d"(dsp[-2]),
      "g"(dsp[-3]), "g"(dsp[-4]), "g"(dsp[-5])
      : "rcx", "r8", "r9", "r10", "r11", "memory");
#elif __arm__
  register cell r0 asm("r0") = dsp[0];
  register cell r1 asm("r1") = dsp[-1];
  register cell r2 asm("r2") = dsp[-2];
  register cell r3 asm("r3") = dsp[-3];
  register cell r4 asm("r4") = dsp[-4];
  register cell r5 asm("r5") = dsp[-5];
  register cell r11 asm("r11") = tos;
  asm volatile (
      "push {r7};"
      "mov r7, %7;"
      "swi 0;"
      "pop {r7};"
      : "=r"(r0) :
      "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5), "r"(r11)
      : "memory");
#else
# error "unsupported arch"
#endif
  return tos;
}

#if !defined(__arm__)

static cell muldiv(cell tos, cell *dsp) {
#ifdef __i386__
  asm("imull %1, %%eax; idivl %3, %%eax;"
      : "=a"(tos) : "g"(tos), "a"(dsp[0]), "g"(dsp[-1]) : "edx");
#elif __x86_64__
  asm("imulq %1, %%rax; idivq %3, %%rax;"
      : "=a"(tos) : "g"(tos), "a"(dsp[0]), "g"(dsp[-1]) : "edx");
#else
# error "unsupported arch"
#endif
  return tos;
}

static cell muldivmod(cell tos, cell *dsp) {
#ifdef __i386__
  asm("imull %2, %%eax; idivl %4, %%eax;"
      : "=a"(tos), "=d"(dsp[-1]) : "g"(tos), "a"(dsp[0]), "d"(dsp[-1]));
#elif __x86_64__
  asm("imulq %2, %%rax; idivq %4, %%rax;"
      : "=a"(tos), "=d"(dsp[-1]) : "g"(tos), "a"(dsp[0]), "d"(dsp[-1]));
#else
# error "unsupported arch"
#endif
  return tos;
}

#endif

#endif
