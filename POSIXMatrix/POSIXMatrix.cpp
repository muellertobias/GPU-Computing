// Aufgabe 2 Übung 2

#include <pthread.h> 
#include <ctime>
#include <iostream>

#define _UWIN

const int P = 16;
const int N = 160;
const int rows = (N / P);
float **A = new float*[N];
float b[N];
float c[N];
typedef struct par { int base; long long sum; float sumRows[rows]; } ThreadParameters;
using namespace std;


void initVectorWithRandom(float* vector, int n)
{
	srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		vector[i] = (float)rand() / (float)(RAND_MAX / (float)rand());
		//vector[i] = 1;
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
			//matrix[i][j] = 1;
		}
	}
}

void* threaded_add(void* vp) {
	ThreadParameters* p = (ThreadParameters*)vp;
	for (int i = p->base; i < p->base + N / P; i++) 
	{
		for (int j = 0; j < N; j++)
		{
			c[i] += A[i][j] * b[j];
		}
	}
	return p;
}

int main(int argc, char *argv[])
{
	pthread_t thr[P];
	ThreadParameters param[P];

	cout << "Init..." << endl;
	initVectorWithRandom(b, N);
	initVectorWithNull(c, N);
	initMatrixWithRandom(A, N, N);

	cout << "Sequenz:  Calculate A x b = c ..." << endl;
	//-----Sequenz-----
	clock_t startSeq = clock();

	float cSeq[N];
	initVectorWithNull(cSeq, N);
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
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
		param[i].base = i * (N / P);
		pthread_create(&thr[i], NULL, threaded_add, (void*)&param[i]);
	}

	for (int i = 0; i < P; i++) 
	{
		pthread_join(thr[i], NULL); // wartet auf Threads
	}

	double endThread = (double)(clock() - startThread) / CLOCKS_PER_SEC;
	cout << "Time with threads: " << endThread;

	getchar();
	return 0;
}