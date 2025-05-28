#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////
// Comment (or uncomment) the previous line to run the single-threaded (or multi-threaded) version, respectively.
//////////////////////////////////////////////////////////////////////////////////
//#define MULTI_THREADED

//////////////////////////////////////////////////////////////////////////////////
// TODO: Write down your DNI without the final letter
//////////////////////////////////////////////////////////////////////////////////
// const unsigned int DNI = *** GAP 1 ***; // Example: const unsigned int DNI = 12345678
const unsigned int DNI = 71779257; // Example: const unsigned int DNI = 12345678

const unsigned short nThreadsMin = 1;
const unsigned short nThreadsMax = 10;

void Task(unsigned long nElements);
int iterativeBinarySearch(int array[], int start_index, int end_index, int element);
void sortArray(int *array, int nElements);

#ifdef MULTI_THREADED
#include <pthread.h>

void *ThreadJob(void *arg)
{
	unsigned long nElements = *((unsigned long *)arg);
	Task(nElements);
	return NULL;
}
#endif

/**********************************************
 * MAIN
 **********************************************/
int main(int argc, char *argv[])
{
	unsigned long nElements = DNI / 10000;
	unsigned int nThreads = DNI % 10;

	struct timespec tIni, tFis;
	double dElapsedTimeS;
	int i;

#ifdef MULTI_THREADED
	if (nThreads < 2)
		nThreads = 7;
		// printf("Running with %d thread(s)...\n", nThreads);
#endif

	printf("Running task    : ");

	//////////////////////////////////////////////////////////////////////////////////
	// Start measuring time
	// TODO: Write here the appropiate instructions to start time measurement
	//////////////////////////////////////////////////////////////////////////////////
	//*** GAP 2 ***
	// Start time measurement
	if (clock_gettime(CLOCK_REALTIME, &tIni) != 0)
	{
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}

#ifdef MULTI_THREADED
	pthread_t Threads[nThreads];
	for (i = 0; i < nThreads; i++)
	{
		int pthread_ret = pthread_create(&Threads[i], NULL, ThreadJob, &nElements);
		if (pthread_ret)
		{
			printf("ERROR: pthread_create error code: %d.\n", pthread_ret);
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < nThreads; i++)
		pthread_join(Threads[i], NULL);

#else
	Task(nElements);

#endif

	//////////////////////////////////////////////////////////////////////////////////
	// Finish measuring time
	// TODO: Write here the appropiate instructions to finish time measurement
	//////////////////////////////////////////////////////////////////////////////////
	//*** GAP 3 ***
	if (clock_gettime(CLOCK_REALTIME, &tFis) != 0)
	{

		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}

	printf("Execution finished\n");

	//////////////////////////////////////////////////////////////////////////////////
	// Show the elapsed time
	// TODO: Write here the appropiate instructions to calculate the elapsed time
	//////////////////////////////////////////////////////////////////////////////////
	//*** GAP 4 ***
	dElapsedTimeS = (tFis.tv_sec - tIni.tv_sec);
    dElapsedTimeS += (tFis.tv_nsec - tIni.tv_nsec) / 1e+9;
	printf("Elapsed time    : %f s.\n", dElapsedTimeS);

	return 0;
}

/**********************************************
 * FUNCION Task
 **********************************************/
void Task(unsigned long nElements)
{
	srand((unsigned)clock());

	int array[nElements];
	int element = 9, nTimes = 10;

	for (int j = 0; j < nTimes; j++)
	{
		for (int i = 0; i < nElements; i++)
			array[i] = rand() % 100;

		sortArray(array, nElements);
		iterativeBinarySearch(array, 0, nElements - 1, element);
	}
}

/**********************************************
 * Auxiliary functions
 **********************************************/
int iterativeBinarySearch(int array[], int start_index, int end_index, int element)
{

	while (start_index <= end_index)
	{
		int middle = start_index + (end_index - start_index) / 2;

		if (array[middle] == element)
			return middle;

		if (array[middle] < element)
			start_index = middle + 1;
		else
			end_index = middle - 1;
	}
	return -1;
}

void sortArray(int *array, int nElements)
{

	int temp;

	for (int i = 0; i < nElements; i++)
	{
		for (int j = i + 1; j < nElements; j++)
		{
			if (array[i] > array[j])
			{
				temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}
}
