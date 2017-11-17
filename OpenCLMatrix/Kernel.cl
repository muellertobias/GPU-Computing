#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel void OpenCLMatrix(__global double *a, __global double *b, __global double *c, __global double *temp, const unsigned int n)
{
    //Get our global thread ID
    int id = get_global_id(0);
	
    //Make sure we do not go out of bounds
	unsigned int maxSize = n*n;
	unsigned int posB = 0;
	if (id < maxSize)
	{
		posB = id % n;
		temp[id] = A[id] * b[posB];
	}
}  