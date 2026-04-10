#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <omp.h>
#include "mandelbrot_ops.h"

#define MAX_ITER 1000
#define RUNS 5 

int main(int argc, char *argv[]) {
    int width = 1024;
    int height = 1024;

    if (argc >= 3) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }

    int *image = malloc(width * height * sizeof(int));
    if (image == NULL) {
        fprintf(stderr, "Failed to allocate host memory!\n");
        return 1;
    }

    printf("Starting %d runs for averaging (OpenCL)...\n", RUNS);
    double total_time_ms = 0;

    for (int i = 0; i < RUNS; i++) {
        double start_time = omp_get_wtime();
        calculate_mandelbrot_opencl(image, width, height, MAX_ITER);
        double end_time = omp_get_wtime();
        
        double current_run_ms = (end_time - start_time) * 1000.0;
        total_time_ms += current_run_ms;
        printf("  Run %d: %.2f ms\n", i + 1, current_run_ms);
    }

    double average_ms = total_time_ms / RUNS;
    // The number of threads in OpenCL context is the total number of pixels
    long total_threads = (long)width * height;

    printf("Average execution time: %.2f ms using OpenCL with %ld threads (%dx%d)\n", 
           average_ms, total_threads, width, height);

    // Save PPM file
    FILE *fp = fopen("mandelbrot_ocl.ppm", "w");
    if (fp) {
        fprintf(fp, "P3\n%d %d\n255\n", width, height);
        for (int i = 0; i < width * height; i++) {
            int color = image[i] % 256;
            fprintf(fp, "%d %d %d ", color, color, color);
        }
        fclose(fp);
    }

    _mkdir("results");
    FILE *fp_csv = fopen("results/mandelbrot_ocl_results.csv", "a");
    if (fp_csv != NULL) {
        fseek(fp_csv, 0, SEEK_END);
        // If file is empty, write header
        if (ftell(fp_csv) == 0) {
            fprintf(fp_csv, "Width,Height,Threads,Time(ms)\n");
        }
        // Log the average time and the total work items (threads)
        fprintf(fp_csv, "%d,%d,%ld,%.2f\n", width, height, total_threads, average_ms);
        fclose(fp_csv);
    }

    free(image);
    return 0;
}