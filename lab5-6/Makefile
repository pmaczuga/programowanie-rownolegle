CC = gcc

all: bucket_sort random_vector example

bucket_sort:
	$(CC) -Wall bucket_sort_3.c bucket.h bucket.c -o bucket_sort_3 -fopenmp -std=c99

random_vector: 
	$(CC) -Wall random_vector.c -o random_vector -fopenmp -std=c99

example:
	$(CC) test.c bucket.h bucket.c -o test -std=c99
