#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////
// Comment (or uncomment) the next line to run the single-threaded (or multi-threaded) version, respectively.
//////////////////////////////////////////////////////////////////////////////////
//#define MULTI_THREADED  

//////////////////////////////////////////////////////////////////////////////////
// TODO: Write down your DNI without the final letter
//////////////////////////////////////////////////////////////////////////////////
const unsigned int DNI = 71777616; // Example: const unsigned int DNI = 12345678 

const unsigned short nThreadsMin = 1;
const unsigned short nThreadsMax = 10;

void Task(unsigned long nElements);
int recursiveBinarySearch(int array[], int start_index, int end_index, int element);
void sortArray(int *array, int nElements);

#ifdef MULTI_THREADED
#include <pthread.h>

void * ThreadJob(void* arg)
{
	unsigned long nElements = *((unsigned long*)arg);
	Task(nElements);
	return NULL;
}
#endif

/**********************************************
* MAIN
**********************************************/
int main(int argc, char* argv[])
{
	unsigned long nElements = DNI/10000;
	unsigned int nThreads = DNI%10;

	struct timespec tS1, tE2;
	double dElapsedTimeS;
	int i;

#ifdef MULTI_THREADED
	if (nThreads<2)
		nThreads = 7;
	// printf("Running with %d thread(s)...\n", nThreads);
#endif

	printf("Running task    : ");

//////////////////////////////////////////////////////////////////////////////////
	// Start measuring time
	// TODO: Write here the appropriate instructions to start time measurement
	//////////////////////////////////////////////////////////////////////////////////
	clock_gettime(CLOCK_REALTIME, &tS1);

	
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
	// TODO: Write here the appropriate instructions to finish time measurement
	//////////////////////////////////////////////////////////////////////////////////
	clock_gettime(CLOCK_REALTIME, &tE2);

	printf("Execution finished\n");

	//////////////////////////////////////////////////////////////////////////////////
	// Show the elapsed time
	// TODO: Write here the appropriate instructions to calculate the elapsed time
	//////////////////////////////////////////////////////////////////////////////////
	dElapsedTimeS = (tE2.tv_sec - tS1.tv_sec);
  dElapsedTimeS += (tE2.tv_nsec - tS1.tv_nsec) / 1e+9;

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
	
	for(int j=0;j<nTimes;j++){
		for(int i=0;i<nElements;i++)
			array[i]=rand()%100;

		sortArray(array, nElements);
		recursiveBinarySearch(array, 0, nElements-1, element);
	}
}

/**********************************************
* Auxiliary functions
**********************************************/

void sortArray(int *array, int nElements){
	
	int temp;

	for (int i = 0; i < nElements; i++)
	{     
		for (int j = i+1; j < nElements; j++) 
		{     
			if(array[i] > array[j]) 
			{    
				temp = array[i];    
				array[i] = array[j];    
				array[j] = temp;    
			}     
		}     
	} 
} 

int recursiveBinarySearch(int array[], int start_index, int end_index, int element){
	if (end_index >= start_index)
	{
		int middle = start_index + (end_index - start_index )/2;
		if (array[middle] == element)
			return middle;
		if (array[middle] > element)
			return recursiveBinarySearch(array, start_index, middle-1, element);
		return recursiveBinarySearch(array, middle+1, end_index, element);
	}
	return -1;
}