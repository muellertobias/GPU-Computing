// POXISTestApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <pthread.h> 
#include <ctime>
#define _UWIN

const int N = 4 * 100000000;
const int P = 4;
int a[N];
typedef struct par { long long base; long long sum; } Par;

void* foo(void* vp) {
	Par* p = (Par*)vp;

	for (int i = p->base; i < p->base + N/P; i++) {
		p->sum += a[i];
	}

	return p;
}

int main(int argc, char *argv[])
{
	pthread_t thr[P];
	Par param[P];
	long long sum = 0;
	for (int i = 0; i < N; i++)
	{
		a[i] = i;
	}
	//-----Sequenz-----
	clock_t startSeq = clock();
	for (int i = 0; i < N; i++) {
		sum += a[i];
	}
	double endSeq = (double)(clock() - startSeq) / CLOCKS_PER_SEC;
	printf("Result: %lld\n", sum);
	printf("Time: %f\n", endSeq);
	sum = 0;

	//-----Thread-----
	clock_t startThread = clock();
	for (int i = 0; i < P; i++) 
	{
		param[i].sum = 0;
		param[i].base = i * (N / P);
		pthread_create(&thr[i], NULL, foo, (void*)&param[i]);
	}
	
	for (int i = 0; i < P; i++)
	{
		pthread_join(thr[i], NULL);
		sum += param[i].sum;
	}
	double endThread = (double)(clock() - startThread) / CLOCKS_PER_SEC;
	printf("Result: %lld\n", sum);
	printf("Time: %f\n", endThread);
	sum = 0;

	
	getchar();
	return 0;
}
