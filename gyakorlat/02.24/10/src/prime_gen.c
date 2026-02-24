#include "prime_gen.h"
#include "kernel_loader.h"
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include <stdlib.h>
#include <stdio.h>

void parallel_prime_generation(const int* input, int* results, int n) {
    int error_code;
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);
    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

    char* source = load_kernel_source("kernels/prime_gen.cl", &error_code);
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "generate_primes", NULL);

    cl_mem bufIn = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * n, (void*)input, NULL);
    cl_mem bufOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * n, NULL, NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufIn);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufOut);
    clSetKernelArg(kernel, 2, sizeof(int), &n);

    size_t global_size = n;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    clEnqueueReadBuffer(queue, bufOut, CL_TRUE, 0, sizeof(int) * n, results, 0, NULL, NULL);

    clReleaseMemObject(bufIn); clReleaseMemObject(bufOut);
    clReleaseKernel(kernel); clReleaseProgram(program);
    clReleaseCommandQueue(queue); clReleaseContext(context);
    free(source);
}