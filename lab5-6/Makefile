CC = gcc

all: bucket_sort random_vector example check_random_distr alg1 alg2

bucket_sort:
	$(CC) -Wall bucket_sort_3.c bucket.h bucket.c -o bucket_sort_3 -fopenmp -std=c99

alg1:
	$(CC) -Wall bucket_sort_1.c -o bucket_sort_1 -fopenmp -std=c99

alg2:
	$(CC) -Wall bucket_sort_2.c -o bucket_sort_2 -fopenmp -std=c99

check_random_distr:
	$(CC) -Wall check_random_distr.c bucket.h bucket.c -o check_random_distr -fopenmp -std=c99

random_vector: 
	$(CC) -Wall random_vector.c -o random_vector -fopenmp -std=c99

example:
	$(CC) test.c bucket.h bucket.c -o test -std=c99

clear:
	rm bucket_sort_3 check_random_distr random_vector test
