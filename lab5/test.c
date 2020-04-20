#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    srand(time(NULL));    

    if(argc < 2)
    {
        printf("ERROR\n");
        return 1;
    }

    int *vector = malloc(atoi(argv[1]) * sizeof(int));

    for(int i=0; i<atoi(argv[1]); i++)
        vector[i] = rand()%1000;

    return 0;
}
