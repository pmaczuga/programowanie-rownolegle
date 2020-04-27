#include "bucket.h"

#include <stdlib.h>
#include <stdio.h>


void _node_insert_after(Node *node, Node *elem)
{
    elem->next = node->next;
    node->next = elem;
}

void _bucket_insert_node(Bucket *bucket, Node *node)
{
    node->next = bucket->first;
    bucket->first = node;
}   

void _bucket_insert_node_sorted(Bucket *bucket, Node *node)
{
    Node *prev = NULL;
    Node *current = bucket->first;
    while(current != NULL && node->data > current->data)
    {
        prev = current;
        current = current->next;
    }
    if (prev == NULL)
    {
        node->next = bucket->first;
        bucket->first = node;
    }
    else
    {
        _node_insert_after(prev, node);
    }
}

void _node_remove_after(Node *node)
{
    Node *to_remove = node->next;
    node->next = to_remove->next;
    free(to_remove);
}

Node *_bucket_pop_node(Bucket *bucket)
{
    Node *popped = bucket->first;
    if (popped != NULL)
        bucket->first = popped->next;
    return popped;
}



void bucket_init(Bucket *bucket)
{
    bucket->size = 0;
    bucket->first = NULL;
}

void bucket_insert(Bucket *bucket, int elem)
{
    Node *node = malloc(sizeof(Node));
    node->data = elem;
    _bucket_insert_node(bucket, node);
}

int bucket_pop(Bucket *bucket)
{
    Node *node = _bucket_pop_node(bucket);
    if (node == NULL)
        return -1;
    int elem = node->data;
    free(node);
    return elem;
}

void bucket_sort(Bucket *bucket)
{
    Bucket tmp;
    tmp.first = bucket->first;
    bucket->first = NULL;
    
    Node *elem = _bucket_pop_node(&tmp);
    while(elem != NULL)
    {
        _bucket_insert_node_sorted(bucket, elem);
        elem = _bucket_pop_node(&tmp);
    }
}

void bucket_merge(Bucket *to, Bucket *from)
{
    Node *elem = _bucket_pop_node(from);
    while(elem != NULL)
    {
        _bucket_insert_node(to, elem);
        elem = _bucket_pop_node(from);
    }
}

int bucket_is_empty(Bucket *bucket)
{
    if (bucket->first == NULL)
        return 1;
    return 0;
}

void bucket_free(Bucket *bucket)
{
    Node *node = bucket->first;
    Node *next;
    while(node != NULL)
    {
        next = node->next;
        free(node);
        node = next;
    }
}

void bucket_print(Bucket *bucket)
{
    printf("[");
    Node *node = bucket->first;
    while(node != NULL)
    {
        printf("%d", node->data);
        if (node->next != NULL)
            printf(", ");
        node = node->next;
    }
    printf("]\n");
}


