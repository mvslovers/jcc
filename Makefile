SRCS = $(wildcard src/*.c)
PROGS = $(patsubst src/%.c,%,$(SRCS))
CFLAGS = -w
PREFIX ?= /usr/local

all: $(PROGS)

%: src/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean: 
	rm -f $(PROGS)
.PHONY: clean

install: $(BINS) $(PROGS)
	install -m 755 -d $(PREFIX)/bin
	for f in $(BINS) $(PROGS); do install -m 755 $$f $(PREFIX)/bin; done
.PHONY: install
