#include "tczero.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

size_t cbc_enc(uint64_t key[2], uint8_t *pt, uint8_t *ct, size_t ptlen){
	
	return 0;
}

int main(){
	int rData = open("/dev/urandom", O_RDONLY);
	if(rData >= 0)
	{
	    char myRandomData[50];
	    ssize_t result = read(rData, myRandomData, sizeof myRandomData);
	    if (result < 0)
	    {
	        // something went wrong
	    }
	}else{
		printf("Probleme");
	}
	return 0;
}