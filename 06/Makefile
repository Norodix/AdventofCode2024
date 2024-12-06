part1: part1.o
	gcc $^ -o $@ -O3 -lm

part2: part2.o
	gcc $^ -o $@ -O3 -lm

part2_thread: part2_thread.o
	gcc $^ -o $@ -O0 -g -lm -lpthread

all: part1 part2
	gcc $^ -o part1 -O3 -lm

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test1: part1
	./part1 example

test2: part2
	./part2 example
