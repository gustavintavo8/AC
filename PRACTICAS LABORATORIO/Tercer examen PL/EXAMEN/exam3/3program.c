#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <atc/linmem.h>

// Vector size
const uint16_t N = 8043;

int main(void) {
	unsigned int first_page = 1;
	unsigned int last_page = 0;
	unsigned int page = 0;
	unsigned int pte = 0;
	void * virtual_addr;
	uint16_t vector[N];
	
//	unsigned int codePage, dataPage, bssPage;
//	unsigned int codeFrame, dataFrame, bssFrame;
//	unsigned int codeFlags, dataFlags, bssFlags;
	
	memset(vector, 0, sizeof(vector));
	printf("\nVector address: %.8Xh\n", (unsigned int)vector);

	// TODO: compute first and last page numbers
	// last_page = 
	// first_page = 

	// Print page and frame numbers
	printf("Vector pages: ");
	for (page = first_page; page <= last_page; page++) {
		// Print page number
		printf("%.5Xh ", page);

		// TODO: virtual address within the current page (offset 0)
		// virtual_addr = 

		// Get PTE of the current page
		if (get_pte(virtual_addr, &pte)) {
			perror("Linmem module error");
			return -1;
		}

		// Check page is present in memory
		if (pte & 0x1) {
			// Print frame number
			printf("(%.5Xh) ", pte >> 12);
		}
		else {
			printf("(no page) ");
		}
	}

	// Print PTE flags for variable N
	if (get_pte((void *)&N, &pte)) {
		perror("Linmem module error");
		return -1;
	}
//	codeFrame = codePage & 0xFFFFF000;
//	codeFlags = codePage & 0x00000FFF;
//	dataFrame = dataPage & 0xFFFFF000;
//	dataFlags = dataPage & 0x00000FFF;
//	bssFrame = bssPage & 0xFFFFF000;
//	bssFlags = bssPage & 0x00000FFF;
//	printf("\nKernel code page %05Xh ---> Frame: %05Xh; Flags: %03Xh\n", codePage, codeFrame, codeFlags);
//	printf("Kernel data page %05Xh ---> Frame: %05Xh; Flags: %03Xh\n", dataPage, dataFrame, dataFlags);	
//	printf("Kernel bss  page %05Xh ---> Frame: %05Xh; Flags: %03Xh\n\n", bssPage, bssFrame, bssFlags);	
	printf("\nFlags of N page: %.3Xh\n\n", pte & 0xFFF);

	return 0;
}
