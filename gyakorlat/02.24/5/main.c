#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "interpolation.h"

int main() {
    int n = 15;
    int* data = (int*)malloc(n * sizeof(int));
    srand((unsigned int)time(NULL));

    // 1. Generate input with missing values
    generate_test_data(data, n, 3);

    printf("Input data (-1 marks missing values):\n");
    for (int i = 0; i < n; i++) printf("%d ", data[i]);
    printf("\n\n");

    // 2. Perform OpenCL interpolation
    parallel_interpolation(data, n);

    printf("Result after interpolation:\n");
    for (int i = 0; i < n; i++) printf("%d ", data[i]);
    printf("\n\n");

    // 3. Verification (Sequential check)
    int errors = 0;
    for (int i = 1; i < n - 1; i++) {
        // Simple logic: in this test data, every element should be i*10
        if (data[i] != i * 10) {
            errors++;
        }
    }

    if (errors == 0) printf("SUCCESS! All missing values restored.\n");
    else printf("FAILURE! Found %d incorrect values.\n", errors);

    free(data);
    return 0;
}