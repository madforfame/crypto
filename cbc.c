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
uint32_t birthday_array[32];
uint16_t arraySize;
uint64_t iv;

size_t cbc_enc(uint16_t key[4], uint8_t *pt, uint8_t *ct, size_t ptlen){
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

uint64_t attack(uint8_t *ct, size_t ctlen) {
	uint8_t cryptT[4];
	uint32_t xored;
	for (int i = 0 ; i < ctlen ; i++) {
		for (int j = 0 ; j < 4 ; j++) {
			cryptT[j] = ct[i*4+j];
		}
		for (int j = 0 ; j < arraySize ; j++) {
			for (int k = j + 1 ; k < arraySize ; k++) {
				if (birthday_array[j] == birthday_array[k]) {
					printf("Collision found\n");
					xored = birthday_array[j-1] ^ birthday_array[k-1];
					printf("XOR : %" PRIu32, xored);
					printf("\n");
					return 1;
				}
			}	
		}
		birthday_array[arraySize] = *(uint32_t *) cryptT;
		arraySize += 1;
	}
	return 0;
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
	arraySize = 0;
	int nbAttacks = 0;

	if(rData >= 0)
	{
		//Generate random key
	    rand_data(rData);
	    key[0]=myRandomData;

	    rand_data(rData);
	    key[1]=myRandomData;

	    //Generate random plain text
	    rand_data(rData);
	    pt[0] = myRandomData;
	    rand_data(rData);
	    pt[1] = myRandomData;    

	    size_t ptlen=HALF_BLOCK_SIZE*2/8;
	    size_t ctlen = HALF_BLOCK_SIZE*2/8;
	    
	    //Encryption
	    //Test for non-deterministic encryption
	    printf("Test for non-deterministic encryption\n");
	    //IV Generation
	    printf("First encryption :\n");
	    rand_data(rData);
	    iv = myRandomData;
	    cbc_enc((uint16_t *)key, (uint8_t *)pt, (uint8_t *)ct, ptlen);
	    printf("\n");
	    //IV Generation
	    printf("Second encryption :\n");
	    rand_data(rData);
	    iv = myRandomData;
	    cbc_enc((uint16_t *)key, (uint8_t *)pt, (uint8_t *)ct, ptlen);
	    printf("\n");
	    //New IV
	    printf("Third encryption :\n");
	    rand_data(rData);
	    iv = myRandomData;
	    cbc_enc((uint16_t *)key, (uint8_t *)pt, (uint8_t *)ct, ptlen);
	    printf("\n");
	    //Decryption
	    printf("Decryption :\n");
	    cbc_dec((uint16_t *)key, (uint8_t *)ct, (uint8_t *)ptdec, ctlen);

	    //Attack
	    printf("Attack :\n");
	    while (!attack((uint8_t *)ct, ctlen)) {
	    	rand_data(rData);
		    pt[0] = myRandomData;
		    rand_data(rData);
		    pt[1] = myRandomData;
		    cbc_enc((uint16_t *)key, (uint8_t *)pt, (uint8_t *)ct, ptlen);
		    arraySize = 0;
		    nbAttacks += 1;
		    printf("%i\n", nbAttacks);
	    }

	}else{
		printf("Error");
	}



	close(rData);
	return 0;
}