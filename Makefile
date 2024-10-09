flags=-Wall -O2 -std=c2x
ldflags=

all: clean tree cache23

tree: tree.o
	cc ${flags} $^ -o $@ ${ldflags}

tree.o: tree.c
	cc ${flags} -c $^

cache23: cache23.o
	cc ${flags} $^ -o $@ ${ldflags}

cache23.o: cache23.c
	cc ${flags} -c $^

clean:
	rm -f *.o tree
	rm -f *.o cache23
