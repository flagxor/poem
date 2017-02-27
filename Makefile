UNAME_S := $(shell uname -s)

CFLAGS=-Wall -Werror -I out/gen -g
LDFLAGS=$(CFLAGS)
LIBS=

all: tests out/poem/poem.ino

tests: out/forth
	bash test.sh out/forth

out:
	mkdir -p out

out/gen:
	mkdir -p out/gen

out/poem:
	mkdir -p out/poem

HEADERS=forth.h out/gen/forth_boot.h

out/gen/forth_boot.h: boot.fs | out/gen
	xxd -i $< > $@

out/forth: main.c $(HEADERS) | out
	$(CC) -g -O2 $(LDFLAGS) main.c $(LIBS) -o $@

out/poem/poem.ino: forth.h out/gen/forth_boot.h arduino.c | out/poem
	cat $^ > $@

clean:
	rm -rf out
