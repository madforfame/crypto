#include "tczero.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char myRandomData[HALF_BLOCK_SIZE*2];

size_t cbc_enc(uint64_t key[2], uint8_t *pt, uint8_t *ct, size_t ptlen){
	
	return 0;
}

void print_tab_1d(char *c, int s){
	int i;
	for(i=0;i<s;i++){
		printf(" %c", *(c+i));
	}
}

void rand_data(int f){
	ssize_t result = read(f, myRandomData, sizeof myRandomData);
	if (result < 0){
	    printf("Probleme lors de la crea random");
	}else{
		print_tab_1d(myRandomData, HALF_BLOCK_SIZE*2);
	}
}

int main(){
	int rData = open("/dev/urandom", O_RDONLY);

	if(rData >= 0)
	{
	    rand_data(rData);

	}else{
		printf("Probleme");
	}



	close(rData);
	return 0;
}