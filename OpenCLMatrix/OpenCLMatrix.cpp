#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/opencl.h>
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string>
#include <sstream>


char* readSourceFile(const char* filename);
int readBool(const char c);
void printMatrix(double* matrix, int n, int m);
void initVector(double* vector, int n);
void initVectorWithNull(double* vector, int n);
void initMatrix(double* matrix, int n, int m);
void initMatrixWithNull(double* matrix, int n, int m);

void matrixVectorMultiplication(double* A, double* b, double* result, int n);
double magnitudeVector(double* vector, int n);

using namespace std;

int main(int argc, char* argv[])
{
	const char* kernelSource = readSourceFile("Kernel.cl");

	// Host input vectors
	double *h_A;
	double *h_b;

	// Host output vector
	double *h_c;

	// Length of vectors
	unsigned int n = 200;


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


	size_t bytes = n * sizeof(double);  // Size, in bytes, of each vector

	// Allocate memory for each vector on host
	h_A = (double*)malloc(bytes * bytes);
	h_b = (double*)malloc(bytes);
	h_c = (double*)malloc(bytes);

	// Initialize vectors on host
	printf_s("init...\n");
	initVector(h_b, n);
	initVectorWithNull(h_c, n);
	initMatrix(h_A, n, n);

	printf_s("Press 'y' to show matrix or press any key ... ");
	int showMatrix = readBool('y');
	if (showMatrix) 
	{
		printMatrix(h_A, n, n);
		printf_s("\n");
		printMatrix(h_b, 1, n);
	}

	clock_t start_normal = clock();

	matrixVectorMultiplication(h_A, h_b, h_c, n);

	clock_t stop_normal = clock();
	clock_t difference = stop_normal - start_normal;
	double t = (double)difference / CLOCKS_PER_SEC;

	printf_s("Finished Normal: %f s\n", t);
	// ---------------------------------------- Bis hier
	double sum = magnitudeVector(h_c, n);
	printf_s("final result: %f\n", sum / n);

	if (showMatrix) 
	{
		printf_s("Ergebnis:\n");
		printMatrix(h_c, 1, n);
	}
	
	initVectorWithNull(h_c, n);

	size_t globalSize, localSize;
	cl_int err;

	// Number of work items in each local work group
	localSize = 64;

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

	// Create a command queue 
	//queue = clCreateCommandQueue(context, device_id, NULL, &err);
	queue = clCreateCommandQueue(context, device_id, NULL, &err);
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

	// Write our data set into the input array in device memory
	err = clEnqueueWriteBuffer(queue, d_A, CL_TRUE, 0, bytes * bytes, h_A, 0, NULL, NULL);
	err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0, bytes, h_b, 0, NULL, NULL);
	printf("EnqueueWriteBuffers: %d\n", err);

	// Set the arguments to our compute kernel
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
	err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &n);
	printf("SetKernelArgs: %d\n", err);

	// -------------------------------------------------------------------------------------- Start auf GPU
	clock_t start_GPU = clock();
	// Execute the kernel over the entire range of the data set  
	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize, 0, NULL, NULL);

	// Wait for the command queue to get serviced before reading back results
	clFinish(queue);

	// Read the results from the device
	clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0, bytes, h_c, 0, NULL, NULL);

	clock_t stop_GPU = clock();
	difference = stop_GPU - start_GPU;
	t = (double)difference / CLOCKS_PER_SEC;
	printf_s("Finished GPU: %f s\n", t);


	//Sum up vector c and print result divided by n, this should equal 1 within error
	sum = magnitudeVector(h_c, n);
	printf_s("final result: %f\n", sum / n);
	// -------------------------------------------------------------------------------------- Ende GPU
	
	// release OpenCL resources
	clReleaseMemObject(d_A);
	clReleaseMemObject(d_b);
	clReleaseMemObject(d_c);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

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
	while ((input = getchar()) != '\n');
	return input == c;
}

void printMatrix(double* matrix, int n, int m) 
{
	for (int n0 = 0; n0 < n; n0++)
	{
		for (int m0 = 0; m0 < m; m0++)
		{
			printf_s("%f ", matrix[n0 * n + m0]);
		}
		printf_s("\n");
	}
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

void initVector(double* vector, int n)
{
	srand(time(NULL));

#pragma omp parallel for
	for (int i = 0; i < n; i++)
	{
		vector[i] = 5 - (i % 5);
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
	int size = n * n;
#pragma omp parallel for
	for (int i = 0; i < size; i++)
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

void matrixVectorMultiplication(double* A, double* b, double* result, int n) 
{
	/*unsigned int posB;
	int rowCounter = 0;
	int vectorRowCurser = 0;
	int rowAdd = 0;

	for (int i = 0; i < n * n; i++)
	{
		posB = (i % n);
		rowAdd += A[i] * b[posB];
		rowCounter++;
		if (rowCounter == n) {
			result[vectorRowCurser] = rowAdd;
			rowAdd = 0;
			vectorRowCurser++;
			rowCounter = 0;
		}
	}*/

	for (int i = 0; i < n; i++) 
	{
		result[i] = 0;
		for (int j = 0; j < n; j++) 
		{
			result[i] += A[i * n + j] * b[j];
		}
	}
}

double magnitudeVector(double* vector, int n)
{
	double sum = 0;
	for (int i = 0; i < n; i++)
	{
		sum += vector[i];
	}
	return sum;
}