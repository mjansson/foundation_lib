/* sha.c  -  Foundation library  -  Public Domain  -  2016 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>

static const uint32_t K256[64] = {
	0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
	0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
	0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
	0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
	0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
	0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
	0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
	0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
	0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
	0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
	0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
	0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
	0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

static uint32_t choise(uint32_t x, uint32_t y, uint32_t z) { return z ^ (x & (y ^ z)); }
static uint32_t majority(uint32_t x, uint32_t y, uint32_t z) { return ((x | y) & z) | (x & y); }
static uint32_t rotate(uint32_t x, uint32_t n) { return (x >> (n & 31)) | (x << (32 - (n & 31))); }
static uint32_t shift(uint32_t x, uint32_t n) { return x >> n; }
static uint32_t sigma0(uint32_t x) { return rotate(x, 2) ^ rotate(x, 13) ^ rotate(x, 22); }
static uint32_t sigma1(uint32_t x) { return rotate(x, 6) ^ rotate(x, 11) ^ rotate(x, 25); }
static uint32_t gamma0(uint32_t x) { return rotate(x, 7) ^ rotate(x, 18) ^ shift(x, 3); }
static uint32_t gamma1(uint32_t x) { return rotate(x, 17) ^ rotate(x, 19) ^ shift(x, 10); }

static void compress(const uint32_t* wbox,
                     uint32_t a, uint32_t b, uint32_t c, uint32_t* d,
                     uint32_t e, uint32_t f, uint32_t g, uint32_t* h, uint32_t i) {
	uint32_t t0 = *h + sigma1(e) + choise(e, f, g) + K256[i] + wbox[i];
	uint32_t t1 = sigma0(a) + majority(a, b, c);
	*d += t0;
	*h = t0 + t1;
}

static uint32_t sha_load32(const unsigned char* buffer) {
	return ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) |
	       ((uint32_t)buffer[2] << 8) | ((uint32_t)buffer[3] << 0);
}

static void sha_store32(unsigned char* buffer, uint32_t val) {
	for (uint32_t i = 0; i < 4; ++i)
		buffer[i] = (val >> ((3UL-i) * 8UL)) & 0xFF;
}

static void sha_store64(unsigned char* buffer, uint64_t val) {
	for (uint64_t i = 0; i < 8; ++i)
		buffer[i] = (val >> ((7ULL-i) * 8ULL)) & 0xFF;
}

static void sha256_compress(sha256_t* digest, const unsigned char* buffer) {
	uint32_t sbox[8];
	uint32_t wbox[64];
	int i;

	for (i = 0; i < 8; i++)
		sbox[i] = digest->state[i];

	for (i = 0; i < 16; ++i)
		wbox[i] = sha_load32(buffer + (4*i));

	for (i = 16; i < 64; ++i)
		wbox[i] = gamma1(wbox[i - 2]) + wbox[i - 7] + gamma0(wbox[i - 15]) + wbox[i - 16];

	//Unrolled
	for (i = 0; i < 64; ) {
		compress(wbox, sbox[0], sbox[1], sbox[2], sbox + 3,
		         sbox[4], sbox[5], sbox[6], sbox + 7, i++);

		compress(wbox, sbox[7], sbox[0], sbox[1], sbox + 2,
		         sbox[3], sbox[4], sbox[5], sbox + 6, i++);

		compress(wbox, sbox[6], sbox[7], sbox[0], sbox + 1,
		         sbox[2], sbox[3], sbox[4], sbox + 5, i++);

		compress(wbox, sbox[5], sbox[6], sbox[7], sbox + 0,
		         sbox[1], sbox[2], sbox[3], sbox + 4, i++);

		compress(wbox, sbox[4], sbox[5], sbox[6], sbox + 7,
		         sbox[0], sbox[1], sbox[2], sbox + 3, i++);

		compress(wbox, sbox[3], sbox[4], sbox[5], sbox + 6,
		         sbox[7], sbox[0], sbox[1], sbox + 2, i++);

		compress(wbox, sbox[2], sbox[3], sbox[4], sbox + 5,
		         sbox[6], sbox[7], sbox[0], sbox + 1, i++);

		compress(wbox, sbox[1], sbox[2], sbox[3], sbox + 4,
		         sbox[5], sbox[6], sbox[7], sbox + 0, i++);
	}

	for (i = 0; i < 8; ++i)
		digest->state[i] = digest->state[i] + sbox[i];
}

sha256_t*
sha256_allocate(void) {
	sha256_t* digest = memory_allocate(0, sizeof(sha256_t), 0, MEMORY_PERSISTENT);
	sha256_initialize(digest);
	return digest;
}

void
sha256_deallocate(sha256_t* digest) {
	sha256_finalize(digest);
	memory_deallocate(digest);
}

void
sha256_initialize(sha256_t* digest) {
	digest->init = false;
	digest->current = 0;
	digest->length = 0;
	digest->state[0] = 0x6A09E667UL;
	digest->state[1] = 0xBB67AE85UL;
	digest->state[2] = 0x3C6EF372UL;
	digest->state[3] = 0xA54FF53AUL;
	digest->state[4] = 0x510E527FUL;
	digest->state[5] = 0x9B05688CUL;
	digest->state[6] = 0x1F83D9ABUL;
	digest->state[7] = 0x5BE0CD19UL;
}

void
sha256_finalize(sha256_t* digest) {
	FOUNDATION_UNUSED(digest);
}

sha256_t*
sha256_digest(sha256_t* digest, const void* buffer, size_t size) {
	const size_t block_size = sizeof(digest->buffer);

	if (digest->init)
		sha256_initialize(digest);

	if (digest->current) {
		size_t remain = block_size - digest->current;
		size_t this_block = math_min(size, remain);
		memcpy(digest->buffer + digest->current, buffer, this_block);
		digest->current += this_block;
		buffer = pointer_offset_const(buffer, this_block);
		size -= this_block;

		if (digest->current == block_size) {
			sha256_compress(digest, digest->buffer);
			digest->length += block_size * 8;
			digest->current = 0;
		}
	}

	while (size >= block_size) {
		sha256_compress(digest, buffer);
		digest->length += block_size * 8;
		buffer = pointer_offset_const(buffer, block_size);
		size -= block_size;
	}

	if (size) {
		memcpy(digest->buffer, buffer, size);
		digest->current += size;
	}

	return digest;
}

void
sha256_digest_finalize(sha256_t* digest) {
	digest->length += digest->current * 8;

	digest->buffer[digest->current++] = 0x80;

	if (digest->current > 56) {
		while (digest->current < 64)
			digest->buffer[digest->current++] = 0;
		sha256_compress(digest, digest->buffer);
		digest->current = 0;
	}

	while (digest->current < 56)
		digest->buffer[digest->current++] = 0;

	sha_store64(digest->buffer + 56, digest->length);
	sha256_compress(digest, digest->buffer);

	digest->init = true;
}

uint256_t
sha256_get_digest_raw(const sha256_t* digest) {
	uint256_t val = {
		((uint64_t)digest->state[0] << 32ULL) | (uint64_t)digest->state[1],
		((uint64_t)digest->state[2] << 32ULL) | (uint64_t)digest->state[3],
		((uint64_t)digest->state[4] << 32ULL) | (uint64_t)digest->state[5],
		((uint64_t)digest->state[6] << 32ULL) | (uint64_t)digest->state[7]
	};
	return val;
}

string_t
sha256_get_digest(const sha256_t* digest, char* str, size_t length) {
	uint256_t raw = sha256_get_digest_raw(digest);
	return string_from_uint256(str, length, raw);
}

