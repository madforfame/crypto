#include "tczero.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>

uint64_t myRandomData;

size_t cbc_enc(uint64_t key[2], uint8_t *pt, uint8_t *ct, size_t ptlen){
	//on suppose ptlen pair, nb d'octet
	printf("Encryption\n");
	uint64_t x[2];
	x[0]=0;
	x[1]=0;
	int i=0, j=0;

	//on recupere le random généré preamptivement :4d chess
	uint64_t iv=myRandomData;


	printf("\nTEST: on a pt=%" PRIu64, *((uint64_t *) pt));

	for(i=0;i<ptlen/2;i++){
		x[0]=x[0] | (uint64_t) pt[i]<< (8*i);

	}
	for(;i<ptlen;i++){
		x[1]=x[1] | (uint64_t) pt[i]<< (8*j);
		j++;
	}
	x[0]=x[0] ^ (iv & 0xFFFF);
	x[1]=x[1] ^ (iv >> 16) & 0xFFFF;


	printf("\nOn a donc:\nx[0]=%" PRIu64 "\nx[1]=%" PRIu64, x[0], x[1]);
	tc0_encrypt(x, key);
	printf("\nOn a donc:\nx[0]=%" PRIu64 "\nx[1]=%" PRIu64, x[0], x[1]);

	j=0;
	for(i=0;i<ptlen/2;i++){
		ct[i]=(uint8_t) (x[0] >> (8*i)) & 0xFF;

	}
	for(;i<ptlen;i++){
		ct[i]=(uint8_t) (x[1] >> (8*j)) & 0xFF;
		j++;
	}
	printf("\nTEST: on a ct=%" PRIu64, *((uint64_t *) ct));
	printf("\nTEST: on a x =%" PRIu64, *x);

	return ptlen;
	
}

size_t cbc_dec(uint64_t key[2], uint8_t *ct, uint8_t *pt, size_t ctlen) {
	int i=0, j=0;
	uint64_t x[2];
	x[0] = 0;
	x[1] = 0;
	printf("Decrypt");
	for(i=0;i<ctlen/2;i++){
		x[0]=x[0] | (uint64_t) ct[i]<< (8*i);

	}
	for(;i<ctlen;i++){
		x[1]=x[1] | (uint64_t) ct[i]<< (8*j);
		j++;
	}
	printf("\nTEST: on a ct=%" PRIu64, *((uint64_t *) ct));
	printf("\nOn a donc:\nx[0]=%" PRIu64 "\nx[1]=%" PRIu64, x[0], x[1]);
	tc0_decrypt(x,key);
	printf("\nOn a donc:\nx[0]=%" PRIu64 "\nx[1]=%" PRIu64, x[0], x[1]);

	j=0;
	for(i=0;i<ctlen/2;i++){
		pt[i]=(uint8_t) (x[0] >> (8*i)) & 0xFF;

	}
	for(;i<ctlen;i++){
		pt[i]=(uint8_t) (x[1] >> (8*j)) & 0xFF;
		j++;
	}

	printf("\nTEST: on a pt=%" PRIu64, *((uint64_t *) pt));
	return ctlen;

}

void rand_data(int f){
	ssize_t result = read(f, &myRandomData, sizeof myRandomData);
	if (result < 0){
	    printf("Probleme lors de la crea random");
	}else{
		//print_tab_1d(myRandomData, HALF_BLOCK_SIZE);
	}
}

int main(){
	int rData = open("/dev/urandom", O_RDONLY);
	uint64_t key[2];

	if(rData >= 0)
	{
	    rand_data(rData);
	    key[0]=myRandomData;

	    rand_data(rData);
	    key[1]=myRandomData;
	    printf("%" PRIu64 "\n", key[0]);
	    printf("%" PRIu64 "\n", key[1]);
	    uint8_t *pt;

	    rand_data(rData);

	    //Attention
	    pt = malloc(HALF_BLOCK_SIZE*2);
	    uint8_t *ct;
	    ct = malloc(HALF_BLOCK_SIZE*2);
	    size_t ptlen=HALF_BLOCK_SIZE*2/8;
	    size_t ctlen = HALF_BLOCK_SIZE*2/8;
	    uint64_t k=5713995888734828147;
	    uint8_t * w=(uint8_t *)&k;

	    for(int i=0;i<ptlen;i++){

	    	pt[i]=w[i];
	    }
	    
	    

	    printf("\nOn obtient la size=%zu\n",cbc_enc(key, pt, ct, ptlen));
	    printf("\nOn obtient la size=%zu\n",cbc_dec(key,ct,pt,ctlen));



	}else{
		printf("Probleme");
	}



	close(rData);
	return 0;
}