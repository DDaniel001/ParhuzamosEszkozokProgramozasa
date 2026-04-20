#pragma OPENCL EXTENSION cl_khr_fp64 : enable

/**
 * MODE 0: Standard Point-based kernel.
 * Maps pixels to a specific "crop" of the Mandelbrot set to remove empty space.
 */
__kernel void mandelbrot_point(__global int* image, int width, int height, int max_iter) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x < width && y < height) {
        // Define the "view window" (Cropping) matching the OpenMP version
        double re_min = -1.6, re_max = 0.6;
        double im_min = -1.1, im_max = 1.1;

        // Linear mapping from pixel (x,y) to complex plane (real, imag)
        double real = re_min + x * (re_max - re_min) / width;
        double imag = im_min + y * (im_max - im_min) / height;

        double z_real = 0.0;
        double z_imag = 0.0;
        int iter = 0;

        // Mandelbrot formula: z_{n+1} = z_n^2 + c
        while (z_real * z_real + z_imag * z_imag <= 4.0 && iter < max_iter) {
            double temp = z_real * z_real - z_imag * z_imag + real;
            z_imag = 2.0 * z_real * z_imag + imag;
            z_real = temp;
            iter++;
        }
        image[y * width + x] = iter;
    }
}

/**
 * MODE 1: Optimized Vectorized kernel (double4).
 * Processes a 1x4 horizontal block of pixels using SIMD instructions.
 */
__kernel void mandelbrot_vector(__global int* image, int width, int height, int max_iter) {
    int x_base = get_global_id(0) * 4;
    int y = get_global_id(1);

    if (x_base + 3 < width && y < height) {
        // Same cropping parameters as point-based
        double re_min = -1.6, re_max = 0.6;
        double im_min = -1.1, im_max = 1.1;

        double imag = im_min + y * (im_max - im_min) / height;
        
        // Create a vector of 4 consecutive X coordinates
        double4 x_vec = (double4)(x_base, x_base + 1, x_base + 2, x_base + 3);
        
        // Map the entire vector to the complex plane at once
        double4 c_re = re_min + x_vec * (re_max - re_min) / width;
        double4 c_im = (double4)(imag);

        double4 z_re = (double4)(0.0);
        double4 z_im = (double4)(0.0);
        int4 iters = (int4)(0);
        int4 active = (int4)(1);

        for (int i = 0; i < max_iter; i++) {
            double4 r2 = z_re * z_re;
            double4 i2 = z_im * z_im;
            
            // Comparison results in -1 for True, 0 for False in OpenCL vectors
            int4 mask = convert_int4(r2 + i2 <= 4.0);
            active &= mask;

            if (active.s0 == 0 && active.s1 == 0 && active.s2 == 0 && active.s3 == 0) break;

            double4 next_re = r2 - i2 + c_re;
            z_im = 2.0 * z_re * z_im + c_im;
            z_re = next_re;
            
            // Increment iterations for pixels that haven't escaped
            iters -= active; 
        }

        int offset = y * width + x_base;
        // Use abs() to ensure positive iteration counts for the host program
        image[offset]     = abs(iters.s0);
        image[offset + 1] = abs(iters.s1);
        image[offset + 2] = abs(iters.s2);
        image[offset + 3] = abs(iters.s3);
    }
}