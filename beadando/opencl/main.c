#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <omp.h>
#include "mandelbrot_ops.h"

#define MAX_ITER 1000

int main(int argc, char *argv[]) {
    // Default image dimensions
    int width = 1024;
    int height = 1024;

    // Parse command line arguments if provided
    if (argc >= 3) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }

    // Allocate host memory for the image
    int *image = malloc(width * height * sizeof(int));
    if (image == NULL) {
        fprintf(stderr, "Failed to allocate host memory!\n");
        return 1;
    }

    // Start execution timer
    double start_time = omp_get_wtime();

    // Call OpenCL wrapper function
    calculate_mandelbrot_opencl(image, width, height, MAX_ITER);

    // Calculate elapsed time
    double end_time = omp_get_wtime();
    double elapsed_ms = (end_time - start_time) * 1000.0;

    printf("Execution time: %.2f ms using OpenCL (%dx%d)\n", elapsed_ms, width, height);

    // Save the result as a PPM image file
    FILE *fp = fopen("mandelbrot_ocl.ppm", "w");
    fprintf(fp, "P3\n%d %d\n255\n", width, height);
    for (int i = 0; i < width * height; i++) {
        int color = image[i] % 256;
        fprintf(fp, "%d %d %d ", color, color, color);
    }
    fclose(fp);

    // Create results directory and log performance data to CSV
    _mkdir("results");
    FILE *fp_csv = fopen("results/mandelbrot_ocl_results.csv", "a");
    if (fp_csv != NULL) {
        fseek(fp_csv, 0, SEEK_END);
        if (ftell(fp_csv) == 0) {
            fprintf(fp_csv, "Width,Height,Platform,Time(ms)\n");
        }
        fprintf(fp_csv, "%d,%d,OpenCL,%.2f\n", width, height, elapsed_ms);
        fclose(fp_csv);
    }

    // Clean up host memory
    free(image);
    return 0;
}