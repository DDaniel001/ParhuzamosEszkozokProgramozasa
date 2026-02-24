#include "average_ops.h"
#include "kernel_loader.h"
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

void parallel_moving_average(const float* input, float* output, int n, int radius) {
    int error_code;
    cl_platform_id platform_id;
    clGetPlatformIDs(1, &platform_id, NULL);
    cl_device_id device_id;
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, NULL);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, NULL);

    char* source = load_kernel_source("kernels/moving_average.cl", &error_code);
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, NULL);
    clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "moving_average", NULL);

    cl_mem bufIn = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n, (void*)input, NULL);
    cl_mem bufOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * n, NULL, NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufIn);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufOut);
    clSetKernelArg(kernel, 2, sizeof(int), &n);
    clSetKernelArg(kernel, 3, sizeof(int), &radius);

    size_t global_size = n;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    clEnqueueReadBuffer(queue, bufOut, CL_TRUE, 0, sizeof(float) * n, output, 0, NULL, NULL);

    clReleaseMemObject(bufIn); clReleaseMemObject(bufOut);
    clReleaseKernel(kernel); clReleaseProgram(program);
    clReleaseCommandQueue(queue); clReleaseContext(context);
    free(source);
}