
#include <iostream> 
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <omp.h>

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
	for (long i = 1; i < size; i++)
	{
		a[i] = a[i - 1] + (1 + rand() % 100);
	}

	cout << "init: array s" << endl;
	for (int k = 0; k < (size_s / 2); k++) 
	{
		s[k] = a[k];
	}

	srand(time(0));
	for (int l = size_s / 2; l < size_s; l++) 
	{
		s[l] = (1 + rand()) * rand();
	}
}

int main()
{
	const long size = 268435456;
	const long size_s = 10000;
	unsigned long *a = new unsigned long[size];
	unsigned long *s = new unsigned long[size_s];

	init_random_array(a, s, size, size_s);
	int counter = 0;
	clock_t start = clock();

	//#pragma omp parallel for
	for (int i = 0; i < size_s; i++)
	{
		long index = binary_search(s[i], a, size);
		if (index != -1)
		{
			counter++;
		}
	}

	cout << "Found: " << counter << endl;
	cout << "Runtime: " << (clock() - start) / CLOCKS_PER_SEC << endl;
	cout << "Press any key..." << endl;
	int tmp = 0;
	cin >> tmp;
}
