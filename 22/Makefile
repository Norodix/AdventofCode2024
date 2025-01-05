BUILDDIR = build

# Create build directory if it doesnt exist
$(shell mkdir -p $(BUILDDIR))

CFLAGS = -O3 -lm -g -march=native -mavx2

%part1: $(BUILDDIR)/part1.o
	gcc $^ -o $@ $(CFLAGS)

%part2: $(BUILDDIR)/part2.o
	gcc $^ -o $@ $(CFLAGS)

_EXE = part1 part2
EXE = $(addprefix $(BUILDDIR)/, $(_EXE))
all: $(EXE)

$(BUILDDIR)/%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -r $(BUILDDIR)

.PHONY: all test1 test2 clean

test1: $(BUILDDIR)/part1
	$(BUILDDIR)/part1 example

test2: $(BUILDDIR)/part2
	$(BUILDDIR)/part2 example2

test3: $(BUILDDIR)/part2
	$(BUILDDIR)/part2 example3
