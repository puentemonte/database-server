# $^ is source file
# $@ is target file

flags=-O2 -Wall -std=c2x

all: clean cache23

cache23: cache23.o
	cc ${flags} $^ -o $@ ${ldflags}


cache23.o: cache23.c
	cc ${flags} -c $^

clean:
	rm -f *.o cache23
