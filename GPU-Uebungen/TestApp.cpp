// TestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <omp.h>

int main()
{
	const int n = 10;

	double b[n];
	double a[n];
	for (int i = 0; i < n; i++) 
	{
		a[i] = i;
		b[i] = 0;
	}


	#pragma omp parallel for 
	for (int i = 1; i < n; i++) 
	{
		b[i] = (a[i] + a[i - 1]) / 2.0;
	}

    return 0;
}

