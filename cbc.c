#include "tczero.h"
#include "speck32.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>

uint64_t myRandomData;

size_t cbc_enc(uint16_t key[4], uint8_t *pt, uint8_t *ct, size_t ptlen){
	uint64_t iv=myRandomData;
	uint8_t plainT[4];
	uint8_t cryptT[4];

	//Encrypt first block
	for (int i = 0 ; i < 4 ; i++) {
		plainT[i] = pt[i];
	}
	for (int i = 0 ; i < 4 ; i++) {
		plainT[i] = plainT[i] ^ (iv >> i*8);
	}

	speck32_64_encrypt(key, plainT, cryptT);
	//Encrypt plain text
	for (int i = 0 ; i < 4 ; i++) {
		ct[i] = cryptT[i];
	}
	for (int i = 1 ; i < ptlen/4 ; i++) {
		for (int j = 0 ; j < 4 ; j++) {
			plainT[j] = pt[i*4+j];
			plainT[j] = plainT[j] ^ cryptT[j];
		}
		speck32_64_encrypt(key, plainT, cryptT);
		for (int k = 0 ; k < 4 ; k++) {
			ct[i*4+k] = cryptT[k];
		}
	}

	printf("PlainText : %" PRIu64, *((uint64_t *) pt));
	printf("\n");
	printf("EncryptedText : %" PRIu64, *((uint64_t *) ct));
	printf("\n");
	return ptlen;
}

size_t cbc_dec(uint16_t key[4], uint8_t *ct, uint8_t *pt, size_t ctlen) {
	uint64_t iv=myRandomData;
	uint8_t plainT[4];
	uint8_t cryptT[4];

	//Decrypt first block
	for (int i = 0 ; i < 4 ; i++) {
		cryptT[i] = ct[i];
	}
	speck32_64_decrypt(key, cryptT, plainT);
	for (int i = 0 ; i < 4 ; i++) {
		pt[i] = plainT[i] ^ (iv >> i*8);
	}

	//Decrypt crypted message
	for (int i = 1 ; i < ctlen ; i++) {
		for (int j = 0 ; j < 4 ; j++) {
			cryptT[j] = ct[i*4+j];
		}
		speck32_64_decrypt(key, cryptT, plainT);
		for (int j = 0 ; j < 4 ; j++) {
			pt[i*4+j] = plainT[j] ^ ct[(i-1)*4+j];
		}
	}

	printf("PlainText Decrypted : %" PRIu64, *((uint64_t *) pt));
	printf("\n");
	return ctlen;
}

void rand_data(int f){
	ssize_t result = read(f, &myRandomData, sizeof myRandomData);
	if (result < 0){
	    printf("Random error");
	}
}

int main(){
	int rData = open("/dev/urandom", O_RDONLY);
	uint64_t key[2];
	uint64_t pt[2];
	uint64_t ptdec[2];
	uint64_t ct[2];

	if(rData >= 0)
	{
	    rand_data(rData);
	    key[0]=myRandomData;

	    rand_data(rData);
	    key[1]=myRandomData;

	    rand_data(rData);
	    pt[0] = myRandomData;
	    rand_data(rData);
	    pt[1] = myRandomData;

	    printf("%" PRIu64 "\n", key[0]);
	    printf("%" PRIu64 "\n", key[1]);
	    

	    rand_data(rData);

	    size_t ptlen=HALF_BLOCK_SIZE*2/8;
	    size_t ctlen = HALF_BLOCK_SIZE*2/8;
	    
	    cbc_enc((uint16_t *)key, (uint8_t *)pt, (uint8_t *)ct, ptlen);
	    cbc_dec((uint16_t *)key, (uint8_t *)ct, (uint8_t *)ptdec, ctlen);


	}else{
		printf("Error");
	}



	close(rData);
	return 0;
}