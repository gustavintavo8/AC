/*
 * Main.cpp
 *
 *  Created on: 09/10/2018
 *      Author: pl5-c
 */
#include <CImg.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
using namespace cimg_library;
using namespace std;
#define NTIMES 10

int width, height; // Width and height of the image
const int N = 16; //Numero de hilos. Tengo 4 cpu con 4 nucleos cada una. No hay hyperthreading.
int *pdstImage; // Pointer to the new image pixels
CImg<int> srcImage("bailarina.bmp"); // Open file and object initialization
float C, T;


void* ThreadProc(void* arg)
{
	int *pRcomp, *pGcomp, *pBcomp; // Pointers to the R, G and B components
	int *pRnew, *pGnew, *pBnew;
	int npixels = (height * width)/N; //numero de pixels que operara cada hilo
	int i = *((int*)arg); //el hilo que esta operando

	//printf("Thread %d started.\n", i);

	// Pointers to the RGB arrays of the source image
	pRcomp = srcImage.data() + npixels * i;
	pGcomp = pRcomp + height * width;
	pBcomp = pGcomp + height * width;

	// Pointers to the RGB arrays of the destination image
	pRnew = pdstImage + npixels * i;
	pGnew= pRnew + height * width;
	pBnew= pGnew + height * width;

	for (int j = 0; j < npixels; j++){
		*pRnew = (float)*pRcomp*C;
		if(*pRnew <0){ *pRnew= 0;
		}else if(*pRnew>255){*pRnew=255;}

		*pGnew = (float)*pGcomp*C;
		if(*pGnew <0){*pGnew= 0;
		}else if(*pGnew>255){*pGnew=255;}

		*pBnew = (float)*pBcomp*C;
		if(*pBnew <0){*pBnew= 0;
		}else if(*pBnew>255){*pBnew=255;}

		*pRnew = (float)*pRnew * 0.393 + (float)*pGnew * 0.769 + (float)*pBnew * 0.189;
		if(*pRnew <0){ *pRnew= 0;
		}else if(*pRnew>255){*pRnew=255;}

		*pGnew = (float)*pRnew * 0.349 + (float)*pGnew * 0.686 + (float)*pBnew * 0.168;
		if(*pGnew <0){*pGnew= 0;
		}else if(*pGnew>255){*pGnew=255;}

		*pBnew = (float)*pRnew * 0.272 + (float)*pGnew * 0.534 + (float)*pBnew * 0.131;
		if(*pBnew <0){*pBnew= 0;
		}else if(*pBnew>255){*pBnew=255;}

		pRnew++;  pRcomp++;
		pGnew++;  pGcomp++;
		pBnew++;  pBcomp++;
	}

	//printf("Thread %d finished.\n", i);
	return NULL;
}



int main() {
	pthread_t thread[N]; // array de hilos
	int i;
	int a[N];
	int nComp; // Number of image component
	struct timespec tStart, tEnd;
	double dTimeS;


	/***************************************************
	 *
	 * Variables inicialization.
	 * Preparation of the necessary elements for the algorithm
	 * Out of the benchmark time
	 *
	 */
	T = 5.0;
	if (T < -100){	T = -100;
	}else if (T > 100){	T = 100;}
	C = pow((100 + T)/100, 2);

	srcImage.display(); // If needed, show the source image
	width = srcImage.width(); // Getting information from the source image
	height  = srcImage.height();
	nComp = srcImage.spectrum(); // source image number of components

	// Allocate memory space for the pixels of the destination (processed) image
	pdstImage = (int *) malloc (width * height * nComp * sizeof(int));
	if (pdstImage == NULL) {
		printf("\nMemory allocating error\n");
		exit(-2);
	}


	if (clock_gettime(CLOCK_REALTIME, &tStart) < 0){
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}
	for(int x = 0; x < NTIMES; x++)
	{
		// Thread creation
		for (i = 0; i < N; i++)
		{
			a[i]= i;
			if (pthread_create(&thread[i], NULL, ThreadProc, &a[i]) != 0)
			{
				fprintf(stderr, "ERROR: Creating thread %d\n", i);
				return EXIT_FAILURE;
			}
		}

		// Wait till the completion of all threads
		//printf("Main thread waiting...\n");
		for (i = 0; i < N; i++)
		{
			pthread_join(thread[i], NULL);
		}
		//printf("Main thread finished.\n");
	}
	if (clock_gettime(CLOCK_REALTIME, &tEnd) < 0){
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}

	dTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;

	printf("Total time (s): %f s.\n", dTimeS);

	// Create a new image object with the calculated pixels
	// In case of normal color image use nComp=3,
	// In case of B&W image use nComp=1.
	CImg<int> dstImage(pdstImage, width, height, 1, nComp);

	// Store the destination image in disk
	dstImage.save("bailarina2.bmp");

	// Display the destination image
	dstImage.display(); // If needed, show the result image
	return(0);

}

