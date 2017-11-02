
#include <iostream> 
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <omp.h>
#include "Binaer.h"
#include <fstream>

using namespace std;

long binary_search(const unsigned long suchwert, unsigned long *a, const  long eintraege)
{
	const long NICHT_GEFUNDEN = -1;
	long links = 0;
	long rechts = eintraege - 1;

	while (links <= rechts)
	{
		long mitte = links + ((rechts - links) / 2);

		if (a[mitte] == suchwert)
			return mitte;
		else
			if (a[mitte] > suchwert)
				rechts = mitte - 1;
			else
				links = mitte + 1;
	}

	return NICHT_GEFUNDEN;
	// alternativ: return -mitte ; // gute Positionsschaetzung: "nach (-Returnwert) wuerde es hingehoeren"
}

void init_random_array(unsigned long *a, unsigned long *s, long size, long size_s)
{
	cout << "init: array a" << endl;
	
	srand(time(0));

	a[0] = (1 + rand() % 3);
	// parallelisierung nicht möglich 
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

void doTestParallel(ofstream &outfile)
{
	const long size = 268435456;
	const long size_s = size / 2;
	unsigned long *a = new unsigned long[size];
	unsigned long *s = new unsigned long[size_s];

	init_random_array(a, s, size, size_s);

	cout << "Search..." << endl;

	int counter = 0;
	clock_t start = clock();
	double end = 0;
	/*
	for (int i = 0; i < size_s; i++)
	{
		long index = binary_search(s[i], a, size);
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
	*/
//#pragma omp parallel for reduction(+:counter)
//	for (int i = 0; i < size_s; i++)
//	{
//		long index = binary_search(s[i], a, size);
//		if (index != -1)
//		{
//			counter++;
//		}
//	}
//	end = (double)(clock() - start) / CLOCKS_PER_SEC;
//	cout << "Runtime with static scheduling: " << end << endl;
//	cout << "Found: " << counter << endl;
//	counter = 0;
//	start = clock();
//	outfile << "Runtime with static scheduling: " << end << " s" << endl;
//
#pragma omp parallel for schedule(dynamic) reduction(+:counter)
	for (int i = 0; i < size_s; i++)
	{
		long index = binary_search(s[i], a, size);
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

#pragma omp parallel for schedule(guided, 3) reduction(+:counter)
	for (int i = 0; i < size_s; i++)
	{
		long index = binary_search(s[i], a, size);
		if (index != -1)
		{
			counter++;
		}
	}
	end = (double)(clock() - start) / CLOCKS_PER_SEC;
	cout << "Runtime  with guided scheduling [c=2]" << end << endl;
	cout << "Found: " << counter << endl;
	counter = 0;
	start = clock();
	outfile << "Runtime with guided scheduling [c=2]: " << end << " s" << endl;

#pragma omp parallel for schedule(guided, 1) reduction(+:counter)
	for (int i = 0; i < size_s; i++)
	{
		long index = binary_search(s[i], a, size);
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

	for (int i = 0; i < 2; i++)
	{
		outfile << i + 1 << ". Iteration:" << endl;
		doTestParallel(outfile);
	}
	outfile.close();

	cout << "Complete runtime: " << (double)(clock() - all) / CLOCKS_PER_SEC << endl;
	cout << "Press any key..." << endl;
	int tmp = 0;
	cin >> tmp;
}
