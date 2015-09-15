UNAME_S := $(shell uname -s)

CFLAGS=-Wall -Werror -I out/gen -g

X86_BINARIES= \
	  out/forth_i686 \
	  out/forth_x86_64 \
	  out/forth_i686_opt \
	  out/forth_x86_64_opt \
	  out/forth_i686_dbg \
	  out/forth_x86_64_dbg
ARM_BINARIES= \
	  out/forth_arm \
	  out/forth_arm_opt \
	  out/forth_arm_dbg

ifeq ($(UNAME_S),Darwin)
LDFLAGS=$(CFLAGS)
RUNNABLE=$(X86_BINARIES) out/forth_fat
BINARIES=$(ARM_BINARIES) $(RUNNABLE)
else
RUNNABLE=$(X86_BINARIES) $(ARM_BINARIES)
BINARIES=$(RUNNABLE)
LDFLAGS=$(CFLAGS) -nostdlib --static
endif

ifeq ($(UNAME_S),Darwin)
SDKDIR=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
CC_arm=clang -arch armv7 -isysroot $(SDKDIR) -miphoneos-version-min=1.0
STRIP_arm=strip
else
CC_arm=arm-linux-gnueabihf-gcc
STRIP_arm=arm-linux-gnueabihf-strip
endif
CC_i686=$(CC) -m32
STRIP_i686=strip
CC_x86_64=$(CC) -m64
STRIP_x86_64=strip

LIBS=

all: tests

tests: $(BINARIES)
	for x in $(RUNNABLE); do bash test.sh $$x; done

out:
	mkdir -p out

out/gen:
	mkdir -p out/gen

HEADERS=forth.h forth_asm.h out/gen/forth_boot.h forth_util.h

out/gen/forth_boot.h: boot.fs | out/gen
	xxd -i $< > $@

out/forth_arm: out/forth_arm_opt
	$(STRIP_arm) $< -o $@

out/forth_i686: out/forth_i686_opt
	$(STRIP_i686) $< -o $@

out/forth_x86_64: out/forth_x86_64_opt
	$(STRIP_x86_64) $< -o $@

out/forth_%: out/forth_%_opt
	strip $< -o $@

out/forth_arm_dbg: forth.c $(HEADERS) | out
	$(CC_arm) -O0 $(LDFLAGS) $< $(LIBS) -o $@

out/forth_i686_dbg: forth.c $(HEADERS) | out
	$(CC_i686) -O0 $(LDFLAGS) $< $(LIBS) -o $@

out/forth_x86_64_dbg: forth.c $(HEADERS) | out
	$(CC_x86_64) -O0 $(LDFLAGS) $< $(LIBS) -o $@

out/forth_arm_opt: forth.c $(HEADERS) | out
	$(CC_arm) -O2 $(LDFLAGS) $< $(LIBS) -o $@

out/forth_i686_opt: forth.c $(HEADERS) | out
	$(CC_i686) -O2 $(LDFLAGS) $< $(LIBS) -o $@

out/forth_x86_64_opt: forth.c $(HEADERS) | out
	$(CC_x86_64) -O2 $(LDFLAGS) $< $(LIBS) -o $@

ifeq ($(UNAME_S),Darwin)
out/forth_fat: forth.c $(HEADER) | out
	rm -f $@
	$(CC) -arch i386 -arch x86_64 -O2 $(LDFLAGS) $< -o $@
endif

clean:
	rm -rf out
