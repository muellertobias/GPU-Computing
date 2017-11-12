// Aufgabe 2 Übung 2

#include <pthread.h> 
#include <ctime>
#include <iostream>

#define _UWIN

const int P = 4;
const int n = 160;
const int rows = (n / P);
float **A = new float*[n];
float b[n];
float c[n];
typedef struct par { int base; long long sum; float sumRows[rows]; } Par;
using namespace std;


void initVectorWithRandom(float* vector, int n)
{
	srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		vector[i] = (float)rand() / (float)(RAND_MAX / (float)rand());
	}
}

void initVectorWithNull(float* vector, int n)
{
	for (int i = 0; i < n; i++)
	{
		vector[i] = 0;
	}
}

void initMatrixWithRandom(float** matrix, int n, int m)
{
	for (int i = 0; i < n; i++)
	{
		matrix[i] = new float[n];
		for (int j = 0; j < n; j++)
		{
			matrix[i][j] = (float)rand() / (float)(RAND_MAX / (float)rand());
		}
	}
}

void* foo(void* vp) {
	Par* p = (Par*)vp;
	for (int i = p->base; i < p->base + n / P; i++) 
	{
		for (int j = 0; j < n; j++)
		{
			c[i - p->base] += A[i][j] * b[j];
		}
	}
	return p;
}

int main(int argc, char *argv[])
{
	pthread_t thr[P];
	Par param[P];

	cout << "Init..." << endl;
	initVectorWithRandom(b, n);
	initVectorWithNull(c, n);
	initMatrixWithRandom(A, n, n);

	cout << "Sequenz:  Calculate A x b = c ..." << endl;
	//-----Sequenz-----
	clock_t startSeq = clock();

	float cSeq[n];
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
		{
			cSeq[i] += A[i][j] * b[j];
		}
	}

	double endSeq = (double)(clock() - startSeq) / CLOCKS_PER_SEC;
	cout << "Time with sequenz: " << endSeq << endl;

	cout << "4 Threads:  Calculate A x b = c ..." << endl;
	//-----Thread-----
	clock_t startThread = clock();
	for (int i = 0; i < P; i++)
	{
		param[i].sum = 0;
		param[i].base = i*n / P;
		pthread_create(&thr[i], NULL, foo, (void*)&param[i]);
	}

	for (int i = 0; i < P; i++) 
	{
		pthread_join(thr[i], NULL);
	}

	double endThread = (double)(clock() - startThread) / CLOCKS_PER_SEC;
	cout << "Time with threads: " << endThread;

	int ende = 0;
	scanf_s("%d", ende);
	return 0;
}