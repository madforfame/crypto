#include <stdint.h>

void speck32_64_encrypt(const uint16_t k[4], const uint8_t p[4], uint8_t c[4]);

void speck32_64_decrypt(const uint16_t k[4], const uint8_t c[4], uint8_t p[4]);