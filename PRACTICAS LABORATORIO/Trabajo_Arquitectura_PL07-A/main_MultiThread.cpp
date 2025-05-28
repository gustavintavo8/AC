#include <stdio.h>
#include <math.h>
#include <CImg.h>
#include <iostream>
#include <fstream>
#include <pthread.h>

// Incluyendo las bibliotecas necesarias
using namespace std;
using namespace cimg_library;


// Constantes y variables globales
const unsigned short NUM_THREADS = 4;
const unsigned int n = 50;
int hilo = 0;


// Definici�n de tipos y nombres de archivos
typedef float data_t;
const char* SOURCE_IMG = "bailarina.bmp";
const char* DESTINATION_IMG = "bailarina2.bmp";


// Estructura para los argumentos del filtro
typedef struct {
    data_t *pRsrc;
    data_t *pGsrc;
    data_t *pBsrc;
    data_t *pLdst;
    uint pixelCount;
    uint Max_hilo;
} filter_args_t;


// Funci�n del filtro aplicado a la imagen
void* filter(void *args_ptr) {

    filter_args_t *args = (filter_args_t*)args_ptr;
    int n_hilo = hilo++;

    // Funci�n que ejecuta el filtro en hilos
    for (uint i = n_hilo * args->Max_hilo; i < (n_hilo + 1) * args->Max_hilo; i++) {
        float pR = (float)*(args->pRsrc + i) * 0.3;
        float pG = (float)*(args->pGsrc + i) * 0.59;
        float pB = (float)*(args->pBsrc + i) * 0.11;
        *(args->pLdst + i) = 255 - (pR + pG + pB);
    }
    return NULL;
}


// FUNCION MAIN
int main() {

    // Verificar la existencia de la imagen origen
    ifstream f(SOURCE_IMG);
    if (!f.good()) {
        printf("\nLa imagen origen no existe\n");
        exit(EXIT_FAILURE);
    }


    // Lectura de la imagen origen y obtenci�n de informaci�n
    CImg<data_t> srcImage(SOURCE_IMG);
    srcImage.display();
    uint width = srcImage.width();
    uint height = srcImage.height();
    uint nComp = 1;


    // Inicializaci�n de variables para hilos y filtro
    pthread_t Threads[NUM_THREADS];
    filter_args_t filter_args;
    filter_args.pixelCount = width * height;


    // Creaci�n del espacio para la imagen destino
    data_t *pDstImage = new data_t[filter_args.pixelCount * nComp];
    if (pDstImage == NULL) {
        perror("Allocating destination image");
        exit(-2);
    }


    // Asignaci�n de punteros a los componentes de la imagen origen
    filter_args.pRsrc = srcImage.data();
    filter_args.pGsrc = filter_args.pRsrc + filter_args.pixelCount;
    filter_args.pBsrc = filter_args.pGsrc + filter_args.pixelCount;


    // Definici�n de tama�os y punteros
    filter_args.Max_hilo = (filter_args.pixelCount+ NUM_THREADS -1)/ NUM_THREADS;
    filter_args.pLdst = pDstImage;
   

    // Inicializaci�n del contador de tiempo
    struct timespec tStart, tEnd;
    double dElapsedTimeS;

    if (clock_gettime(CLOCK_REALTIME, &tStart) == -1) {
        printf("ERROR: clock_gettime: %d.\n", 0);
        exit(EXIT_FAILURE);
    }

    //BUCLE CON EL MISMO NUMERO DE REPETICIONES QUE SINGLE-THREAD
    for (int i = 0; i < 100; i++){ 
        // CREACION DE LOS HILOS
        for (int i = 0; i < NUM_THREADS; i++) {
            int pthread_ret = pthread_create(&Threads[i], NULL, filter, (void*)&filter_args);
            if (pthread_ret) {
                printf("ERROR: pthread_create error code: %d.\n", pthread_ret);
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(Threads[i], NULL);
        }
    }//FIN DEL BUCLE

    // Finalizaci�n de mediciones de tiempo
    if (clock_gettime(CLOCK_REALTIME, &tEnd) == -1) {
        printf("ERROR: clock_gettime: %d.\n", 0);
        exit(EXIT_FAILURE);
    }

    printf("Terminado\n");

    // C�lculo del tiempo de ejecuci�n
    dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
    dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
    printf("Tiempo de ejecucion: %f s.\n", dElapsedTimeS);
    

    // Creaci�n de la imagen destino y mostrarla
    CImg<data_t> dstImage(pDstImage, width, height, 1, nComp);
    dstImage.save(DESTINATION_IMG);
    dstImage.display();


    // Liberaci�n de memoria
    delete[] pDstImage;


    // COMPROBACION DEL TAMA�O DE IMAGEN FUENTE Y DESTINO
    if ((dstImage.width() * dstImage.height()) == (srcImage.width() * srcImage.height())) {
        printf("\nLas im�genes tienen el mismo tama�o\n");
    } else {
        printf("\nLas im�genes no tienen el mismo tama�o\n");
        exit(EXIT_FAILURE);
    }

    // Verificaci�n del tama�o de la imagen destino, al ser multihilo pueden no coincidir los tama�os
	//debido a que si el n� de pixeles no es divisible entero entre NUM_THREADS, el ultimo hilo procesa menos pixeles
	

    return 0;
}