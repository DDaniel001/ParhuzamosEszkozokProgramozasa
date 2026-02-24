__kernel void vector_add(__global const float* a, __global const float* b, __global float* result, int n) {
    // Get the index of the current work item
    int i = get_global_id(0);
    
    // Boundary check to prevent overflow
    if (i < n) {
        result[i] = a[i] + b[i];
    }
}