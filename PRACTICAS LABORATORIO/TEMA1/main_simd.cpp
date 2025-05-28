#include <CImg.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <immintrin.h>

using namespace cimg_library;

typedef double data_t;

#define ITEMS_PER_PACKET (sizeof(__m256)/sizeof(data_t))

const char* SOURCE_IMG      = "bailarina.bmp";
const char* DESTINATION_IMG = "bailarina2.bmp";
const char* SOURCE_IMG2		= "background_V.bmp";

int main() {
	
	CImg<data_t> srcImageX(SOURCE_IMG);
	CImg<data_t> srcImageY(SOURCE_IMG2);
	
	data_t *pRsrcX, *pGsrcX, *pBsrcX,*pRsrcY, *pGsrcY, *pBsrcY;
	data_t *pRdest, *pGdest, *pBdest;
	data_t *pDstImage;
	uint width, height, widthY, heightY;
	uint nComp;
	uint index;
	
	
	//srcImageX.display(); // Displays the source image
	width  = srcImageX.width(); // Getting information from the source image
	height = srcImageX.height();
	nComp  = srcImageX.spectrum(); // source image number of srconents
	//srcImageY.display();
	widthY  = srcImageY.width(); // Getting information from the source image
	heightY = srcImageY.height();
	nComp  = srcImageY.spectrum();
	
	pDstImage = (data_t *) malloc (width * height * nComp * sizeof(data_t));
	if (pDstImage == NULL) {
		perror("Allocating destination image");
		exit(-2);
	}
	
	pRsrcX = srcImageX.data(); // pRsrc points to the R srconent array
	pGsrcX = pRsrcX + height * width; // pGsrc points to the G srconent array
	pBsrcX = pGsrcX + height * width; // pBsrc points to B srconent array
	
	pRsrcY = srcImageY.data(); // pRsrc points to the R srconent array
	pGsrcY = pRsrcY + height * width; // pGsrc points to the G srconent array
	pBsrcY = pGsrcY + height * width; // pBsrc points to B srconent array

	// Pointers to the RGB arrays of the destination image
	pRdest = pDstImage;
	pGdest = pRdest + height * width;
	pBdest = pGdest + height * width;
	
	struct timespec tStart, tEnd;
	double dElapsedTimeS;

	printf("Running fusion SIMD \n");
	if (clock_gettime(CLOCK_REALTIME, &tStart)==-1)
	{
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}
	
	__m256d vConst= _mm256_set1_pd(255);
	__m256d vConst2= _mm256_set1_pd(2);
	__m256d vComp1, vComp2;
	
	
	
	for(int k=1;k<=7;k++){
		for (uint i = 0; i < height ; i++ ) {
			for (uint j=0; j < width; j=j+ITEMS_PER_PACKET)  {
				
				index=i*width+j;

				vComp1= _mm256_loadu_pd(pRsrcX+index);
				vComp2= _mm256_loadu_pd(pRsrcY+index);

				// a = Y/255
				__m256d aR =_mm256_div_pd(vComp2,vConst);
				// b = 255 - Y
				__m256d bR =_mm256_sub_pd(vConst, vComp2);
				// c = 2*X
				__m256d cR = _mm256_mul_pd(vConst2,vComp1);
				// d = c / 255
				__m256d dR = _mm256_div_pd(cR, vConst);
				// e = b * d
				__m256d eR = _mm256_mul_pd(bR,dR);
				// f = Y + e 
				__m256d fR = _mm256_add_pd(vComp2,eR);
				// resultado = a * f
				__m256d resultadoR = _mm256_mul_pd(aR,fR);

				_mm256_storeu_pd ((pRdest+index),resultadoR);

				vComp1= _mm256_loadu_pd(pGsrcX+index);
				vComp2= _mm256_loadu_pd(pGsrcY+index);

				// a = Y/255
				__m256d aG =_mm256_div_pd(vComp2,vConst);
				// b = 255 - Y
				__m256d bG =_mm256_sub_pd(vConst, vComp2);
				// c = 2*X
				__m256d cG = _mm256_mul_pd(vConst2,vComp1);
				// d = c / 255
				__m256d dG = _mm256_div_pd(cG, vConst);
				// e = b * d
				__m256d eG = _mm256_mul_pd(bG,dG);
				// f = Y + e 
				__m256d fG = _mm256_add_pd(vComp2,eG);
				// resultado = a * f
				__m256d resultadoG = _mm256_mul_pd(aG,fG);
				
				_mm256_storeu_pd ((pGdest+index),resultadoG);

				vComp1= _mm256_loadu_pd(pBsrcX+index);
				vComp2= _mm256_loadu_pd(pBsrcY+index);

				// a = Y/255
				__m256d aB =_mm256_div_pd(vComp2, vConst);
				// b = 255 - Y
				__m256d bB =_mm256_sub_pd(vConst, vComp2);
				// c = 2*X
				__m256d cB = _mm256_mul_pd(vConst2,vComp1);
				// d = c / 255
				__m256d dB = _mm256_div_pd(cB, vConst);
				// e = b * d
				__m256d eB = _mm256_mul_pd(bB,dB);
				// f = Y + e 
				__m256d fB = _mm256_add_pd(vComp2,eB);
				// resultado = a * f
				__m256d resultadoB = _mm256_mul_pd(aB,fB);
				
				_mm256_storeu_pd ((pBdest+index),resultadoB);

			}
		}
	}
	
	if (clock_gettime(CLOCK_REALTIME, &tEnd)== -1)
	{
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}
	printf("Finished fusion SIMD\n");

	dElapsedTimeS= (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS+= (tEnd.tv_nsec - tStart.tv_nsec)/1e+9;
	printf("Elapsed time fusion SIMD\n: %f s\n", dElapsedTimeS);
	
	CImg<data_t> dstImage(pDstImage, width, height, 1, nComp);


	dstImage.save(DESTINATION_IMG); 


	//dstImage.display();
	
	free(pDstImage);
	
	
	return 0;

}