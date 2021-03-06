// POXISTestApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdlib.h>
#include <pthread.h> 
#include <ctime>
#define _UWIN

const int N = 4 * 10000;
const int P = 4;

typedef struct 
{ 
	int id;
	unsigned long* values;
	long nValues; 
	int nThreads;
	pthread_mutex_t lock;
	unsigned long* sum;
} ThreadParameters;

void* threaded_add(void* parameters) 
{
	ThreadParameters* param = (ThreadParameters*)parameters;

	int begin = param->id * (int)((double)param->nValues / (double)param->nThreads);
	int end = begin + (int)((double)param->nValues / (double)param->nThreads);
	unsigned long sum = 0;
	for (unsigned long i = begin; i < end; i++)
	{
		sum += param->values[i];
	}

	pthread_mutex_lock(&param->lock);
	*(param->sum) += sum;
	pthread_mutex_unlock(&param->lock);

	return 0;
}

int main(int argc, char *argv[])
{
	unsigned long *values = (unsigned long *)malloc(N * sizeof(unsigned long));

	pthread_t thr[P];
	ThreadParameters param[P];
	unsigned long sum = 0UL;
	for (unsigned long i = 0; i < N; i++)
	{
		values[i] = i;
	}
	//-----Sequenz-----
	clock_t startSeq = clock();
	for (unsigned long i = 0; i < N; i++) 
	{
		sum += values[i];
	}
	double endSeq = (double)(clock() - startSeq) / CLOCKS_PER_SEC;
	printf("Result: %lld\n", sum);
	printf("Time: %f\n", endSeq);
	sum = 0;

	//-----Thread-----
	clock_t startThread = clock();
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	for (unsigned long i = 0; i < P; i++)
	{
		param[i].id = i;
		param[i].nThreads = P;
		param[i].nValues = N;
		param[i].sum = &sum;
		param[i].values = values;
		param[i].lock = lock;
		pthread_create(&thr[i], NULL, threaded_add, (void*)&param[i]);
	}
	
	for (int i = 0; i < P; i++)
	{
		pthread_join(thr[i], NULL);
	}
	double endThread = (double)(clock() - startThread) / CLOCKS_PER_SEC;
	printf("Result: %lld\n", sum);
	printf("Time: %f\n", endThread);
	
	free(values);
	getchar();
	return 0;
}
