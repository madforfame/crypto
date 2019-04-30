#include <stdint.h>
#include "speck32.h"

#define ROTL16_9(x) ((((x) << 9) ^ ((x) >> 7)) & 0xFFFF)
#define ROTL16_2(x) ((((x) << 2) ^ ((x) >> 14)) & 0xFFFF)

#define ROTL16_7(x) ((((x) << 7) ^ ((x) >> 9)) & 0xFFFF)
#define ROTL16_14(x) ((((x) << 14) ^ ((x) >> 2)) & 0xFFFF)

void speck32_64_encrypt(const uint16_t k[4], const uint8_t p[4], uint8_t c[4])
{
	uint16_t rk[21];
	uint16_t ell[3] = {k[1], k[2], k[3]};
	uint16_t ct[2];

	rk[0] = k[0];

	ct[0] = p[0] ^ ((uint16_t)p[1] << 8);
	ct[1] = p[2] ^ ((uint16_t)p[3] << 8);
	
	/* full key schedule */
	for (unsigned i = 0; i < 21; i++)
	{
		uint16_t new_ell = ((ROTL16_9(ell[0]) + rk[i]) ^ i) & 0xFFFF;
		rk[i+1] = ROTL16_2(rk[i]) ^ new_ell;
		ell[0] = ell[1];
		ell[1] = ell[2];
		ell[2] = new_ell;
	}

	for (unsigned i = 0; i < 22; i++)
	{
		ct[0] = ((ROTL16_9(ct[0]) + ct[1]) ^ rk[i]) & 0xFFFF;
		ct[1] = ROTL16_2(ct[1]) ^ ct[0];
	}

	c[0] = ct[0] & 0xFF;
	c[1] = ct[0] >> 8;
	c[2] = ct[1] & 0xFF;
	c[3] = ct[1] >> 8;

	return;
}

void speck32_64_decrypt(const uint16_t k[4], const uint8_t c[4], uint8_t p[4])
{
	uint16_t rk[23];
	uint16_t ell[3] = {k[1], k[2], k[3]};
	uint16_t pt[2];

	rk[0] = k[0];

	pt[0] = c[0] ^ ((uint16_t)c[1] << 8);
	pt[1] = c[2] ^ ((uint16_t)c[3] << 8);

	/* full key schedule */
	for (unsigned i = 0; i < 21; i++)
	{
		uint16_t new_ell = ((ROTL16_9(ell[0]) + rk[i]) ^ i) & 0xFFFF;
		rk[i+1] = ROTL16_2(rk[i]) ^ new_ell;
		ell[0] = ell[1];
		ell[1] = ell[2];
		ell[2] = new_ell;
	}

	for (int i = 21; i >= 0; i--)
	{
		pt[1] = ROTL16_14(pt[1] ^ pt[0]);
		pt[0] = ROTL16_7(((pt[0] ^ rk[i]) - pt[1]) & 0xFFFF);
	}

	p[0] = pt[0] & 0xFF;
	p[1] = pt[0] >> 8;
	p[2] = pt[1] & 0xFF;
	p[3] = pt[1] >> 8;

	return;
}
