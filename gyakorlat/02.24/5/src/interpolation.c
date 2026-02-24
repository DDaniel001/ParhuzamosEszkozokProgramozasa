#include "interpolation.h"
#include "kernel_loader.h"
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

void generate_test_data(int* data, int n, int missing_count) {
    // Fill with positive integers (e.g., multiples of 10)
    for (int i = 0; i < n; i++) {
        data[i] = i * 10;
    }
    // Scatter missing elements (-1), excluding first and last
    for (int i = 0; i < missing_count; i++) {
        int index = 1 + rand() % (n - 2);
        data[index] = -1;
    }
}

void parallel_interpolation(int* data, int n) {
    int error_code;
    cl_platform_id platform_id;
    clGetPlatformIDs(1, &platform_id, NULL);
    cl_device_id device_id;
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, NULL);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, NULL);

    char* source = load_kernel_source("kernels/interpolation.cl", &error_code);
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, NULL);
    clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "interpolate_missing", NULL);

    // We use CL_MEM_READ_WRITE because we modify the buffer in place
    cl_mem bufData = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * n, (void*)data, NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufData);
    clSetKernelArg(kernel, 1, sizeof(int), &n);

    size_t global_size = n;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    clEnqueueReadBuffer(queue, bufData, CL_TRUE, 0, sizeof(int) * n, data, 0, NULL, NULL);

    clReleaseMemObject(bufData);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(source);
}