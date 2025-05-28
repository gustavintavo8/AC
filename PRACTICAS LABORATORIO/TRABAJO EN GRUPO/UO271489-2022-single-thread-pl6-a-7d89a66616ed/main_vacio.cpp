/*
 * Main.cpp
 *
 *  Created on: 13 sept. 2018
 *      Author: arias
 */

#include <CImg.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>

using namespace cimg_library;
using namespace std;


/**********************************
 * TODO
 * 	- Change the data type returned by CImg.srcImage to adjust the
 * 	requirements of your workgroup
 * 	- Change the data type of the components pointers to adjust the
 * 	requirements of your workgroup
 */

int main() {
	CImg<float> srcImage("bailarina.bmp"); // Open file and object initialization
    CImg<float> srcBackground("background.bmp"); // Open file and background initialization
    CImg<float> destination("resultado.bmp"); // Open file and object initialization
	float *pRcomp, *pGcomp, *pBcomp; // Pointers to the R, G and B components
	float *pRnew, *pGnew, *pBnew;
	float *pdstImage; // Pointer to the new image pixels
	int width, height; // Width and height of the image
	int nComp; // Number of image components
    double dTimeS;


	/***************************************************
	 *
	 * Variables initialization.
	 * Preparation of the necessary elements for the algorithm
	 * Out of the benchmark time
	 *
	 */

	srcImage.display(); // If needed, show the source image
	width = srcImage.width(); // Getting information from the source image
	height  = srcImage.height();
	nComp = srcImage.spectrum(); // source image number of components
				// Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)


	// Allocate memory space for the pixels of the destination (processed) image 
	pdstImage = (float *) malloc (width * height * nComp * sizeof(float));
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
     * 
     * Blend of two images (X and Y) into image I
	 */


	for (uint i = 0; i < args.pixelCountX; i++) {
		for (uint j = 0; j < args.pixelCountY; j++) {
			*(args.pRdst + i + j * args.pixelCountX) = 255-((255- *(args.pRsrc + i + j * args.pixelCountX) * (255- *(args.pRsrc2 + i + j * args.pixelCountX))/255));
			*(args.pGdst + i + j * args.pixelCountX) = 255-((255- *(args.pGsrc + i + j * args.pixelCountX) * (255- *(args.pGsrc2 + i + j * args.pixelCountX))/255));
			*(args.pBdst + i + j * args.pixelCountX) = 255-((255- *(args.pBsrc + i + j * args.pixelCountX) * (255- *(args.pBsrc2 + i + j * args.pixelCountX))/255));
		}
	}

	/***********************************************
	 * End of the algorithm
	 *
	 * Measure the final time and calculate the time spent
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
	CImg<float> dstImage(pdstImage, width, height, 1, nComp);

	// Store the destination image in disk
	dstImage.save("bailarina2.bmp"); 

	// Display the destination image
	dstImage.display(); // If needed, show the result image
	return(0);

}