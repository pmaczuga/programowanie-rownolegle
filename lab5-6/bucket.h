#ifndef BUCKET_H_
#define BUCKET_H_

typedef struct Node
{
    struct Node *next;
    int data;
} Node;

typedef struct Bucket
{
    struct Node *first;
    int size; 
} Bucket;

void bucket_init(Bucket *bucket);
void bucket_insert(Bucket *bucket, int elem);
int bucket_pop(Bucket *bucket);
void bucket_sort(Bucket *bucket);
void bucket_merge(Bucket *first, Bucket *second);
int bucket_is_empty(Bucket *bucket);
void bucket_free(Bucket *bucket);
void bucket_print(Bucket *bucket);

#endif
