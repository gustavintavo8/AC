/*
 * Main.cpp
 *
 *  Created on: Fall 2019
 */

#include <stdio.h>
#include <math.h>
#include <CImg.h>
#include <immintrin.h>

using namespace cimg_library;

#define REPETICIONES 30

#define ITEMS_PER_PACKET (sizeof(__m128)/sizeof(data_t))

// Data type for image components
// FIXME: Change this type according to your group assignment
typedef double data_t;

const char* SOURCE_IMG_1      = "bailarina.bmp";
const char* SOURCE_IMG_2      = "flores_3.bmp";
const char* DESTINATION_IMG = "bailarina2.bmp";

// Filter argument data type
typedef struct {
	data_t *pRsrc1; // Pointers to the R, G and B components
	data_t *pGsrc1;
	data_t *pBsrc1;
	data_t *pRsrc2; // Pointers to the R, G and B components
	data_t *pGsrc2;
	data_t *pBsrc2;
	data_t *pRdst;
	data_t *pGdst;
	data_t *pBdst;
	uint pixelCount; // Size of the image in pixels
	uint width;
	uint height;
	float blendLevel;
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

	args.blendLevel = 0.5; //blend level C in range [0,1]

    __m128d vA, vB;
	__m128d vConstC = _mm_set1_pd(args.blendLevel);
	__m128d vConst1 = _mm_set1_pd(1);
	__m128d vConst255 = _mm_set1_pd(255);


	for (uint i = 0; i < args.height ; i++ ) {
		for (uint j=0; j < args.width; j=j+ITEMS_PER_PACKET)  {
			
			int index=i*args.width+j;
			__m128d vC;

			//Calculate the R component
			vA= _mm_load_pd(args.pRsrc1+index);
			vB= _mm_load_pd(args.pRsrc2+index);
			
		
			vA = _mm_mul_pd(vConstC,vA);
			//
			vC = _mm_sub_pd(vConst1,vConstC);
			//
			vB = _mm_mul_pd(vC,vB);
			//
			vA = _mm_add_pd(vA,vB);

			vA = _mm_min_pd(vA,vConst255);
			
			_mm_store_pd((args.pRdst+index),vA);
			
			
			//Calculate the G component
			vA= _mm_load_pd(args.pGsrc1+index);
			vB= _mm_load_pd(args.pGsrc2+index);
			
		
			vA = _mm_mul_pd(vConstC,vA);
			//
			vC = _mm_sub_pd(vConst1,vConstC);
			//
			vB = _mm_mul_pd(vC,vB);
			//
			vA = _mm_add_pd(vA,vB);

			vA = _mm_min_pd(vA,vConst255);
			
			_mm_store_pd((args.pGdst+index),vA);



			//Calculate the B component
			vA= _mm_load_pd(args.pBsrc1+index);
			vB= _mm_load_pd(args.pBsrc2+index);
			
		
			vA = _mm_mul_pd(vConstC,vA);
			//
			vC = _mm_sub_pd(vConst1,vConstC);
			//
			vB = _mm_mul_pd(vC,vB);
			//
			vA = _mm_add_pd(vA,vB);

			vA = _mm_min_pd(vA,vConst255);
			
			_mm_store_pd((args.pBdst+index),vA);
			
			
		}

		uint width_simd = ITEMS_PER_PACKET * (args.width / ITEMS_PER_PACKET);

		for (uint j = width_simd; j <= args.width; j++)
		{
			int index=i*args.width+j;

			*(args.pRdst + index) = args.blendLevel * *(args.pRsrc1 + index) + (1 - args.blendLevel) * *(args.pRsrc2 + index);  // This is equals to pRdest[i] = C * pGsrc1[i] + (1 - C) * pGsrc2[i]
			*(args.pGdst + index) = args.blendLevel * *(args.pGsrc1 + index) + (1 - args.blendLevel) * *(args.pGsrc2 + index);
			*(args.pBdst + index) = args.blendLevel * *(args.pBsrc1 + index) + (1 - args.blendLevel) * *(args.pBsrc2 + index);
		}
	}


}

