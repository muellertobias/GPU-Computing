
#include <iostream> 
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <omp.h>
using namespace std;


void random_array(int *a,int *s, int size, int size_s)
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
	#pragma omp parallel for
	for (int i = 0; i < size;i++) {
		
		//temp = rand();
		temp = i;
		j = 0;
		// Prüfung ob Element schon vorhanden
		while (a[j] != -1 && a[j] != temp)
		{
			j++;
		}
		// Element nicht vorhanden
		if (a[j] == -1) {
			a[j] = temp;
			if (last + 10000 < clock()) {
				cout << remain - i << "	Plaetze warten auf Wert 	" << i << "	Plaetze besitzen Wert" << endl;
				last = clock();
			}
		}
		// Element bereits vorhanden
		else
		{
			i--; // Suche für gleichen Platz neue Zahl
		}
	}
	// --------------------------------------Problem-bis------------------------------------------------------------
	int curs = 0;
	for (int k = 0; k < (size_s / 2); k++) {
		s[k] = a[k];
		curs = k + 1;
	}
	srand(time(0));
	for (int l = curs; l < size_s; l++) {
		s[l] = rand();
	}
}
int vgl(const void *x,  const void *y) { // Die Funktion macht den Abfuck noch größer
	return *(int*)x - *(int*)y;
}

int main()
{
	
	const int size = 268435456;
	const int size_s = 10000;
	int *a = new int[size];
	int *s = new int[size_s];
	random_array(a,s, size, size_s);
	qsort(a, sizeof(a)/sizeof(int), sizeof(int), vgl); // Sortieren ... kp das Teil ist ziemlicher abfuck :-D ... Prüfung
	for (int i = 0; i < size;i++) {
		cout << a[i] << endl;
	} 
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