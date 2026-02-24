__kernel void moving_average(__global const float* input, __global float* output, int n, int radius) {
    int gid = get_global_id(0);

    if (gid < n) {
        float sum = 0.0f;
        int count = 0;

        // Iterate through the neighborhood defined by the radius
        for (int i = -radius; i <= radius; i++) {
            int index = gid + i;

            // Boundary check: only add elements that are within the array
            if (index >= 0 && index < n) {
                sum += input[index];
                count++;
            }
        }

        // Calculate average for the current position
        output[gid] = sum / count;
    }
}