#include <stdio.h>
#include <string.h>

int main()
{
	int  aux1 = 1;
	int *aux2 = NULL;
	
	aux2 = aux2 + 2; // Increments the pointed address

	__asm__(
		"iret"
	);

}

