#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/opencl.h>
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string>
#include <sstream>

#define type int // float, double

void testMVMWithoutThreading(type* A, type* b, type* result, const size_t n);
void testOpenCL(const char* kernelSource, type* h_A, type* h_b, type* h_c, const size_t n, const size_t bytes, size_t localSize);

void callGPU(cl_event& event, cl_int &err, const cl_command_queue &queue, cl_mem &d_A, const size_t &bytes, type * h_A, cl_mem &d_b, type * h_b, const cl_kernel &kernel, cl_mem &d_c, const size_t &n, size_t &globalSize, size_t &localSize, type * h_c);

char* readSourceFile(const char* filename);
int readBool(const char c);
void flush();
void printMatrix(type* matrix, const size_t n, const size_t m);
void initVector(type* vector, const size_t n);
void initVectorWithNull(type* vector, const size_t n);
void initMatrix(type* matrix, const unsigned int n, const size_t m);
void initMatrixWithNull(type* matrix, const unsigned int n, const size_t m);

void matrixVectorMultiplication(type* A, type* b, type* result, const size_t n);

using namespace std;

int main(int argc, char* argv[])
{
	const char* kernelSource = readSourceFile("Kernel.cl");

	// Host input vectors
	type *h_A;
	type *h_b;

	// Host output vector
	type *h_c;

	// Length of vectors
	size_t n = 256;

	// Number of work items in each local work group
	size_t localSize = 64;

	printf_s("n=");
	scanf_s("%d", &n);
	flush();
	printf_s("localSize=");
	scanf_s("%d", &localSize);
	flush();

	size_t bytes = n * sizeof(type);  // Size, in bytes, of each vector

	// Allocate memory for each vector on host
	h_A = (type*)malloc(bytes * bytes);
	h_b = (type*)malloc(bytes);
	h_c = (type*)malloc(bytes);

	// Initialize vectors on host
	printf_s("init...\n");
	initVector(h_b, n);
	initVectorWithNull(h_c, n);
	initMatrix(h_A, n, n);

	testMVMWithoutThreading(h_A, h_b, h_c, n);
	
	initVectorWithNull(h_c, n);

	testOpenCL(kernelSource, h_A, h_b, h_c, n, bytes, localSize);

	//release host memory
	free(h_A);
	free(h_b);
	free(h_c); 

	printf("Press any key and then press enter...");
	return getchar();
}

int readBool(const char c) 
{
	char input = 0;
	input = getchar();
	while (input != '\n' && getchar() != '\n');
	return input == c;
}

void flush() 
{
	while (getchar() != '\n');
}

void printMatrix(type* matrix, const unsigned int n, const size_t m)
{
	for (size_t n0 = 0; n0 < n; n0++)
	{
		for (size_t m0 = 0; m0 < m; m0++)
		{
			printf_s("%d ", matrix[n0 * n + m0]);
		}
		printf_s("\n");
	}
}

void testMVMWithoutThreading(type* A, type* b, type* result, const size_t n)
{
	clock_t start_normal = clock();

	matrixVectorMultiplication(A, b, result, n);

	clock_t stop_normal = clock();
	clock_t difference = stop_normal - start_normal;
	double t = (double)difference / CLOCKS_PER_SEC;

	printf_s(" \nFinished Normal: %f ms\n", t / 1000);
}

