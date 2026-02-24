#include <stdio.h>
#include <stdlib.h>
#include "prime_gen.h"

int main() {
    int n = 100;
    int* input = (int*)malloc(n * sizeof(int));
    int* results = (int*)malloc(n * sizeof(int));

    // Fill with numbers from 1 to 100
    for (int i = 0; i < n; i++) {
        input[i] = i + 1;
    }

    printf("Generating primes up to %d using OpenCL...\n", n);
    parallel_prime_generation(input, results, n);

    printf("Primes found:\n");
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (results[i] != 0) {
            printf("%d ", results[i]);
            count++;
        }
    }
    
    printf("\n\nTotal primes found: %d\n", count);

    free(input);
    free(results);
    return 0;
}