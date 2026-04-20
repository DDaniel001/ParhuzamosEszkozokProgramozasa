// mandelbrot.exe 1024 1024 4

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <direct.h>
#include <math.h>

#define MAX_ITER 1000
#define RUNS 5 // Number of measurements for averaging

// Mathematic formula: z = z² + c, analyse that |z| <= 2, if not, it exits.
// If the iter reaches the Max_ITER it is part of the Mandelbrot set.
// z=a+bi => z² = (a² + b²) = a² - b² + 2abi
// z+1_real = z_real² - z_imag² + real(c)
// z+1_imag = 2 * z_real * z_imag + imag(c)
// The higher the iteration it reaches, the closer it is to the Mandelbrot set.
int mandelbrot(double real, double imag) {
    double z_real = 0.0, z_imag = 0.0;
    int iter = 0;
    while (z_real * z_real + z_imag * z_imag <= 4.0 && iter < MAX_ITER) {
        double temp = z_real * z_real - z_imag * z_imag + real;
        z_imag = 2.0 * z_real * z_imag + imag;
        z_real = temp;
        iter++;
    }
    return iter;
}

int main(int argc, char *argv[]) {
    int width = 1024;
    int height = 1024;
    int num_threads = 4;

    if (argc >= 3) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }
    if (argc >= 4) {
        num_threads = atoi(argv[3]);
    }

    omp_set_num_threads(num_threads);

    int *image = malloc(width * height * sizeof(int));
    if (image == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }

    printf("Starting %d runs for averaging (OpenMP with %d threads)...\n", RUNS, num_threads);
    
    // Zoom/Cropping coordinates
    double re_min = -1.6, re_max = 0.6;
    double im_min = -1.1, im_max = 1.1;

    double total_time_ms = 0;

    for (int r = 0; r < RUNS; r++) {
        double start_time = omp_get_wtime();

        #pragma omp parallel for schedule(dynamic)
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double real = re_min + x * (re_max - re_min) / width;
                double imag = im_min + y * (im_max - im_min) / height;
                image[y * width + x] = mandelbrot(real, imag);
            }
        }

        double end_time = omp_get_wtime();
        double current_run_ms = (end_time - start_time) * 1000.0;
        total_time_ms += current_run_ms;
        printf("  Run %d: %.2f ms\n", r + 1, current_run_ms);
    }

    double average_ms = total_time_ms / RUNS;
    printf("Average execution time: %.2f ms with %d threads (%dx%d)\n",
           average_ms, num_threads, width, height);

    // Save Color PPM file (P3 format)
    FILE *fp = fopen("mandelbrot_openmp.ppm", "w");
    if (fp) {
        fprintf(fp, "P3\n%d %d\n255\n", width, height);
        for (int i = 0; i < width * height; i++) {
            int iter = image[i];
            int red, green, blue;

            if (iter == MAX_ITER) {
                red = green = blue = 0;
            } else {
                red = (int)(128 + 127 * sin(0.3 * iter + 0));
                green = (int)(128 + 127 * sin(0.3 * iter + 2));
                blue = (int)(128 + 127 * sin(0.3 * iter + 4));
            }
            fprintf(fp, "%d %d %d ", red % 256, green % 256, blue % 256);
        }
        fclose(fp);
        printf("Image saved to mandelbrot_openmp.ppm\n");
    }

    _mkdir("results");
    FILE *fp_csv = fopen("results/mandelbrot_results.csv", "a");
    if (fp_csv != NULL) {
        fseek(fp_csv, 0, SEEK_END);
        if (ftell(fp_csv) == 0) {
            fprintf(fp_csv, "Width,Height,Threads,Time(ms),Label\n");
        }
        fprintf(fp_csv, "%d,%d,%d,%.2f,OpenMP\n", width, height, num_threads, average_ms);
        fclose(fp_csv);
    }

    free(image);
    return 0;
}