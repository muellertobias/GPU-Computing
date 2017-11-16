#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel void vecAdd(__global double *a, __global double *b, __global double *c, __global double *temp, const unsigned int n)
{
    //Get our global thread ID
    int id = get_global_id(0);
	
    //Make sure we do not go out of bounds
	int maxSize = n*n;
	if (id < maxSize)
	{
		int posB = (id % n) -1;
		if (posB < 0)
		{
			posB = n;
		} else 
		{
			temp[id] = A[id] * b[posB];
		}
	}
}  