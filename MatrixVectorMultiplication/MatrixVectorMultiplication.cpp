// MatrixVectorMultiplication.cpp
// Übung 1 - Aufgabe 2
// Julian Kirsch & Tobias Mueller

#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <omp.h>

using namespace std;

void initVectorWithRandom(float* vector, int n) 
{
	srand(time(NULL));
	#pragma omp parallel for
	for (int i = 0; i < n; i++) 
	{
		vector[i] = (float)rand() / (float)(RAND_MAX / (float)rand());
	}
}

void initVectorWithNull(float* vector, int n)
{
	#pragma omp parallel for
	for (int i = 0; i < n; i++)
	{
		vector[i] = 0;
	}
}

void initMatrixWithNull(float** matrix, int n, int m) 
{
	#pragma omp parallel for
	for (int i = 0; i < n; i++)
	{
		matrix[i] = new float[n];
		for (int j = 0; j < n; j++)
		{
			matrix[i][j] = 0;
		}
	}
}

int main()
{
	const int n = 50000;

	float **a = new float*[n];
	float b[n];
	float c[n];

	cout << "Init..." << endl;
	initVectorWithRandom(b, n);
	initVectorWithNull(c, n);
	initMatrixWithNull(a, n, n);

	cout << "Calculate A x b = c ..." << endl;
	clock_t start = clock();

	#pragma omp parallel for // 1,67s (n=29000, speedup=66%), 6,9s (n=40000, speedup=52%), 0,745s (n=20000, speedup=70%), 2,6s (n=35000, speedup=65%), 14,2s (n=50000)
	for (int i = 0; i < n; i++) 
	{
		for (int j = 0; j < n; j++) 
		{
			c[i] += a[i][j] * b[j];
		}
	}

	clock_t stop = clock();
	clock_t difference = stop - start;
	double t = (double)difference / CLOCKS_PER_SEC; 
	cout << "Finished: " << t  << "s" << endl; // ohne OpenMP 5,2s (n=29000), 14,24s (n=40000), 2,46s (n=20000), 7,55s (n=35000), 22,4s
	cout << "Press any key..." << endl;
	int tmp = 0;
	cin >> tmp;

	return 0;
}

