UNAME_S := $(shell uname -s)

CFLAGS=-Wall -Werror -I out/gen -g
LDFLAGS=$(CFLAGS)
LIBS=
SRCS=forth.c main.c

all: tests

tests: out/forth
	bash test.sh out/forth

out:
	mkdir -p out

out/gen:
	mkdir -p out/gen

HEADERS=forth.h out/gen/forth_boot.h forth_util.h

out/gen/forth_boot.h: boot.fs | out/gen
	xxd -i $< > $@

out/forth: $(SRCS) $(HEADERS) | out
	$(CC) -g -O2 $(LDFLAGS) $(SRCS) $(LIBS) -o $@

clean:
	rm -rf out
