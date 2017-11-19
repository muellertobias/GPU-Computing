// Aufgabe 2 Übung 2

#include <pthread.h> 
#include <ctime>
#include <iostream>

#define _UWIN

const int P = 4;
const int N = 160;

typedef struct
{
	int id;
	float** matrixA;
	float* vectorB;
	float* vectorC;
	long nValues;
	int nThreads;
} ThreadParameters;

using namespace std;

double magnitudeVector(float* vector, const unsigned int n)
{
	double sum = 0;
	for (int i = 0; i < n; i++)
	{
		sum += vector[i];
	}
	return sum;
}

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

void* threaded_add(void* parameters)
{
	ThreadParameters* param = (ThreadParameters*)parameters;

	float** A = param->matrixA;
	float* c = param->vectorC;
	float* b = param->vectorB;

	int begin = param->id * (int)((double)param->nValues / (double)param->nThreads);
	int end = begin + (int)((double)param->nValues / (double)param->nThreads);

	for (unsigned long i = begin; i < end; i++)
	{
		for (int j = 0; j < N; j++)
		{
			c[i] += A[i][j] * b[j];
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	float **A = new float*[N];
	float b[N];
	float c[N];

	cout << "Init..." << endl;
	initVectorWithRandom(b, N);
	initVectorWithNull(c, N);
	initMatrixWithRandom(A, N, N);

	cout << "Sequenz:  Calculate A x b = c ..." << endl;
	//-----Sequenz-----
	clock_t startSeq = clock();

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
		{
			c[i] += A[i][j] * b[j];
		}
	}

	double endSeq = (double)(clock() - startSeq) / CLOCKS_PER_SEC;

	cout << "Time with sequenz: " << endSeq << endl;
	cout << "|c| = " << magnitudeVector(c, N) << endl;
	initVectorWithNull(c, N);
	cout << "4 Threads:  Calculate A x b = c ..." << endl;

	//-----Thread-----
	pthread_t thr[P];
	ThreadParameters param[P];
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

	clock_t startThread = clock();
	for (int i = 0; i < P; i++)
	{
		param[i].id = i;
		param[i].nThreads = P;
		param[i].nValues = N;
		param[i].matrixA = A;
		param[i].vectorB = b;
		param[i].vectorC = c;
		pthread_create(&thr[i], NULL, threaded_add, (void*)&param[i]);
	}

	for (int i = 0; i < P; i++) 
	{
		pthread_join(thr[i], NULL); // wartet auf Threads
	}

	double endThread = (double)(clock() - startThread) / CLOCKS_PER_SEC;
	cout << "Time with threads: " << endThread << endl;
	cout << "|c| = " << magnitudeVector(c, N) << endl;
	getchar();
	return 0;
}