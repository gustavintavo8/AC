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


// Definición de tipos y nombres de archivos
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


// Función del filtro aplicado a la imagen
void* filter(void *args_ptr) {

    filter_args_t *args = (filter_args_t*)args_ptr;
    int n_hilo = hilo++;

    // Función que ejecuta el filtro en hilos
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


    // Lectura de la imagen origen y obtención de información
    CImg<data_t> srcImage(SOURCE_IMG);
    srcImage.display();
    uint width = srcImage.width();
    uint height = srcImage.height();
    uint nComp = 1;


    // Inicialización de variables para hilos y filtro
    pthread_t Threads[NUM_THREADS];
    filter_args_t filter_args;
    filter_args.pixelCount = width * height;


    // Creación del espacio para la imagen destino
    data_t *pDstImage = new data_t[filter_args.pixelCount * nComp];
    if (pDstImage == NULL) {
        perror("Allocating destination image");
        exit(-2);
    }


    // Asignación de punteros a los componentes de la imagen origen
    filter_args.pRsrc = srcImage.data();
    filter_args.pGsrc = filter_args.pRsrc + filter_args.pixelCount;
    filter_args.pBsrc = filter_args.pGsrc + filter_args.pixelCount;


    // Definición de tamaños y punteros
    filter_args.Max_hilo = (filter_args.pixelCount+ NUM_THREADS -1)/ NUM_THREADS;
    filter_args.pLdst = pDstImage;
   

    // Inicialización del contador de tiempo
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

    // Finalización de mediciones de tiempo
    if (clock_gettime(CLOCK_REALTIME, &tEnd) == -1) {
        printf("ERROR: clock_gettime: %d.\n", 0);
        exit(EXIT_FAILURE);
    }

    printf("Terminado\n");

    // Cálculo del tiempo de ejecución
    dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
    dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
    printf("Tiempo de ejecucion: %f s.\n", dElapsedTimeS);
    

    // Creación de la imagen destino y mostrarla
    CImg<data_t> dstImage(pDstImage, width, height, 1, nComp);
    dstImage.save(DESTINATION_IMG);
    dstImage.display();


    // Liberación de memoria
    delete[] pDstImage;


    // COMPROBACION DEL TAMAÑO DE IMAGEN FUENTE Y DESTINO
    if ((dstImage.width() * dstImage.height()) == (srcImage.width() * srcImage.height())) {
        printf("\nLas imágenes tienen el mismo tamaño\n");
    } else {
        printf("\nLas imágenes no tienen el mismo tamaño\n");
        exit(EXIT_FAILURE);
    }

    // Verificación del tamaño de la imagen destino, al ser multihilo pueden no coincidir los tamaños
	//debido a que si el nº de pixeles no es divisible entero entre NUM_THREADS, el ultimo hilo procesa menos pixeles
	

    return 0;
}