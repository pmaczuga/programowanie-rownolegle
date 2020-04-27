#include "bucket.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    Bucket bucket;
    bucket_init(&bucket);
    bucket_insert(&bucket, 877);
    bucket_insert(&bucket, 880);
    bucket_insert(&bucket, 889);
    bucket_print(&bucket);

    bucket_print(&bucket);

    bucket_sort(&bucket);
    bucket_print(&bucket);
    
    printf("Popped %d\n", bucket_pop(&bucket));
    bucket_print(&bucket);

    Bucket array[5];
    for (int i = 0; i < 5; i++)
        bucket_init(&array[i]);

    bucket_insert(&array[0], 1);
    bucket_print(&array[0]);

    bucket_merge(&array[0], &array[1]);
    bucket_merge(&array[2], &array[0]);
    bucket_merge(&array[3], &array[4]);
    bucket_sort(&array[4]);

    for (int i = 0; i < 5; i++)
        bucket_print(&array[i]);

    printf("Popped %d\n", bucket_pop(&array[0])); 

    return 0;
}
