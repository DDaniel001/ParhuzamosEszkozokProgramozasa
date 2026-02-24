#ifndef INTERPOLATION_H
#define INTERPOLATION_H

// Pure C interface to hide OpenCL
void parallel_interpolation(int* data, int n);

// Helper to generate test data with missing elements
void generate_test_data(int* data, int n, int missing_count);

#endif