// Binaer.cpp
// Übung 1 - Aufgabe 3
// Julian Kirsch & Tobias Mueller

#include <iostream> 
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <omp.h>
#include <fstream>

using namespace std;

long binarySearch(const unsigned long searchValue, unsigned long *a, const long n)
{
	const long NOT_FOUND = -1;
	long left = 0;
	long right = n - 1;

	while (left <= right)
	{
		long mitte = left + ((right - left) / 2);

		if (a[mitte] == searchValue)
			return mitte;
		else
			if (a[mitte] > searchValue)
				right = mitte - 1;
			else
				left = mitte + 1;
	}

	return NOT_FOUND;
}

void initArrayWithSortedRandomValues(unsigned long *a, unsigned long *s, long size, long size_s)
{
	cout << "init: array a" << endl;
	
	srand(time(0));

	a[0] = (1 + rand() % 3);
	#pragma omp parallel for 
	for (long i = 1; i < size; i++)
	{
		a[i] = a[i - 1] + (1 + rand() % 100);
	}

	cout << "init: array s" << endl;
	#pragma omp parallel for 
	for (int k = 0; k < (size_s / 2); k++) 
	{
		s[k] = a[k];
	}

	srand(time(0));
	#pragma omp parallel for 
	for (int l = size_s / 2; l < size_s; l++) 
	{
		s[l] = (1 + rand()) * rand();
	}
}

void testSchedulings(ofstream &outfile)
{
	const long size = 268435456;
	const long size_s = size / 2;
	unsigned long *a = new unsigned long[size];
	unsigned long *s = new unsigned long[size_s];

	initArrayWithSortedRandomValues(a, s, size, size_s);

	cout << "Search... without threading" << endl;

	int counter = 0;
	clock_t start = clock();
	double end = 0;

	for (int i = 0; i < size_s; i++)
	{
		long index = binarySearch(s[i], a, size);
		if (index != -1)
		{
			counter++;
		}
	}

	end = (double)(clock() - start) / CLOCKS_PER_SEC;
	cout << "Runtime without OpenMP: " << end << endl;
	cout << "Found: " << counter << endl;
	counter = 0;
	start = clock();
	outfile << "Runtime without parallelization: " << end << " s" << endl;
	
	cout << "Search... static" << endl;
#pragma omp parallel for schedule(static) reduction(+:counter)
	for (int i = 0; i < size_s; i++)
	{
		long index = binarySearch(s[i], a, size);
		if (index != -1)
		{
			counter++;
		}
	}
	end = (double)(clock() - start) / CLOCKS_PER_SEC;
	cout << "Runtime with static scheduling: " << end << endl;
	cout << "Found: " << counter << endl;
	counter = 0;
	start = clock();
	outfile << "Runtime with static scheduling: " << end << " s" << endl;

	cout << "Search... static" << endl;
#pragma omp parallel for schedule(static, 1) reduction(+:counter)
	for (int i = 0; i < size_s; i++)
	{
		long index = binarySearch(s[i], a, size);
		if (index != -1)
		{
			counter++;
		}
	}
	end = (double)(clock() - start) / CLOCKS_PER_SEC;
	cout << "Runtime with static scheduling [c=1]: " << end << endl;
	cout << "Found: " << counter << endl;
	counter = 0;
	start = clock();
	outfile << "Runtime with static scheduling [c=1]: " << end << " s" << endl;

	cout << "Search... dynamic" << endl;
#pragma omp parallel for schedule(dynamic) reduction(+:counter)
	for (int i = 0; i < size_s; i++)
	{
		long index = binarySearch(s[i], a, size);
		if (index != -1)
		{
			counter++;
		}
	}
	end = (double)(clock() - start) / CLOCKS_PER_SEC;
	cout << "Runtime  with dynamic scheduling: " << end << endl;
	cout << "Found: " << counter << endl;
	counter = 0;
	start = clock();
	outfile << "Runtime with dynamic scheduling: " << end << " s" << endl;

	cout << "Search... dynamic" << endl;
#pragma omp parallel for schedule(dynamic, 5) reduction(+:counter)
	for (int i = 0; i < size_s; i++)
	{
		long index = binarySearch(s[i], a, size);
		if (index != -1)
		{
			counter++;
		}
	}
	end = (double)(clock() - start) / CLOCKS_PER_SEC;
	cout << "Runtime  with dynamic scheduling [c=5]: " << end << endl;
	cout << "Found: " << counter << endl;
	counter = 0;
	start = clock();
	outfile << "Runtime with dynamic scheduling [c=5]: " << end << " s" << endl;

	cout << "Search... guided" << endl;
#pragma omp parallel for schedule(guided, 5) reduction(+:counter)
	for (int i = 0; i < size_s; i++)
	{
		long index = binarySearch(s[i], a, size);
		if (index != -1)
		{
			counter++;
		}
	}
	end = (double)(clock() - start) / CLOCKS_PER_SEC;
	cout << "Runtime  with guided scheduling [c=5]" << end << endl;
	cout << "Found: " << counter << endl;
	counter = 0;
	start = clock();
	outfile << "Runtime with guided scheduling [c=5]: " << end << " s" << endl;

	cout << "Search... guided" << endl;
#pragma omp parallel for schedule(guided, 1) reduction(+:counter)
	for (int i = 0; i < size_s; i++)
	{
		long index = binarySearch(s[i], a, size);
		if (index != -1)
		{
			counter++;
		}
	}
	end = (double)(clock() - start) / CLOCKS_PER_SEC;
	cout << "Runtime with guided scheduling [c=1]: " << end << endl;
	cout << "Found: " << counter << endl;
	outfile << "Runtime with guided scheduling [c=1]: " << end << " s" << endl;

	delete[] a;
	delete[] s;
}

int main()
{
	ofstream outfile("test.txt");

	clock_t all = clock();

	for (int i = 0; i < 5; i++)
	{
		outfile << i + 1 << ". Iteration:" << endl;
		testSchedulings(outfile);
	}
	outfile.close();

	cout << "Complete runtime: " << (double)(clock() - all) / CLOCKS_PER_SEC << endl;
	cout << "Press any key..." << endl;
	int tmp = 0;
	cin >> tmp;
}
