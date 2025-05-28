/*
 *  Archivo Main.cpp del Trabajo SingleThread-2023
 *
 *  Created on: Fall 2023
 *
 *  Grupo: PL07-A
 *	   Problema #3 -> Black and white inversion
 *	   Tipo de datos -> float
 *	   Paquete para la parte SIMD -> _256
 */

#include <stdio.h>
#include <math.h>
#include <CImg.h>
#include <iostream>

using namespace cimg_library;

//TIPO DE DATOS PARA LAS COMPONENTES DE LA IMAGEN -> float
typedef float data_t;

const char* SOURCE_IMG      = "bailarina.bmp";
const char* DESTINATION_IMG = "bailarina2.bmp";

// Filter argument data type
typedef struct {
	data_t *pRsrc; // Pointers to the R, G and B components
	data_t *pGsrc;
	data_t *pBsrc;
	data_t *pLdst; //ES EL PIXEL DESTINO CON TODAS LAS COMPONENTES DE COLOR
	uint pixelCount; // Size of the image in pixels
} filter_args_t;

/************************************
 ***   FILTRO DEL ALGORTIMO	*****
 ************************************/
void filter (filter_args_t args) {
    	for (uint i = 0; i < args.pixelCount; i++) {
		float pR = (float) *(args.pRsrc+i)*0.3;
		float pG = (float) *(args.pGsrc+i)*0.59;
		float pB = (float) *(args.pBsrc+i)*0.11;
        	*(args.pLdst + i) = 255-(pR+pG+pB);
    	}
}

int main() {
	//ABRIR EL ARCHIVO FUENTE IMAGEN
	CImg<data_t> srcImage(SOURCE_IMG);

	filter_args_t filter_args;
	data_t *pDstImage; // Pointer to the new image pixels

	//COMPROBACION DE LA EXISTENCIA DE LAS IMAGENES FUENTE
	try{ 
		srcImage.load(SOURCE_IMG);
	}
	catch( std::invalid_argument e){
		printf("No existe la imagen fuente");
                exit(-2);
	}

	//INICIALIZACION DE VARIABLES PARA MEDICION DE TIEMPOS
	struct timespec tStart, tEnd;
	double dElapsedTimeS;

	//MOSTRAR IMAGEN ORIGINAR Y EXTRAER SU INFORMACION
	srcImage.display();
	uint width = srcImage.width();
	uint height = srcImage.height();
	uint nComp = 1; //PQ ES B/W, SI NO SERIA: srcImage.spectrum();


	//CALCULAR EL TAMANO DE LA IMAGEN EN PIXELES
	filter_args.pixelCount = width * height;

	//Allocate memory space for destination image components
	pDstImage = (data_t *) malloc (filter_args.pixelCount * nComp * sizeof(data_t)); //EN IMAGENES B/W nComp=1
	if (pDstImage == NULL) {
		perror("Allocating destination image");
		exit(-2);
	}

	// Pointers to the componet arrays of the source image
	filter_args.pRsrc = srcImage.data(); // pRcomp points to the R component array
	filter_args.pGsrc = filter_args.pRsrc + filter_args.pixelCount; // pGcomp points to the G component array
	filter_args.pBsrc = filter_args.pGsrc + filter_args.pixelCount; // pBcomp points to B component array

	//PUNTERO AL ARRAY CON LA COMPONENTE DE LA IMAGEN DESTINO
    	filter_args.pLdst = pDstImage;


    	//INICICALIZACION DEL CONTADOR DE TIEMPO
	if(clock_gettime(CLOCK_REALTIME, &tStart) == -1){
		printf("ERROR: clock_gettime: %d.\n", 0);
		exit(EXIT_FAILURE);
	}

	//LLAMADA PARA LA APLICACION DEL FILTRO (BUCLE PARA TARDAR 5-10 SEGUNDOS)
	for(int i=0 ; i<100 ; i++){
		filter(filter_args);
	}

    	//FINALIZACION DEL CONTADOR DE TIEMPO
	if(clock_gettime(CLOCK_REALTIME, &tEnd) == -1){
		printf("ERROR: clock_gettime: %d.\n", 0);
		exit(EXIT_FAILURE);
	}
	printf("Terminado\n");


    	//MUESTRA DEL TIEMPO DE EJECUCION
	dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
	printf("Tiempo de ejecucion:      :  %f s.\n", dElapsedTimeS);



	// Create a new image object with the calculated pixels
	// In case of normal color images use nComp=3,
	// In case of B/W images use nComp=1.
	CImg<data_t> dstImage(pDstImage, width, height, 1, nComp);

	// Store destination image in disk
	dstImage.save(DESTINATION_IMG);

	// Display destination image
	dstImage.display();

	// Free memory
	free(pDstImage);

	return 0;
}