int main() {
	// Open file and object initialization
	CImg<data_t> srcImage1(SOURCE_IMG_1);
	CImg<data_t> srcImage2(SOURCE_IMG_2);

	filter_args_t filter_args;
	data_t *pDstImage; // Pointer to the new image pixels

	struct timespec tStart, tEnd;
	double dElapsedTimeS;


	/***************************************************
	 * TODO: Variables initialization.
	 *   - Prepare variables for the algorithm
	 *   - This is not included in the benchmark time
	 */

	srcImage1.display(); // Displays the source image
	srcImage2.display(); // Displays the source image

	uint width1 = srcImage1.width();// Getting information from the source image
	uint height1 = srcImage1.height();	
	uint nComp1 = srcImage1.spectrum();// source image number of components

	uint width2 = srcImage2.width();// Getting information from the source image
	uint height2 = srcImage2.height();	
	uint nComp2 = srcImage2.spectrum();// source image number of components
	         // Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)

	// Calculating image size in pixels
	filter_args.pixelCount = width1 * height1;
	filter_args.width = width1;
	filter_args.height = height1;
	
	// Allocate memory space for destination image components
	pDstImage = (data_t *)_mm_malloc(filter_args.pixelCount * nComp1 * sizeof(data_t), sizeof(__m128));
	if (pDstImage == NULL) {
		perror("Allocating destination image");
		exit(-2);
	}

	// Pointers to the componet arrays of the source image
	filter_args.pRsrc1 = srcImage1.data(); // pRcomp points to the R component array
	filter_args.pGsrc1 = filter_args.pRsrc1 + filter_args.pixelCount; // pGcomp points to the G component array
	filter_args.pBsrc1 = filter_args.pGsrc1 + filter_args.pixelCount; // pBcomp points to B component array

	// Pointers to the componet arrays of the source image
	filter_args.pRsrc2 = srcImage2.data(); // pRcomp points to the R component array
	filter_args.pGsrc2 = filter_args.pRsrc2 + filter_args.pixelCount; // pGcomp points to the G component array
	filter_args.pBsrc2 = filter_args.pGsrc2 + filter_args.pixelCount; // pBcomp points to B component array
	
	// Pointers to the RGB arrays of the destination image
	filter_args.pRdst = pDstImage;
	filter_args.pGdst = filter_args.pRdst + filter_args.pixelCount;
	filter_args.pBdst = filter_args.pGdst + filter_args.pixelCount;

	//Check if both images have the same width, the same height and the same number of components
	if(width1 != width2){
		printf("\n Las imágenes tienen achuras diferentes \n");
		exit(EXIT_FAILURE);
	}
	else if(height1 != height2){
		printf("\n Las imágenes tienen alturas diferentes \n");
		exit(EXIT_FAILURE);
	}
	else if(nComp1 != nComp2){
		printf("\n Las imágenes tienen distinto número de componentes");
		exit(EXIT_FAILURE);
	}


	/***********************************************
	 * TODO: Algorithm start.
	 *   - Measure initial time
	 */
	printf("Start: \n");
	if (clock_gettime(CLOCK_REALTIME, &tStart))
	{
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}


	/************************************************
	 * Algorithm.
	 */
	for (int i = 0; i < REPETICIONES; i++)
	{
		filter(filter_args);
	}

	/***********************************************
	 * TODO: End of the algorithm.
	 *   - Measure the end time
	 *   - Calculate the elapsed time
	 */

	if (clock_gettime(CLOCK_REALTIME, &tEnd) == -1)
	{
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}


	dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
	printf("Elapsed time   : %f s.\n", dElapsedTimeS);
	printf("Finished\n");
		
	// Create a new image object with the calculated pixels
	// In case of normal color images use nComp=3,
	// In case of B/W images use nComp=1.
	CImg<data_t> dstImage(pDstImage, width1, height1, 1, nComp1);

	// Store destination image in disk
	dstImage.save(DESTINATION_IMG); 

	// Display destination image
	dstImage.display();
	
	// Free memory
	_mm_free(pDstImage);

	return 0;
}
