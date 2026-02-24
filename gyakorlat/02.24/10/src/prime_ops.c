#include "prime_ops.h"
#include "kernel_loader.h"
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

bool is_prime_parallel(long number, bool use_range_mode) {
    if (number < 2) return false;
    if (number == 2) return true;

    int error_code;
    cl_platform_id platform_id;
    clGetPlatformIDs(1, &platform_id, NULL);
    cl_device_id device_id;
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, NULL);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, NULL);

    char* source = load_kernel_source("kernels/prime_kernels.cl", &error_code);
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, NULL);
    clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Initial value: 0 (assume prime until divisor is found)
    int host_found = 0;
    cl_mem bufFound = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int), &host_found, NULL);

    cl_kernel kernel;
    size_t global_size;

    if (!use_range_mode) {
        kernel = clCreateKernel(program, "check_single_divisor", NULL);
        clSetKernelArg(kernel, 0, sizeof(long), &number);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufFound);
        global_size = (size_t)sqrt(number);
    } else {
        int range_size = 1000;
        kernel = clCreateKernel(program, "check_range_divisors", NULL);
        clSetKernelArg(kernel, 0, sizeof(long), &number);
        clSetKernelArg(kernel, 1, sizeof(int), &range_size);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufFound);
        global_size = ((size_t)sqrt(number) / range_size) + 1;
    }

    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    clEnqueueReadBuffer(queue, bufFound, CL_TRUE, 0, sizeof(int), &host_found, 0, NULL, NULL);

    clReleaseMemObject(bufFound);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(source);

    return host_found == 0;
}