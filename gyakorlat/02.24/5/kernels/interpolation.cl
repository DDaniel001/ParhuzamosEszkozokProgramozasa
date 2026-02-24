__kernel void interpolate_missing(__global int* data, int n) {
    int i = get_global_id(0);

    // Skip the first and last element to ensure neighbors exist
    // and check if the current element is missing (marked as -1)
    if (i > 0 && i < n - 1) {
        if (data[i] < 0) {
            data[i] = (data[i - 1] + data[i + 1]) / 2;
        }
    }
}