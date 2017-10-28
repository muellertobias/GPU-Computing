
#include <iostream> 
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <omp.h>

using namespace std;

void init_random_array(unsigned long *a, unsigned long *s, unsigned long size, unsigned long size_s)
{
	cout << "init: array a" << endl;
	
	srand(time(0));

	a[0] = (1 + rand() % 3);
	// parallelisierung nicht möglich 
	for (unsigned long i = 1; i < size; i++)
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
	const unsigned long size = 268435456;
	const unsigned long size_s = 10000;
	unsigned long *a = new unsigned long[size];
	unsigned long *s = new unsigned long[size_s];

	init_random_array(a, s, size, size_s);

	clock_t start = clock();

	// do it!

	cout << "Laufzeit: " << (clock() - start) / CLOCKS_PER_SEC << endl;
	cout << "Press any key..." << endl;
	int tmp = 0;
	cin >> tmp;
}

/*int binary_search(const int suchwert, int *a, const int eintraege)
{
	const int NICHT_GEFUNDEN = -1;
	int links = 0;                     
	int rechts = eintraege - 1;         

	while (links <= rechts)
	{
		int mitte = links + ((rechts - links) / 2); 

		if (M[mitte] == suchwert)      
			return mitte;
		else
			if (M[mitte] > suchwert)
				rechts = mitte - 1;     
			else 
				links = mitte + 1;      
	}

	return NICHT_GEFUNDEN;
	// alternativ: return -mitte ; // gute Positionsschaetzung: "nach (-Returnwert) wuerde es hingehoeren"
}*/