/*
 * Main.cpp
 *
 *  Created on: Fall 2019
 */

#include <stdio.h>
#include <immintrin.h> // Required to use intrinsic functions
#include <math.h>
#include <CImg.h>
#include <time.h>
#include <errno.h>

using namespace cimg_library;
using namespace std;

const char* SOURCE_IMG1      = "bailarina.bmp";
const char* SOURCE_IMG2      = "background_V.bmp";
const char* DESTINATION_IMG  = "resultado.bmp";
const  uint REPETICIONES     = 30;

typedef double data_t;

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


/*
 for (uint i = 0; i < args.pixelCountX; i++) {
	*(args.pRdst + i) = 255 - (( (255 - *(args.pRsrc + i)) * (255 - *(args.pRsrc2 + i)))/255);
	*(args.pGdst + i) = 255 - (( (255 - *(args.pGsrc + i)) * (255 - *(args.pGsrc2 + i)))/255);
	*(args.pBdst + i) = 255 - (( (255 - *(args.pBsrc + i)) * (255 - *(args.pBsrc2 + i)))/255);}
*/

int main(){

	CImg<data_t> srcImageX(SOURCE_IMG1); // ABRIR ARCHIVO Y OBJETO DE INICIALIZACION
	CImg<data_t> srcImageY(SOURCE_IMG2);

	filter_args_t filter_args;
	data_t *pDstImage; // Pointer to the new image pixels

	struct timespec tStart, tEnd;
	double dElapsedTimeS;

    //Imagen X
    srcImageX.display(); // Displays the source image
	uint widthX = srcImageX.width();// Getting information from the source image
	uint heightX = srcImageX.height();	
	uint nCompX = srcImageX.spectrum();
	filter_args.pixelCountX = widthX * heightX;

    //Imagen Y
	srcImageY.display(); // Displays the source image
	uint widthY = srcImageX.width();// Getting information from the source image
	uint heightY = srcImageX.height();	
	uint nCompY = srcImageX.spectrum();
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
	if (clock_gettime(CLOCK_REALTIME, &tStart)==-1) {
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}

    // El numero de pixeles debe ser multiplo de 32 usando extensiones SIMD de 256 bits
	// (cualquier  acceso  a  operandos  de  tamano  256  bits  debe  realizarse  empleando  direcciones
	// de memoria multiplos de 32: para almacenar 256 bits hacen falta 32 bytes, es decir, 32 posiciones
	// de memoria).

    uint pixelCountX = filter_args.pixelCountX;
    uint pixelCountY = filter_args.pixelCountY;

	if ((pixelCountX % 32 != 0) || (pixelCountY % 32 != 0)) {
		printf("\nEl numero de pixeles no es multiplo de 32\n");
		exit(-3);
	}

	/************************************************
	 * Algorithm.
	 */
	if (widthX == widthY && heightX == heightY && nCompX == nCompY){
	    printf("Ejecutamos la fusión %i veces.\n",REPETICIONES);

		for( uint i = 0; i < REPETICIONES; i++){

            /* for (uint i = 0; i < args.pixelCountX; i++) {
	        *(args.pRdst + i) = 255 - (( (255 - *(args.pRsrc + i)) * (255 - *(args.pRsrc2 + i)))/255);
	        *(args.pGdst + i) = 255 - (( (255 - *(args.pGsrc + i)) * (255 - *(args.pGsrc2 + i)))/255);
	        *(args.pBdst + i) = 255 - (( (255 - *(args.pBsrc + i)) * (255 - *(args.pBsrc2 + i)))/255);}*/     
            
            __m256 rsPacket, gsPacket, bsPacket; // Paquetes float para imagenes RGB (origen) 
            __m256 rsPacket2, gsPacket2, bsPacket2; // Paquetes float para imagenes RGB (origen)
            __m256 rdPacket, gdPacket, bdPacket; // Paquetes float para imagenes RGB (destino)
			__m256d rPacket, gPacket, bPacket; // Paquetes double para imagenes RGB
			__m256d rPacket2, gPacket2, bPacket2; // Paquetes int para imagenes RGB

            //Operamos en float para una mayor precisión entre operaciones
            //256/32 = 8 pixeles por paquete

            for (uint j = 0; j < pixelCountX; j+=8) {

				//Se leen 32 componentes de cada imagen (8 pixeles) y se almacenan en los paquetes
                // _mm256_load_pd AVX
				rPacket = _mm256_load_pd(filter_args.pRsrc + j);
				gPacket = _mm256_load_pd(filter_args.pGsrc + j);
				bPacket = _mm256_load_pd(filter_args.pBsrc + j);

				rPacket2 = _mm256_load_pd(filter_args.pRsrc2 + j);
				gPacket2 = _mm256_load_pd(filter_args.pGsrc2 + j);
				bPacket2 = _mm256_load_pd(filter_args.pBsrc2 + j);

				//Convierte el paquete de 8 pixeles de 32 bits a 8 pixeles de 8 bits
				// _mm256_cvtps_epi32 AVX
				rsPacket = _mm256_cvtepi32_ps(_mm256_castpd_si256(rPacket));
				gsPacket = _mm256_cvtepi32_ps(_mm256_castpd_si256(gPacket));
				bsPacket = _mm256_cvtepi32_ps(_mm256_castpd_si256(bPacket));

				rsPacket2 = _mm256_cvtepi32_ps(_mm256_castpd_si256(rPacket2));
				gsPacket2 = _mm256_cvtepi32_ps(_mm256_castpd_si256(gPacket2));
				bsPacket2 = _mm256_cvtepi32_ps(_mm256_castpd_si256(bPacket2));

				//Se realiza la operación de fusión de capas
				// _mm256_sub_ps AVX
				rdPacket = _mm256_sub_ps(_mm256_set1_ps(255.0), _mm256_div_ps(_mm256_mul_ps(_mm256_sub_ps(_mm256_set1_ps(255.0), rsPacket), _mm256_sub_ps(_mm256_set1_ps(255.0), rsPacket2)), _mm256_set1_ps(255.0)));
				gdPacket = _mm256_sub_ps(_mm256_set1_ps(255.0), _mm256_div_ps(_mm256_mul_ps(_mm256_sub_ps(_mm256_set1_ps(255.0), gsPacket), _mm256_sub_ps(_mm256_set1_ps(255.0), gsPacket2)), _mm256_set1_ps(255.0)));
				bdPacket = _mm256_sub_ps(_mm256_set1_ps(255.0), _mm256_div_ps(_mm256_mul_ps(_mm256_sub_ps(_mm256_set1_ps(255.0), bsPacket), _mm256_sub_ps(_mm256_set1_ps(255.0), bsPacket2)), _mm256_set1_ps(255.0)));
				
				//Convierte el paquete de 8 pixeles de 8 bits a 8 pixeles de 32 bits
				// _mm256_cvtps_epi32 AVX
				rPacket = _mm256_castps_pd(rdPacket);
				gPacket = _mm256_castps_pd(gdPacket);
				bPacket = _mm256_castps_pd(bdPacket);

				//Se almacenan los 8 pixeles en la imagen destino
				// _mm256_store_pd AVX (_mm256_castps_si256)
				_mm256_store_pd(filter_args.pRdst + j, rPacket);
				_mm256_store_pd(filter_args.pGdst + j, gPacket);
				_mm256_store_pd(filter_args.pBdst + j, bPacket);
            }
        }
	}

	else {
        printf ("Error, las dimensiones de las imagenes no son iguales.\n");
		exit (EXIT_FAILURE);
	}


	/***********************************************
	 * TODO: End of the algorithm.
	 *   - Measure the end time
	 *   - Calculate the elapsed time
	 */
	if (clock_gettime(CLOCK_REALTIME, &tEnd)== -1){
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