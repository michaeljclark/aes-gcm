/*
 *  test.cl
 */

__kernel void add(__global int * a, __global int *b, __global int* c)
{
    int i = get_global_id(0);
    c[i] = a[i] + b[i];
}

__kernel void write_float(__global float *dest, float src)
{
    dest[0] = src;
}
