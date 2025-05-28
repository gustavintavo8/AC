/*
 * Main.cpp
 *
 *  Created on: Fall 2019
 */

#include <stdio.h>
#include <math.h>
#include <CImg.h>
#include <time.h>
#include <errno.h>

using namespace cimg_library;

// Data type for image components
// FIXME: Change this type according to your group assignment
typedef double data_t;

const char* SOURCE_IMG1      = "bailarina.bmp";
const char* SOURCE_IMG2      = "background_V.bmp";
const char* DESTINATION_IMG = "resultado.bmp";

// Filter argument data type
typedef struct {
	data_t *pRsrc; // Pointers to the R, G and B components
	data_t *pGsrc;
	data_t *pBsrc;
	data_t *pRsrc2; // Pointers to the R, G and B components
	data_t *pGsrc2;
	data_t *pBsrc2;
	data_t *pRdst;
	data_t *pGdst;
	data_t *pBdst;
	uint pixelCountX; // Size of the image in pixels
	uint pixelCountY; // Size of the image in pixels
} filter_args_t;

/***********************************************
 * 
 * Algorithm. Image filter.
 * In this example, the algorithm is a components swap
 *
 * TO BE REPLACED BY YOUR ALGORITHM
 * 		
 * *********************************************/
void filter (filter_args_t args) {
	/************************************************
	 * FIXME: Algorithm.
	 * In this example, the algorithm is a components swap
	 *
	 * TO BE REPLACED BY YOUR ALGORITHM
	 */
//    for (uint i = 0; i < args.pixelCount; i++) {
//		*(args.pRdst + i) = *(args.pGsrc + i);  // This is equals to pRdest[i] = pGsrc[i]
//		*(args.pGdst + i) = *(args.pBsrc + i);
//		*(args.pBdst + i) = *(args.pRsrc + i);
//	}

    for (uint i = 0; i < args.pixelCountX; i++) {
	*(args.pRdst + i) = 255-((255- *(args.pRsrc + i) * (255- *(args.pRsrc2 + i))/255));
	*(args.pGdst + i) = 255-((255- *(args.pGsrc + i) * (255- *(args.pGsrc2 + i))/255));
	*(args.pBdst + i) = 255-((255- *(args.pBsrc + i) * (255- *(args.pBsrc2 + i))/255));
    }
}

int main() {
	// Open file and object initialization
	CImg<data_t> srcImageX(SOURCE_IMG1); // ABRIR ARCHIVO Y OBJETO DE INICIALIZACION
	CImg<data_t> srcImageY(SOURCE_IMG2);


	filter_args_t filter_args;
	filter_args_t filter_argsY;
	data_t *pDstImage; // Pointer to the new image pixels

	struct timespec tStart, tEnd;
	double dElapsedTimeS;


	/***************************************************
	 * TODO: Variables initialization.
	 *   - Prepare variables for the algorithm
	 *   - This is not included in the benchmark time
	 */

	/*
	srcImage.display(); // Displays the source image
	uint width = srcImage.width();// Getting information from the source image
	uint height = srcImage.height();	
	uint nComp = srcImage.spectrum();// source image number of components
	         // Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)

	// Calculating image size in pixels
	filter_args.pixelCount = width * height;
	*/
	srcImageX.display(); // Displays the source image
	uint widthX = srcImageX.width();// Getting information from the source image
	uint heightX = srcImageX.height();	
	uint nCompX = srcImageX.spectrum();// source image number of components
	         // Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)

	// Calculating image size in pixels
	filter_args.pixelCountX = widthX * heightX;

	srcImageY.display(); // Displays the source image
	uint widthY = srcImageX.width();// Getting information from the source image
	uint heightY = srcImageX.height();	
	uint nCompY = srcImageX.spectrum();// source image number of components
	         // Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)

	// Calculating image size in pixels
	filter_args.pixelCountY = widthY * heightY;

	
	// Allocate memory space for destination image components
	pDstImage = (data_t *) malloc (filter_args.pixelCountX * nCompX * sizeof(data_t));
	if (pDstImage == NULL) {
		perror("Allocating destination image");
		exit(-2);
	}

	
	// Pointers to the componet arrays of the source image
	filter_args.pRsrc = srcImageX.data(); // pRcomp points to the R component array
	filter_args.pGsrc = filter_args.pRsrc + filter_args.pixelCountX; // pGcomp points to the G component array
	filter_args.pBsrc = filter_args.pGsrc + filter_args.pixelCountX; // pBcomp points to B component array
	// Pointers to the componet arrays of the source image
	filter_args.pRsrc2 = srcImageY.data(); // pRcomp points to the R component array
	filter_args.pGsrc2 = filter_args.pRsrc2 + filter_args.pixelCountY; // pGcomp points to the G component array
	filter_args.pBsrc2 = filter_args.pGsrc2 + filter_args.pixelCountY; // pBcomp points to B component array
	
	// Pointers to the RGB arrays of the destination image
	filter_args.pRdst = pDstImage;
	filter_args.pGdst = filter_args.pRdst + filter_args.pixelCountX;
	filter_args.pBdst = filter_args.pGdst + filter_args.pixelCountX;


	/***********************************************
	 * TODO: Algorithm start.
	 *   - Measure initial time
	 */
		printf("Running fusion Single \n");
	if (clock_gettime(CLOCK_REALTIME, &tStart)==-1)
	{
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}


	/************************************************
	 * Algorithm.
	 */
	 if (widthX == widthY && heightX == heightY){
		filter(filter_args);
	} else {
		 printf ("Error, las dimensiones de las imagenes no son iguales.\n");
		  exit (EXIT_FAILURE);
	 }


	/***********************************************
	 * TODO: End of the algorithm.
	 *   - Measure the end time
	 *   - Calculate the elapsed time
	 */
	if (clock_gettime(CLOCK_REALTIME, &tEnd)== -1)
	{
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}
	printf("Finished fusion Single\n");

	dElapsedTimeS= (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS+= (tEnd.tv_nsec - tStart.tv_nsec)/1e+9;
	printf("Elapsed time fusion Single: %f s\n", dElapsedTimeS);

		
	// Create a new image object with the calculated pixels
	// In case of normal color images use nComp=3,
	// In case of B/W images use nComp=1.
	CImg<data_t> dstImage(pDstImage, widthX, heightX, 1, nCompX);

	// Store destination image in disk
	dstImage.save(DESTINATION_IMG); 

	// Display destination image
	dstImage.display();
	
	// Free memory
	free(pDstImage);

	return 0;
}
