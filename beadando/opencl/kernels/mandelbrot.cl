// Enable double precision floating point support if available on the device
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

/**
 * Mandelbrot set kernel function
 * @param image: Output buffer for iteration counts
 * @param width: Image width
 * @param height: Image height
 * @param max_iter: Maximum number of iterations
 */
__kernel void mandelbrot(__global int* image, int width, int height, int max_iter) {
    // Get the global ID of the work-item in the 2D grid
    int x = get_global_id(0);
    int y = get_global_id(1);

    // Boundary check to ensure we don't write outside the image buffer
    if (x < width && y < height) {
        // Map pixel coordinates to the complex plane
        double real = (x - width / 2.0) * 4.0 / width;
        double imag = (y - height / 2.0) * 4.0 / height;

        double z_real = 0.0;
        double z_imag = 0.0;
        int iter = 0;

        // Iteration loop: z = z^2 + c
        // Check if |z| > 2 (z_real^2 + z_imag^2 > 4) or max iterations reached
        while (z_real * z_real + z_imag * z_imag <= 4.0 && iter < max_iter) {
            double temp = z_real * z_real - z_imag * z_imag + real;
            z_imag = 2.0 * z_real * z_imag + imag;
            z_real = temp;
            iter++;
        }

        // Store the result in the 1D output buffer
        image[y * width + x] = iter;
    }
}