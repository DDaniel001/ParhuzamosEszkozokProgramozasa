#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "vector_ops.h"

int main() {
    // 1. Settings
    int n = 10; // Number of elements in the vectors
    
    // Allocate memory on Host (CPU)
    float *a = (float*)malloc(n * sizeof(float));
    float *b = (float*)malloc(n * sizeof(float));
    float *res_ocl = (float*)malloc(n * sizeof(float));

    // 2. Initialize random seed
    srand((unsigned int)time(NULL)); 

    // 3. Fill vectors with random floats between 0.0 and 100.0
    for(int i = 0; i < n; i++) {
        a[i] = ((float)rand() / (float)RAND_MAX) * 100.0f;
        b[i] = ((float)rand() / (float)RAND_MAX) * 100.0f;
    }

    printf("--- Random Vector Addition (n=%d) ---\n", n);
    if (n <= 10) {
        printf("Input data:\n");
        for(int i = 0; i < n; i++) {
            printf("[%d]. element: %6.2f + %6.2f\n", i, a[i], b[i]);
        }
    }

    // 4. Call the wrapper function (Hides OpenCL implementation)
    parallel_vector_add(a, b, res_ocl, n);

    // 5. Print results
    if (n <= 10) {
        printf("\nOpenCL results:\n");
        for(int i = 0; i < n; i++) {
            printf("[%d]. sum: %6.2f\n", i, res_ocl[i]);
        }
    }

    // 6. Sequential verification on CPU
    int errors = 0;
    for(int i = 0; i < n; i++) {
        float expected = a[i] + b[i];
        // Compare the results with a small tolerance (epsilon)
        if (fabs(res_ocl[i] - expected) > 0.001f) {
            errors++;
        }
    }

    // 7. Final evaluation
    printf("\n----------------------------------------\n");
    if (errors == 0) {
        printf("SUCCESS! OpenCL and CPU results match.\n");
    } else {
        printf("FAILURE! %d elements differ from expected values.\n", errors);
    }
    printf("----------------------------------------\n");

    // Clean up
    free(a); free(b); free(res_ocl);
    return 0;
}