void testOpenCL(const char* kernelSource, type* h_A, type* h_b, type* h_c, const size_t n, const size_t bytes, size_t localSize)
{
	// Device input buffers
	cl_mem d_A;
	cl_mem d_b;
	// Device output buffer
	cl_mem d_c;

	cl_platform_id cpPlatform;		  // OpenCL platform
	cl_device_id device_id;           // device ID
	cl_context context;				  // context
	cl_command_queue queue;			  // command queue
	cl_program program;				  // program
	cl_kernel kernel;				  // kernel

	size_t globalSize;
	cl_int err;

	// Number of total work items - localSize must be devisor
	globalSize = ceil(n / (float)localSize) * localSize;

	// Bind to platform
	err = clGetPlatformIDs(1, &cpPlatform, NULL);
	printf("GetPlatfrom: %d\n", err);

	// Get ID for the device
	err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
	printf("GetDeviceIDs: %d\n", err);

	// Create a context  
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	printf("CreateContext: %d\n", err);

	cl_event event;
	// Create a command queue 
	//queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &err);
	queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
	printf("CreateCommandQueue: %d\n", err);

	// Create the compute program from the source buffer
	program = clCreateProgramWithSource(context, 1, (const char **)& kernelSource, NULL, &err);
	printf("CreateProgramWithSource: %d\n", err);

	// Build the program executable 
	clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

	// Create the compute kernel in the program we wish to run
	kernel = clCreateKernel(program, "OpenCLMatrix", &err);
	printf("CreateKernel: %d\n", err);

	// Create the input and output arrays in device memory for our calculation
	d_A = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes * bytes, NULL, NULL);
	d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
	d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);

	callGPU(event, err, queue, d_A, bytes, h_A, d_b, h_b, kernel, d_c, n, globalSize, localSize, h_c);

	cl_ulong time_start, time_end;
	double total_time;
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	total_time = time_end - time_start;
	printf("OpenCL Execution time: %f ms\n", total_time / 1000000.0);

	// release OpenCL resources
	clReleaseMemObject(d_A);
	clReleaseMemObject(d_b);
	clReleaseMemObject(d_c);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
}

void callGPU(cl_event& event, cl_int &err, const cl_command_queue &queue, cl_mem &d_A, const size_t &bytes, type * h_A, cl_mem &d_b, type * h_b, const cl_kernel &kernel, cl_mem &d_c, const size_t &n, size_t &globalSize, size_t &localSize, type * h_c)
{	
	// Write our data set into the input array in device memory
	err = clEnqueueWriteBuffer(queue, d_A, CL_TRUE, 0, bytes * bytes, h_A, 0, NULL, NULL);
	err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0, bytes, h_b, 0, NULL, NULL);
	//printf("EnqueueWriteBuffers: %d\n", err);

	// Set the arguments to our compute kernel
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
	err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &n);
	//printf("SetKernelArgs: %d\n", err);


	// Execute the kernel over the entire range of the data set  
	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize, 0, NULL, &event);

	clWaitForEvents(1, &event);
	// Wait for the command queue to get serviced before reading back results
	clFinish(queue);

	// Read the results from the device
	clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0, bytes, h_c, 0, NULL, NULL);
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

void initVector(type* vector, size_t n)
{
	srand(time(NULL));

#pragma omp parallel for
	for (long i = 0; i < n; i++)
	{
		vector[i] = 5 - (i % 5);
	}
}

void initVectorWithNull(type* vector, const size_t n)
{
#pragma omp parallel for
	for (long i = 0; i < n; i++)
	{
		vector[i] = 0;
	}
}

void initMatrix(type* matrix, const size_t n, const size_t m)
{
	size_t size = n * m;
	#pragma omp parallel for
	for (long i = 0; i < size; i++)
	{
		size_t j_0 = i % n;
		size_t i_0 = (i - j_0) / n;

		// A[i][j] = (i + j) % 5
		matrix[i] = (i_0 + j_0) % 5; 
		//matrix[i] = i % 5; // funktioniert auch! Wenn man jedoch die ursprünglichen Werte von i,j benötigt, ist die obere Lösung besser
	}
}

void initMatrixWithNull(type* matrix, const size_t n, const size_t m)
{
	size_t size = n * m;
#pragma omp parallel for
	for (int i = 0; i < size; i++)
	{
		matrix[i] = 0;
	}
}

void matrixVectorMultiplication(type* A, type* b, type* result, const size_t n)
{
	for (int i = 0; i < n; i++) 
	{
		result[i] = 0;
		for (size_t j = 0; j < n; j++)
		{
			result[i] += A[i * n + j] * b[j];
		}
	}
}
