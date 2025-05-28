#include <stdio.h>
#include <stdlib.h>

#define PRIME_NUM 100000   //Prime numbers to be generated
#define BOOLEAN int
#define TRUE 1
#define FALSE 0

const unsigned int nTimes = 3;
const unsigned int nSwaps = 10000;

void primeGenerator(int v1[], int v2[]);
BOOLEAN checkPrime_fast (int number);
void swap(int v1[], int v2[]);
int getRandom();

int main()
{
  int v1[PRIME_NUM/2];
  int v2[PRIME_NUM/2];

  primeGenerator(v1, v2);

  int v3[PRIME_NUM/2];
  srand(1);
  for (int i = 0; i<nTimes; i++){
    for (int j = 0; j<PRIME_NUM/2; j++){
      v3[j] = getRandom();
    }
  }

  for (int i = 0; i<nSwaps; i++){
    swap(v1,v2);
  }

  //Print v1 v2
  /*
  for (int i = 0; i<PRIME_NUM/2; i++) printf("%i\t", v1[i]);
  printf("\n");
  for (int i = 0; i<PRIME_NUM/2; i++) printf("%i\t", v2[i]);
  printf("\n");
  for (int i = 0; i<PRIME_NUM/2; i++) printf("%i\t", v3[i]);
  printf("\n");
  */

  return 0;
}

void primeGenerator(int v1[], int v2[]){
  v1[0] = 1;
  v2[0] = 2;
  int count = 2; 
  BOOLEAN isPrime;
  int i = 0;

  for (i = 3; count != PRIME_NUM; i++){
    isPrime = checkPrime_fast(i); //Fastest algorithm
    
    if (isPrime) {
      if (count % 2 == 0)
        v1[count/2] = i;
      else
        v2[count/2] = i;

      count++;
    }
  }

}

BOOLEAN checkPrime_fast(int number) {
  BOOLEAN isPrime = TRUE;
  for (int j = 2; j*j<=number; j++){
    if (number % j == 0) {
      return FALSE;
    }
  }
  
  return TRUE;
}

void swap(int v1[], int v2[]){
  int temp;
  for (int i = 0; i<PRIME_NUM/2; i++){
    temp = v1[i];
    v1[i] = v2[i];
    v2[i] = temp;
  }
}

int getRandom(){  //Random int in [1,90]
  return (rand() % 90 + 1);
}

