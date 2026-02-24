#ifndef AVERAGE_OPS_H
#define AVERAGE_OPS_H

// Interface to hide OpenCL moving average implementation
void parallel_moving_average(const float* input, float* output, int n, int radius);

#endif