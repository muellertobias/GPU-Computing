
#include <iostream> 
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <omp.h>
using namespace std;

unsigned long long ulrand() 
{
	return (static_cast<long>(rand()) << (sizeof(int) * 8)) | rand();
}

void random_array(unsigned long *a, unsigned long *s, unsigned long size, unsigned long size_s)
{
	cout << "-1 init" << endl;
	for (int i = 0; i < size; i++) {
		a[i] = -1;
	}
	cout << "random init" << endl;
	int j;
	int temp;
	int remain = size;
	clock_t last = clock();
	// --------------------------------------Problem-von------------------------------------------------------------
	srand(time(0));

	a[0] = (1 + rand() % 3);
	//#pragma omp parallel for
	for (unsigned long i = 1; i < size; i++)
	{
		a[i] = a[i - 1] + (1 + rand() % 100);
	}
	// --------------------------------------Problem-bis------------------------------------------------------------
	/*int curs = 0;
	for (int k = 0; k < (size_s / 2); k++) {
		s[k] = a[k];
		curs = k + 1;
	}
	srand(time(0));
	for (int l = curs; l < size_s; l++) {
		s[l] = rand();
	}*/
}
int vgl(const void *x,  const void *y) { // Die Funktion macht den Abfuck noch größer
	return *(unsigned long*)x - *(unsigned long*)y;
}

int main()
{
	const unsigned long size = 268435456;
	const unsigned long size_s = 10000;
	unsigned long *a = new unsigned long[size];
	unsigned long *s = new unsigned long[size_s];
	
	random_array(a, s, size, size_s);
	/*for (unsigned long i = 0; i < size; i++) {
		cout << a[size - i - 1] << endl;
	}*/

	qsort(a, sizeof(a)/sizeof(unsigned long), sizeof(unsigned long), vgl); // Sortieren ... kp das Teil ist ziemlicher abfuck :-D ... Prüfung
	
	/*for (unsigned long i = 0; i < size; i++) {
		cout << a[size - i - 1] << endl;
	} */

	cout << a[size - 1] << endl;
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