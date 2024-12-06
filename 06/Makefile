BUILDDIR = build

# Create build directory if it doesnt exist
$(shell mkdir -p $(BUILDDIR))

CFLAGS = -O3 -lm
ODIR = obj

%part1: $(BUILDDIR)/part1.o
	gcc $^ -o $@ $(CFLAGS)

%part2: $(BUILDDIR)/part2.o
	gcc $^ -o $@ $(CFLAGS)

%part2_thread: $(BUILDDIR)/part2_thread.o
	gcc $^ -o $@ $(CFLAGS) -lpthread

%part2_threadpool: $(BUILDDIR)/part2_threadpool.o $(BUILDDIR)/thpool.o
	gcc $^ -o $@ -lpthread -IC-Thread-Pool

_EXE = part1 part2 part2_thread part2_threadpool
EXE = $(addprefix $(BUILDDIR)/, $(_EXE))
all: $(EXE)

$(BUILDDIR)/%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS) -IC-Thread-Pool

$(BUILDDIR)/thpool.o: C-Thread-Pool/thpool.c
	$(CC) -c -o $@ $^ $(CFLAGS) -IC-Thread-Pool

test1: part1
	$(BUILDDIR)/part1 example

test2: part2
	$(BUILDDIR)/part2 example

test2_threadpool: part2_threadpool
	$(BUILDDIR)/part2_threadpool example

clean:
	rm -r $(BUILDDIR)

.PHONY: all test1 test2 test2_threadpool clean
