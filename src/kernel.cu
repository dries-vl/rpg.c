#include <cuda_runtime.h>

// cuda kernel function
__global__ void addKernel(int *a, int *b, int *c, int size) {
    int index = threadIdx.x + blockIdx.x * blockDim.x;
    if (index < size) {
        c[index] = a[index] + b[index];
    }
}

// Export the function to be called from TCC
extern "C" __declspec(dllexport) void cuda_add(int *a, int *b, int *c) {
    int size = 10;
    int *d_a, *d_b, *d_c; // device copies of a, b, c

    // Allocate memory on the GPU
    cudaMalloc((void**)&d_a, size * sizeof(int));
    cudaMalloc((void**)&d_b, size * sizeof(int));
    cudaMalloc((void**)&d_c, size * sizeof(int));

    // Copy inputs to device
    cudaMemcpy(d_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, size * sizeof(int), cudaMemcpyHostToDevice);

    // Launch kernel on GPU
    addKernel<<<1, size>>>(d_a, d_b, d_c, size);

    // Copy result back to host
    cudaMemcpy(c, d_c, size * sizeof(int), cudaMemcpyDeviceToHost);

    // Clean up
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);

    // need to call this to avoid lingering cuda overhead even after freeing the dll itself
    cudaDeviceReset(); // reset the gpu and free all related resources -> 8mb => 2mb again -> this frees all the overhead
}
