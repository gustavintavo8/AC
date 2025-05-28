/*
 * Main.cpp
 *
 *  Created on: 11 dec. 2018
 *  	Author: pl5_c
 *
 *  128 bit SIMD extensions
 *  Se usan funciones intrinsecas de SSE, SSE2 y SSE3
 *  Requires AVX, so compile with -mavx option in g++
 */

#include <CImg.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <immintrin.h>
#include <emmintrin.h>

using namespace cimg_library;
using namespace std;

// Mismo numero de repeticiones que en singlethread
#define NTIMES 10

int main() {

	CImg<int> srcImage("bailarina.bmp"); // Open file and object initialization

	int *pRcomp, *pGcomp, *pBcomp; // Punteros a los componentes R, G, B
	int *pRnew, *pGnew, *pBnew;
	int *pdstImage; // Puntero a la imagen destino
	int width, height;
	int nComp; // Numero de componentes de la imagen
	float C, T;
	struct timespec tStart, tEnd;
	double dTimeS;

	T = 5.0;
	if (T < -100){	T = -100;
	}else if (T > 100){	T = 100;}
	C = pow((100 + T)/100, 2);


	srcImage.display(); // Muestra la imagen inicial
	width = srcImage.width();
	height  = srcImage.height();
	nComp = srcImage.spectrum();
				// Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)

	// Asigna espacio en memoria para los pixeles de la imagen destino (procesada)
	pdstImage = (int *) malloc (width * height * nComp * sizeof(int));
	if (pdstImage == NULL) {
		printf("\nMemory allocating error\n");
		exit(-2);
	}

	// Punteros a los arrays RGB de la imagen original
	pRcomp = srcImage.data(); // pRcomp apunta al componente R
	pGcomp = pRcomp + height * width; // pGcomp apunta al componente G
	pBcomp = pGcomp + height * width; // pBcomp apunta al componente B

	// Punteros a los arrays RGB de la imagen destino
	pRnew = pdstImage;
	pGnew= pRnew + height * width;
	pBnew= pGnew + height * width;

	// Medida de tiempo inicial
	if (clock_gettime(CLOCK_REALTIME, &tStart) < 0){
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}
	// El numero de pixels debe ser multiplo de 16 usando extensiones SIMD de 128 bits
	// (cualquier  acceso  a  operandos  de  tamaño  128  bits  debe  realizarse  empleando  direcciones
	// de memoria multiplos de 16: para almacenar 128 bits hacen falta 16 bytes, es decir, 16 posiciones
	// de memoria).
	if (width * height % 16 != 0){
		printf("\nThe number of pixels %d is not multiple of 16\n", width * height);
		exit(-3);
	}

	// Repeat the processing NTIMES
	for (int i = 0; i < NTIMES; i++){
		int npixels = width * height; // Numero de pixels de la imagen

		__m128 rsPacket, gsPacket, bsPacket; // Paquetes float para componentes R, G, B (origen)
		__m128 rdPacket, gdPacket, bdPacket; // Paquetes float para componentes R, G, B (destino)
		__m128i tmp128iPacket1, tmp128iPacket2, tmp128iPacket3;
		// Operamos en float para mantener la coherencia entre operaciones.
		//  128/32 = 4 pixels por paquete
		for (int j = 0; j < npixels; j += 4){
			// Se leen 16 componentes de memoria (16 x 8 bits = 128 bits)
			// _mm_lddqu_si128 SSE3
			tmp128iPacket1 = _mm_lddqu_si128((__m128i*) pRcomp);
			tmp128iPacket2 = _mm_lddqu_si128((__m128i*) pGcomp);
			tmp128iPacket3 = _mm_lddqu_si128((__m128i*) pBcomp);

			// Convierte el paquete de 4 its en un paquete de 4 floats para operar
			// _mm_cvtepi32_ps SSE2
			rsPacket = _mm_cvtepi32_ps(tmp128iPacket1);
			gsPacket = _mm_cvtepi32_ps(tmp128iPacket2);
			bsPacket = _mm_cvtepi32_ps(tmp128iPacket3);

			// Algoritmo de contraste
			// _mm_mul_ps multiplica arg1 * C SSE
			// _mm_set1_ps SSE
			rdPacket = _mm_mul_ps(rsPacket, _mm_set1_ps(C));
			gdPacket = _mm_mul_ps(gsPacket, _mm_set1_ps(C));
			bdPacket = _mm_mul_ps(bsPacket, _mm_set1_ps(C));


			// Saturacion de resultados
			// _mm_min_ps compara arg1 y arg2 y almacena el valor minimo SSE
			// _mm_max_ps compara arg1 y arg2 y almacena el valor maximo SSE
			rdPacket = _mm_min_ps(rdPacket, _mm_set1_ps(255.0));
			rdPacket = _mm_max_ps(rdPacket, _mm_set1_ps(0.0));
			gdPacket = _mm_min_ps(gdPacket, _mm_set1_ps(255.0));
			gdPacket = _mm_max_ps(gdPacket, _mm_set1_ps(0.0));
			bdPacket = _mm_min_ps(bdPacket, _mm_set1_ps(255.0));
			bdPacket = _mm_max_ps(bdPacket, _mm_set1_ps(0.0));

			//Algoritmo sepia
			// _mm_add_ps suma 32bit float arg1 y arg2 SSE
			rdPacket = _mm_add_ps(_mm_mul_ps(rdPacket, _mm_set1_ps(0.393)), _mm_add_ps(_mm_mul_ps(gdPacket, _mm_set1_ps(0.769)), _mm_mul_ps(bdPacket, _mm_set1_ps(0.189))));
			gdPacket = _mm_add_ps(_mm_mul_ps(rdPacket, _mm_set1_ps(0.349)), _mm_add_ps(_mm_mul_ps(gdPacket, _mm_set1_ps(0.686)), _mm_mul_ps(bdPacket, _mm_set1_ps(0.168))));
			bdPacket = _mm_add_ps(_mm_mul_ps(rdPacket, _mm_set1_ps(0.272)), _mm_add_ps(_mm_mul_ps(gdPacket, _mm_set1_ps(0.534)), _mm_mul_ps(bdPacket, _mm_set1_ps(0.131))));

			// Saturacion de resultados
			// _mm_min_ps compara arg1 y arg2 y almacena el valor minimo SSE
			// _mm_max_ps compara arg1 y arg2 y almacena el valor maximo SSE
			rdPacket = _mm_min_ps(rdPacket, _mm_set1_ps(255.0));
			rdPacket = _mm_max_ps(rdPacket, _mm_set1_ps(0.0));
			gdPacket = _mm_min_ps(gdPacket, _mm_set1_ps(255.0));
			gdPacket = _mm_max_ps(gdPacket, _mm_set1_ps(0.0));
			bdPacket = _mm_min_ps(bdPacket, _mm_set1_ps(255.0));
			bdPacket = _mm_max_ps(bdPacket, _mm_set1_ps(0.0));

			// Convierte los paquetes float de vuelta a paquetes int
			// _mm_cvtps_epi32 SSE2
			tmp128iPacket1 =_mm_cvtps_epi32(rdPacket);
			tmp128iPacket2 =_mm_cvtps_epi32(gdPacket);
			tmp128iPacket3 =_mm_cvtps_epi32(bdPacket);

			//Punteros para acceder a los paquetes RGB como int
			int *pR = (int *)&tmp128iPacket1;
			int *pG = (int *)&tmp128iPacket2;
			int *pB = (int *)&tmp128iPacket3;

			// Se guardan los resultados obtenidos en los arrays de destino
			for (int k = 0; k < 4; k++){
				*pRnew = (int)*pR;
				*pGnew = (int)*pG;
				*pBnew = (int)*pB;

				//*pRnew = &tmp128iPacket1;
				//*pGnew = &tmp128iPacket2;
				//*pBnew = &tmp128iPacket3;

				pRnew++; pGnew++; pBnew++;
				pR++; pG++; pB++;
			}
			pRcomp+=4; pGcomp+=4; pBcomp+=4;
		}

		// Reiniciamos los punteros para la siguiente iteracion
		pRcomp -= npixels; pGcomp -= npixels; pBcomp -= npixels;
		pRnew -= npixels; pGnew -= npixels; pBnew -= npixels;
	}

	// Medida de tiempo final
	if (clock_gettime(CLOCK_REALTIME, &tEnd) < 0){
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}

	// Impresion por consola de la medida de tiempo
	dTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
	printf("Total time (s): %f s.\n", dTimeS);

	// Nueva imagen con los pixels calculados
	CImg<int> dstImage(pdstImage, width, height, 1, nComp);

	// Se guarda la nueva imagen en disco
	dstImage.save("bailarina2-128.bmp");

	// Se muestra la nueva imagen
	dstImage.display();
	return(0);
}


