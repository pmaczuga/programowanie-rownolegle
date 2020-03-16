#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 2) {
        printf("Arguments:\n\tNumber of random points\n");
        return 0;
    }
    long count = atol(argv[1]);
    long within_circle = 0;

    for (long i = 0; i < count; i++) {
        double x = (double)rand() / (double)RAND_MAX;
        double y = (double)rand() / (double)RAND_MAX;

        double distance = x*x + y*y;

        if (distance <= 1.0) {
            within_circle++;
        }
    }

    double pi = (double)within_circle / (double)count;
    pi = 4.0 * pi;
    printf("PI = %.15lf\n", pi);

    return 0;
}
