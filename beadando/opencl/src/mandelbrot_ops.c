#define CL_TARGET_OPENCL_VERSION 220
#include "mandelbrot_ops.h"
#include "kernel_loader.h"
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Host function to manage OpenCL execution.
 * Only reports build errors to keep the console output clean.
 */
void calculate_mandelbrot_opencl(int* image, int width, int height, int max_iter, int mode) {
    cl_int err;
    int error_code;

    // 1. Get Platform and GPU Device
    cl_platform_id platform_id;
    clGetPlatformIDs(1, &platform_id, NULL);
    cl_device_id device_id;
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

    // 2. Create Context and Command Queue
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, &err);

    // 3. Load and Build the Kernel Source
    char* source = load_kernel_source("kernels/mandelbrot.cl", &error_code);
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &err);
    
    // Attempt to build the program
    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    
    // Only fetch and print the log if the build failed
    if (err != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char* log = (char*)malloc(log_size);
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf("OpenCL Build ERROR Log:\n%s\n", log);
        free(log); 
        free(source);
        return; // Stop execution if kernel build fails
    }

    // 4. Create the specific Kernel and Memory Buffer
    const char* kernel_name = (mode == 1) ? "mandelbrot_vector" : "mandelbrot_point";
    cl_kernel kernel = clCreateKernel(program, kernel_name, &err);

    size_t buffer_size = width * height * sizeof(int);
    cl_mem bufImage = clCreateBuffer(context, CL_MEM_WRITE_ONLY, buffer_size, NULL, &err);

    // 5. Setup Kernel Arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufImage);
    clSetKernelArg(kernel, 1, sizeof(int), &width);
    clSetKernelArg(kernel, 2, sizeof(int), &height);
    clSetKernelArg(kernel, 3, sizeof(int), &max_iter);

    // 6. Define Work Dimensions
    size_t global_work_size[2];
    if (mode == 1) {
        global_work_size[0] = (size_t)(width / 4);
        global_work_size[1] = (size_t)height;
    } else {
        global_work_size[0] = (size_t)width;
        global_work_size[1] = (size_t)height;
    }

    // 7. Enqueue Kernel and Read Results
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);
    clEnqueueReadBuffer(queue, bufImage, CL_TRUE, 0, buffer_size, image, 0, NULL, NULL);

    // 8. Resource Cleanup
    clReleaseMemObject(bufImage);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(source);
}