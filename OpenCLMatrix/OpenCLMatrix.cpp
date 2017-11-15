#define CL_USE_DEPRECATED_OPENCL_1_0_APIS
#include <CL/opencl.h>
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <ctime>
#include <string>
#include <sstream>

// Host input vectors
double *h_A;
double *h_b;
// temp Vector
double *temp;
// Host output vector
double *h_c;

// Kopiert von https://www.olcf.ornl.gov/tutorials/opencl-vector-addition/

char* readSourceFile(const char* filename);

using namespace std;

// OpenCL kernel. Each work item takes care of one element of c
//const char *kernelSource = "\n" \
//"#pragma OPENCL EXTENSION cl_khr_fp64 : enable                    \n" \
//"__kernel void vecAdd(  __global double *a,                       \n" \
//"                       __global double *b,                       \n" \
//"                       __global double *c,                       \n" \
//"                       const unsigned int n)                    \n" \
//"{                                                               \n" \
//"    //Get our global thread ID                                  \n" \
//"    int id = get_global_id(0);                                  \n" \
//"                                                                \n" \
//"    //Make sure we do not go out of bounds                      \n" \
//"    if (id < n)                                                 \n" \
//"        c[id] = a[id] + b[id];                                  \n" \
//"}                                                               \n" \
//"\n";

void initVector(double* vector, int n)
{
	srand(time(NULL));
#pragma omp parallel for
	for (int i = 0; i < n; i++)
	{
		vector[i] = 5-(i % 5);
	}
}

void initVectorWithNull(double* vector, int n)
{
#pragma omp parallel for
	for (int i = 0; i < n; i++)
	{
		vector[i] = 0;
	}
}

void initMatrix(double* matrix, int n, int m)
{
#pragma omp parallel for
	for (int i = 0; i < n*n; i++)
	{
		matrix[i] = i % 5;
	}
}

void initMatrixWithNull(double* matrix, int n, int m)
{
#pragma omp parallel for
	for (int i = 0; i < n; i++)
	{
		matrix[i] = 0;
	}
}

int main(int argc, char* argv[])
{
	const char* kernelSource = readSourceFile("Kernel.cl");

	// Length of vectors
	unsigned int n = 10;


	// Device input buffers
	cl_mem d_A;
	cl_mem d_b;
	// Device output buffer
	cl_mem d_c;
	// temp
	cl_mem d_temp;

	cl_platform_id cpPlatform;		  // OpenCL platform
	cl_device_id device_id;           // device ID
	cl_context context;				  // context
	cl_command_queue queue;			  // command queue
	cl_program program;				  // program
	cl_kernel kernel;				  // kernel


	size_t bytes = n * sizeof(float);  // Size, in bytes, of each vector

										// Allocate memory for each vector on host
	h_A = (double*)malloc(bytes*bytes);
	h_b = (double*)malloc(bytes);
	h_c = (double*)malloc(bytes);
	temp = (double*)malloc(bytes*bytes);

	// Initialize vectors on host
	cout << "init" << endl;
	initVector(h_b, n);
	initVectorWithNull(h_c, n);
	initMatrixWithNull(temp, n, n);
	initMatrix(h_A, n, n);

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n;j++)
		{
			cout << " " <<h_A[i];
		}
		cout << endl;
	}
	

	size_t globalSize, localSize;
	cl_int err;

	// Number of work items in each local work group
	localSize = 64;

	// Number of total work items - localSize must be devisor
	globalSize = ceil(n / (float)localSize)*localSize;
	globalSize *= globalSize;

	// Bind to platform
	err = clGetPlatformIDs(1, &cpPlatform, NULL);
	printf("GetPlatfrom: %d\n", err);

	// Get ID for the device
	err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
	printf("GetDeviceIDs: %d\n", err);

	// Create a context  
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	printf("CreateContext: %d\n", err);

	// Create a command queue 
	//queue = clCreateCommandQueue(context, device_id, NULL, &err);
	queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &err);
	printf("CreateCommandQueue: %d\n", err);

	// Create the compute program from the source buffer
	program = clCreateProgramWithSource(context, 1,
		(const char **)& kernelSource, NULL, &err);
	printf("CreateProgramWithSource: %d\n", err);

	// Build the program executable 
	clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

	// Create the compute kernel in the program we wish to run
	kernel = clCreateKernel(program, "vecAdd", &err);
	printf("CreateKernel: %d\n", err);

	// Create the input and output arrays in device memory for our calculation
	d_A = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
	d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
	d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);
	d_temp = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);

	// Write our data set into the input array in device memory
	err = clEnqueueWriteBuffer(queue, d_A, CL_TRUE, 0,
		bytes, h_A, 0, NULL, NULL);
	err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0,
		bytes, h_b, 0, NULL, NULL);
	printf("EnqueueWriteBuffers: %d\n", err);

	// Set the arguments to our compute kernel
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
	err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &d_temp);
	err |= clSetKernelArg(kernel, 4, sizeof(unsigned int), &n);
	printf("SetKernelArgs: %d\n", err);

	// Execute the kernel over the entire range of the data set  
	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize,
		0, NULL, NULL);

	// Wait for the command queue to get serviced before reading back results
	clFinish(queue);

	// Read the results from the device
	clEnqueueReadBuffer(queue, d_temp, CL_TRUE, 0,
		bytes, temp, 0, NULL, NULL);

	//Sum up vector c and print result divided by n, this should equal 1 within error
	double sum = 0;
	int row = n;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < row;j++)
		{
			h_c[i] += temp[i];
		}
		row += n;
		cout << h_c[i] << endl;
	}
	//printf("final result: %f\n", sum / n);

	// release OpenCL resources
	clReleaseMemObject(d_A);
	clReleaseMemObject(d_b);
	clReleaseMemObject(d_c);
	clReleaseMemObject(d_temp);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	//release host memory
	free(h_A);
	free(h_b);
	free(h_c);
	free(temp);

	printf("Press any key and then press enter...");
	int a = 0;
	std::cin >> a;
	return 0;
}

char* readSourceFile(const char* filename)
{
	FILE *fp;
	fopen_s(&fp, filename, "rb");
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* source = (char*)malloc(size + 1);
	fread(source, size, 1, fp);
	fclose(fp);
	source[size] = 0;

	return source;
}