#define CL_TARGET_OPENCL_VERSION 220
#include "mandelbrot_ops.h"
#include "kernel_loader.h"
#include "error_handler.h"
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Executes the Mandelbrot calculation on the GPU using OpenCL
 */
void calculate_mandelbrot_opencl(int* image, int width, int height, int max_iter) {
    cl_int err;
    int error_code;

    // 1. Setup Platform and Device
    cl_platform_id platform_id;
    clGetPlatformIDs(1, &platform_id, NULL);
    cl_device_id device_id;
    // Prefer GPU device, fallback to any available device
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    if (err != CL_SUCCESS) {
        clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id, NULL);
    }

    // 2. Create Context and Command Queue
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, &err);

    // 3. Load, Create and Build the Program
    char* source = load_kernel_source("kernels/mandelbrot.cl", &error_code);
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &err);
    
    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    // Check for build errors and print the build log
    if (err != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char* log = (char*)malloc(log_size);
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf("OpenCL Build Log:\n%s\n", log);
        free(log); return;
    }

    // 4. Create Kernel and Memory Buffers
    cl_kernel kernel = clCreateKernel(program, "mandelbrot", &err);
    size_t buffer_size = width * height * sizeof(int);
    cl_mem bufImage = clCreateBuffer(context, CL_MEM_WRITE_ONLY, buffer_size, NULL, &err);

    // 5. Set Kernel Arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufImage);
    clSetKernelArg(kernel, 1, sizeof(int), &width);
    clSetKernelArg(kernel, 2, sizeof(int), &height);
    clSetKernelArg(kernel, 3, sizeof(int), &max_iter);

    // 6. Define Global Workspace and Execute Kernel
    size_t global_work_size[2] = { (size_t)width, (size_t)height };
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);

    // 7. Read results back to host memory (image pointer)
    clEnqueueReadBuffer(queue, bufImage, CL_TRUE, 0, buffer_size, image, 0, NULL, NULL);

    // 8. Cleanup resources
    clReleaseMemObject(bufImage);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(source);
}