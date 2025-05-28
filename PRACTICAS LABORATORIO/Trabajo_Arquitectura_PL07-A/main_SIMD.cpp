
#include <stdio.h>
#include <math.h>
#include <CImg.h>
#include <time.h>
#include <fstream>
#include <immintrin.h> // Required to use intrinsic functions


using namespace std;
using namespace cimg_library;

//TIPO DE DATOS PARA LAS COMPONENTES DE LA IMAGEN -> float
typedef float data_t;


#define REPETITIONS 100
#define ITEMS_PER_PACKET (sizeof(__m256)/sizeof(float))


//COMPONENTES PARA CALCULAR EL TIEMPO
struct timespec tStart, tEnd;
double dElapsedTimeS;

//IMAGENES FUENTE Y DESTINO
const char* SOURCE_IMG      = "bailarina.bmp";
const char* DESTINATION_IMG = "bailarina2.bmp";


// Filter argument data type
typedef struct {
	data_t *pRsrc; // Pointers to the R, G and B components
	data_t *pGsrc;
	data_t *pBsrc;
    data_t *pLdst;
	uint pixelCount; // Size of the image in pixels
} filter_args_t;


void filter (filter_args_t args) {

    for (uint i = 0; i < args.pixelCount; i += ITEMS_PER_PACKET) {
        __m256 pR = _mm256_loadu_ps(args.pRsrc + i);
        __m256 pG = _mm256_loadu_ps(args.pGsrc + i);
        __m256 pB = _mm256_loadu_ps(args.pBsrc + i);

        // Realiza las operaciones en vectores
        __m256 weightedR = _mm256_mul_ps(pR, _mm256_set1_ps(0.3f));
        __m256 weightedG = _mm256_mul_ps(pG, _mm256_set1_ps(0.59f));
        __m256 weightedB = _mm256_mul_ps(pB, _mm256_set1_ps(0.11f));
        __m256 sum = _mm256_add_ps(_mm256_add_ps(weightedR, weightedG), weightedB);
        __m256 inverted = _mm256_sub_ps(_mm256_set1_ps(255.0f), sum);

        // Almacenar los resultados
        _mm256_storeu_ps(args.pLdst + i, inverted);
    }
}


int main() {

    //ABRIR EL ARCHIVO FUENTE IMAGEN
	CImg<data_t> srcImage(SOURCE_IMG);

	//CREAR ESTRUCTURA PARA PASAR AL FILTRO
	filter_args_t filter_args;


    //COMPROBACION DE LA EXISTENCIA DE LA IMAGEN FUENTE
	try{
		srcImage.load(SOURCE_IMG);
	}
	catch( std::invalid_argument e){
		printf("No existe la imagen fuente");
                exit(-2);
	}
	

    //MOSTRAR IMAGEN ORIGINAR Y EXTRAER SU INFORMACION
	srcImage.display();
	uint width = srcImage.width();
	uint height = srcImage.height();
	uint nComp = 1;


    //CALCULAR EL TAMANO DE LA IMAGEN EN PIXELES
	filter_args.pixelCount = width * height;


	//PUNTERO A LA IMAGEN DESTINMO
	data_t *pDstImage; 


    //RESERVAR MEMORIA PARA LA IMAGEN DESTINO
	pDstImage = (data_t *) malloc (filter_args.pixelCount * nComp * sizeof(data_t)); //EN IMAGENES B/W nComp=1
	if (pDstImage == NULL) {
		perror("Allocating destination image");
		exit(-2);
	}


    //DAR VALORES A LA ESTRUCTURA
	filter_args.pRsrc = srcImage.data(); // pRcomp points to the R component array
	filter_args.pGsrc = filter_args.pRsrc + filter_args.pixelCount; // pGcomp points to the G component array
	filter_args.pBsrc = filter_args.pGsrc + filter_args.pixelCount; // pBcomp points to B component array


    //PUNTERO AL ARRAY CON LA COMPONENTE DE LA IMAGEN DESTINO
    filter_args.pLdst = pDstImage;
    
    
    //NUMEOR DE PAQUETES NECESARIOS PARA PROCESAR LA IMAGEN
    int nPackets = (filter_args.pixelCount / ITEMS_PER_PACKET);


    //SI NO ES DIVISIBLE HAY QUE AÑADIR UN PAQUETE MAS
    if ( ((filter_args.pixelCount * sizeof(float)) % sizeof(__m256)) != 0) {
		nPackets++;
	}


    //COMIENZA EL TIEMPO
	if (clock_gettime(CLOCK_REALTIME, &tStart) == -1) {
		printf("Error: couldn't obtain starting time print.");
		exit(1);
	}

	//SE APLICA EL FILTRO
    for(int i=0 ; i<REPETITIONS ; i++){
		filter(filter_args);
	}

	//TERMINA EL TIEMPO
    if (clock_gettime(CLOCK_REALTIME, &tEnd) == -1) {
		printf("Error: couldn't obtain final time print.");
		exit(1);
	}


    printf("Terminado\n");


    //IMPRIMIR LOS TIEMPOS
	dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
	printf ("Final execution time: %f\n", dElapsedTimeS);

	CImg<data_t> dstImage(pDstImage, width, height, 1, nComp);

    //GUARDAR LA IMAGEN EN UN ARCHIVO
	dstImage.save(DESTINATION_IMG);   

	//ABRIR LA IMAGEN
	dstImage.display();

	//LIBERAR MEMORIA DE LA IMAGEN DESTION
	free(pDstImage);

	//TERMINAR PROGRAMA SIN ERRORES
	return 0;
	
}
