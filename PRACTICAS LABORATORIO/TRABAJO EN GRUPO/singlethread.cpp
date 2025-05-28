/*
 * Main.cpp
 *
 *  Created on: 9 nov. 2018
 *      Author: pl5_c
 */

#include <CImg.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
using namespace cimg_library;
using namespace std;
#define NTIMES 10

/**********************************
 * TODO
 * 	- Change the data type returned by CImg.srcImage to adjust the
 * 	requirements of your workgroup
 * 	- Change the data type of the components pointers to adjust the
 * 	requirements of your workgroup
 */

int main() {
	CImg<int> srcImage("bailarina.bmp"); // Open file and object initialization

	int *pRcomp, *pGcomp, *pBcomp; // Pointers to the R, G and B components
	int *pRnew, *pGnew, *pBnew;
	int *pdstImage; // Pointer to the new image pixels
	int width, height; // Width and height of the image
	int nComp; // Number of image components
	float C, T;
	struct timespec tStart, tEnd;
	double dTimeS;
	/***************************************************
	 *
	 * Variables initialization.
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
				// Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)


	// Allocate memory space for the pixels of the destination (processed) image 
	pdstImage = (int *) malloc (width * height * nComp * sizeof(int));
	if (pdstImage == NULL) {
		printf("\nMemory allocating error\n");
		exit(-2);
	}

	// Pointers to the RGB arrays of the source image
	pRcomp = srcImage.data(); // pRcomp points to the R component
	pGcomp = pRcomp + height * width; // pGcomp points to the G component
	pBcomp = pGcomp + height * width; // pBcomp points to B component

	// Pointers to the RGB arrays of the destination image
	pRnew = pdstImage;
	pGnew= pRnew + height * width;
	pBnew= pGnew + height * width;
	/*********************************************
	 * Algorithm start
	 *
	 * Measure initial time
	 *
	 *	COMPLETE
	 *
	 */
	if (clock_gettime(CLOCK_REALTIME, &tStart) < 0){
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}

	/************************************************
	 * Algorithm.
	 * In this example, the algorithm is a components exchange
	 *
	 * TO BE REPLACED BY YOUR ALGORITHM
	 */
	for (int i =0; i < NTIMES; i++){
		int npixels = height * width;

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

		pRnew-=npixels; pRcomp-=npixels;
		pGnew-=npixels; pGcomp-=npixels;
		pBnew-=npixels; pBcomp-=npixels;
	}

	/***********************************************
	 * End of the algorithm
	 *
	 * Measure the final time and calculate the time spent
	 *
	 *
	 * COMPLETE
	 *
	 */

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

