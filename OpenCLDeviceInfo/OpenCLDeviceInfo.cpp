// OpenCLDeviceInfo.cpp : Defines the entry point for the console application.
// https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clGetDeviceInfo.html

#include "stdafx.h"
#include <iostream>
#include <CL/opencl.h>

int main()
{
	cl_platform_id cpPlatform;		  // OpenCL platform
	cl_device_id device_id;           // device ID

	cl_int err;

	cl_uint maxFrequence = 0;

	err = clGetPlatformIDs(1, &cpPlatform, NULL);
	printf("GetPlatfrom: %d\n", err);

	// Get ID for the device
	err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
	printf("GetDeviceIDs: %d\n", err);

	size_t cl_uint_size = sizeof(cl_uint);
	err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &maxFrequence, NULL);
	printf("Max. Freq: %d\n", maxFrequence);

	char deviceName[64];
	size_t cl_string_size = 0;
	err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, 64 * sizeof(char), deviceName, &cl_string_size);
	printf("Device Name: %s\n", deviceName);

	getchar();
	return 0;
}

