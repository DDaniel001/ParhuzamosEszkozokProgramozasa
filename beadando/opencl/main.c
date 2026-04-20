#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <omp.h>
#include <math.h>
#include "mandelbrot_ops.h"

#define MAX_ITER 1000
#define RUNS 5 

int main(int argc, char *argv[]) {
    int width = 1024, height = 1024, mode = 0;

    if (argc >= 4) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        mode = atoi(argv[3]);
    }

    int *image = malloc(width * height * sizeof(int));
    if (!image) return 1;

    const char* label = (mode == 1) ? "OpenCL_Vectorized" : "OpenCL_PointBased";
    printf("Benchmarking MODE: %s (%dx%d)\n", label, width, height);
    
    double total_time_ms = 0;
    for (int i = 0; i < RUNS; i++) {
        double start = omp_get_wtime();
        calculate_mandelbrot_opencl(image, width, height, MAX_ITER, mode);
        double end = omp_get_wtime();
        
        double current = (end - start) * 1000.0;
        
        // Skip first run (warm-up) for average calculation to exclude JIT time
        if (i > 0) { 
            total_time_ms += current;
        }
        printf("  Run %d: %.2f ms %s\n", i + 1, current, (i == 0 ? "(warm-up)" : ""));
    }

    double avg_time = total_time_ms / (RUNS - 1);
    printf("Final Average: %.2f ms\n", avg_time);

    // Save output to Color PPM
    char filename[64];
    sprintf(filename, "mandelbrot_%s.ppm", (mode == 1) ? "vector" : "point");
    FILE *fp = fopen(filename, "w");
    if (fp) {
        fprintf(fp, "P3\n%d %d\n255\n", width, height);
        for (int i = 0; i < width * height; i++) {
            // Use absolute value for consistency with vectorized kernel output
            int iter = abs(image[i]);
            int r, g, b;

            // Black center for points within the set
            if (iter >= MAX_ITER) {
                r = g = b = 0;
            } else {
                // Procedural sine coloring for smooth gradients
                r = (int)(128 + 127 * sin(0.3 * iter + 0));
                g = (int)(128 + 127 * sin(0.3 * iter + 2));
                b = (int)(128 + 127 * sin(0.3 * iter + 4));
            }
            fprintf(fp, "%d %d %d ", r % 256, g % 256, b % 256);
        }
        fclose(fp);
    }

    // Logging to CSV
    _mkdir("results");
    FILE *fp_csv = fopen("results/mandelbrot_ocl_results.csv", "a");
    if (fp_csv) {
        fseek(fp_csv, 0, SEEK_END);
        if (ftell(fp_csv) == 0) {
            fprintf(fp_csv, "Width,Height,Threads,Time(ms),Label\n");
        }
        fprintf(fp_csv, "%d,%d,%ld,%.2f,%s\n", width, height, (long)width*height, avg_time, label);
        fclose(fp_csv);
    }

    free(image);
    return 0;
}