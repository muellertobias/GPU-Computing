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
	err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_device_info), &maxFrequence, &cl_uint_size);
	printf("Max. Freq: %d\n", maxFrequence);

	char* deviceName = NULL;
	size_t cl_string_size = 0;
	err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(cl_device_info), deviceName, &cl_string_size);
	if (deviceName != NULL) 
	{
		std::cout << "Device Name: " << deviceName << std::endl;
	}

	int a = 0;
	std::cin >> a;
	return 0;
}

