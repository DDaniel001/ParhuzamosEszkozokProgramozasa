#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "average_ops.h"

int main() {
    int n = 10;
    int radius = 1; // Window size will be 2*1 + 1 = 3
    float *input = (float*)malloc(n * sizeof(float));
    float *output = (float*)malloc(n * sizeof(float));

    // Initialize with some data
    for (int i = 0; i < n; i++) {
        input[i] = (float)((i + 1) * 10); // 10, 20, 30, 40...
    }
    // Add some "noise" to index 4
    input[4] = 100.0f;

    printf("Input data:\n");
    for (int i = 0; i < n; i++) printf("%.1f ", input[i]);
    printf("\n\n");

    // Perform moving average
    parallel_moving_average(input, output, n, radius);

    printf("Moving average (radius=%d):\n", radius);
    for (int i = 0; i < n; i++) printf("%.1f ", output[i]);
    printf("\n\n");

    // Sequential verification for the middle element (index 4)
    // Neighbors: index 3, 4, 5 -> values 40, 100, 60. Average: (40+100+60)/3 = 66.66
    float expected = (input[3] + input[4] + input[5]) / 3.0f;
    printf("Verification at index 4:\n");
    printf("Expected: %.2f, Got: %.2f\n", expected, output[4]);

    if (fabs(output[4] - expected) < 0.01f) {
        printf("SUCCESS! The moving average calculation is correct.\n");
    } else {
        printf("FAILURE! Verification failed.\n");
    }

    free(input);
    free(output);
    return 0;
}