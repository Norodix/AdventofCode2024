BUILDDIR = build

# Create build directory if it doesnt exist
$(shell mkdir -p $(BUILDDIR))

CFLAGS = -O0 -lm -g

%part1: $(BUILDDIR)/part1.o
	gcc $^ -o $@ $(CFLAGS)

%part2: part2.c hashmap.c/hashmap.o
	gcc $^ -o $@ $(CFLAGS)

_EXE = part1 part2
EXE = $(addprefix $(BUILDDIR)/, $(_EXE))
all: $(EXE)

hashmap.c/hashmap.o: hashmap.c/hashmap.c
	$(CC) -c -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -r $(BUILDDIR)

.PHONY: all test1 test2 clean

test1: $(BUILDDIR)/part1
	$(BUILDDIR)/part1 example

test2: $(BUILDDIR)/part2
	$(BUILDDIR)/part2 example
