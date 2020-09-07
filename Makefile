SRCS = $(wildcard src/*.c)
PROGS = $(patsubst src/%.c,%,$(SRCS))
CFLAGS = -w

all: $(PROGS)

%: src/%.c
	$(CC) $(CFLAGS) -o $@ $<
clean: 
	rm -f $(PROGS)
