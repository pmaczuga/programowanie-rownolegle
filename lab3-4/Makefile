all: seq par

seq: seq.c
	gcc -std=c99 $^ -o $@ -lm

par: par.c
	mpicc -std=c99 $^ -o $@ -lm

clean:
	rm -f *.o seq
