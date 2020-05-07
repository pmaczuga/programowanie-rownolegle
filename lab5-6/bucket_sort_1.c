				
#include <stdlib.h>
#include <float.h>
#include <stdbool.h> 
#include <stdio.h>
#include <omp.h>
#include <math.h>

typedef struct node {
	struct node* next;
	double value;
} bucket_node;

typedef struct bucket {
	bucket_node* bucket_head;
	int count;
} bucket;

void fillArrayRandomly(double *array, int n, int min, int max);
void bucketSort(double* array, int n, int buckets_count, int min, int max);
bool isSorted(double* array, int n);

double generate_random_time, bucket_time, sorting_time, bucket_merging_time;

int main(int argc, char** argv) {
	if(argc != 6){
		printf("Usage: %s <threads> <array_size> <num__of_buckets> <min_value> <max_value>\n", argv[0]);
        return -1;
    }
	int threads, n, buckets_count, min, max;
	double *array, time;
	threads = atoi(argv[1]); 
    n = atoi(argv[2]);
	buckets_count = atoi(argv[3]);
    min = atoi(argv[4]);
    max = atoi(argv[5]);
	array = (double*)malloc(n * sizeof(double));
	
	omp_set_num_threads(threads);
	time = omp_get_wtime();
	fillArrayRandomly(array, n, min, max);
	bucketSort(array, n, buckets_count, min, max);
	time = omp_get_wtime() - time;
	
	printf("Time of generating random array: %f\n", generate_random_time);
	printf("Time of dividing data into buckets: %f\n", bucket_time);
	printf("Time of sorting buckets: %f\n", sorting_time);
	printf("Time of writing buckets into array: %f\n", bucket_merging_time);
	printf("Full time: %f\n", time);
	
	printf("%s%s%s\n", "Array is ", isSorted(array, n) ? "" : "not ", "sorted properly\n");	
	
	free(array);
   
    return 0;
}

void fillArrayRandomly(double *array, int n, int min, int max) {
	int i;
	unsigned int seed;
	double time;
	
	time = omp_get_wtime();
	#pragma omp parallel default(none) shared(array, n, min, max) private(seed)
    {
        seed = omp_get_thread_num();
        #pragma omp for private(i) schedule(static)
        for(i = 0; i < n; i++) {
			array[i] = min + ((double) rand_r(&seed) / RAND_MAX) * (max - min);
		}
    }
	generate_random_time = omp_get_wtime() - time;
}

void push(bucket_node *head, double value) {
    bucket_node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = (bucket_node*)malloc(sizeof(bucket_node));
    current->next->next = NULL;
	current->next->value = value;
}

void sortBucketValues(bucket_node *head) {
	bucket_node *temp1, *temp2;
	double temp_value;
	
	for(temp1 = head->next; temp1 != NULL; temp1 = temp1->next) {
		for(temp2 = temp1->next; temp2 != NULL; temp2 = temp2->next) { 
			if(temp2->value < temp1->value)  {
				temp_value = temp1->value;
				temp1->value = temp2->value;
				temp2->value = temp_value;
			}
		}
	}
}

void bucketSort(double* array, int n, int buckets_count, int min, int max) {
	int i, j, thread_i, bucket_index, thread_num, num_threads, *buckets_start_indexes;
	double time, bucket_range, actual_value, range, left_bound, right_bound;
	bucket* buckets;
	bucket_range = (double) (max - min) / buckets_count;
	buckets = (bucket*)malloc(buckets_count * sizeof(bucket));
	buckets_start_indexes = (int*)malloc(buckets_count * sizeof(int));
	
	#pragma omp parallel default(none) shared(array, n, min, max, buckets, buckets_count, num_threads, buckets_start_indexes, \
		time, bucket_range, generate_random_time, bucket_time, sorting_time, bucket_merging_time) \
		private(i, j, thread_i, bucket_index, thread_num, actual_value, range, left_bound, right_bound)
	{		
		// buckets initiation
		#pragma omp master
		{		
			time = omp_get_wtime();
		}
		#pragma omp for schedule(static)
		for (i = 0; i < buckets_count; i++) {
			bucket_node* head = (bucket_node*)malloc(sizeof(bucket_node));
			head->next = NULL;
			buckets[i].bucket_head = head;
			buckets[i].count = 0;
		}

		// dividing data into buckets
		thread_num = omp_get_thread_num();
		num_threads = omp_get_num_threads();
		range = (double) (max - min) / num_threads;
		left_bound = min + range * thread_num;
		right_bound = left_bound + range;
		if (thread_num + 1 == num_threads) {
			right_bound += 1.0;
		}
		for (i = 0; i < n; i++) {
			thread_i = (i + n / num_threads * thread_num) % n;
			actual_value = array[thread_i];
			if (actual_value >= left_bound && actual_value < right_bound) {
				bucket_index = (actual_value - min) / bucket_range;
				push(buckets[bucket_index].bucket_head, actual_value);
				buckets[bucket_index].count++;
			}
		}
		#pragma omp barrier
		#pragma omp master
		{	
			bucket_time = omp_get_wtime() - time;
		
		// sorting buckets
			time = omp_get_wtime();
		}
		#pragma omp for schedule(static)
		for (i = 0; i < buckets_count; i++) {
			sortBucketValues(buckets[i].bucket_head);
			bucket_node *current = buckets[i].bucket_head;
			while (current->next != NULL) {
				current = current->next;
			}
		}
		#pragma omp master
		{	
			sorting_time = omp_get_wtime() - time;

		// writing buckets into array
			time = omp_get_wtime();	
			buckets_start_indexes[0] = 0;
			for (i = 1; i < buckets_count; i++) {
				buckets_start_indexes[i] = buckets_start_indexes[i-1] + buckets[i-1].count;
			}	
		}
		#pragma omp barrier
		#pragma omp for schedule(static)
		for (i = 0; i < buckets_count; i++) {
			bucket_node *current = buckets[i].bucket_head;
			int bucket_start_index = buckets_start_indexes[i] ;
			j = 0;		
			while(current->next != NULL) {
				array[bucket_start_index+ j] = current->next->value;
				bucket_node *next = current->next;
				free(current);
				current = next;
				j++;
			}
		}
		#pragma omp master
		{	
			bucket_merging_time = omp_get_wtime() - time;
		}
	}
	
	free(buckets_start_indexes);
	free(buckets);
}

bool isSorted(double* array, int n) {
	int i;
	for(i = 0; i < n - 1; i++) {
		if (array[i] > array[i+1]) {
			printf("Wrong values on indexes: [%d] %f [%d] %f\n", i, array[i], i+1, array[i+1]);
			return false;
		}
	}
	return true;
}
