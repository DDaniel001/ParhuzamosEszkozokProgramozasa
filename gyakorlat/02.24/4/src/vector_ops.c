#include "vector_ops.h"
#include "kernel_loader.h"
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

void parallel_vector_add(const float* a, const float* b, float* result, int n) {
    cl_int err;
    int error_code;

    // Get Platform and Device
    cl_platform_id platform_id;
    clGetPlatformIDs(1, &platform_id, NULL);
    cl_device_id device_id;
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, NULL);

    // Create Context and Command Queue
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);
    // Note: clCreateCommandQueue is deprecated but used for compatibility with older examples
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, NULL);

    // Load kernel source using the provided loader
    char* source = load_kernel_source("kernels/vector_add.cl", &error_code);
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, NULL);
    clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "vector_add", NULL);

    // Create Device buffers
    cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n, (void*)a, NULL);
    cl_mem bufB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n, (void*)b, NULL);
    cl_mem bufRes = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * n, NULL, NULL);

    // Set Kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufRes);
    clSetKernelArg(kernel, 3, sizeof(int), &n);

    // Execute kernel
    size_t global_size = n;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);

    // Read result back to Host memory
    clEnqueueReadBuffer(queue, bufRes, CL_TRUE, 0, sizeof(float) * n, result, 0, NULL, NULL);

    // Release resources
    clReleaseMemObject(bufA); clReleaseMemObject(bufB); clReleaseMemObject(bufRes);
    clReleaseKernel(kernel); clReleaseProgram(program);
    clReleaseCommandQueue(queue); clReleaseContext(context);
    free(source);
}