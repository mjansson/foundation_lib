/* sha.c  -  Foundation library  -  Public Domain  -  2016 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>

#if FOUNDATION_COMPILER_CLANG
// We have separate unaligned loads on platforms which requires it
#pragma clang diagnostic ignored "-Wcast-align"
#endif

static const uint32_t K256[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
    0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL, 0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL, 0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL};

static const uint64_t K512[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL,
    0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
    0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
    0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 0x983e5152ee66dfabULL,
    0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL,
    0x53380d139d95b3dfULL, 0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
    0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
    0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL,
    0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, 0xca273eceea26619cULL,
    0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
    0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
    0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL};

static uint32_t
choise32(uint32_t x, uint32_t y, uint32_t z) {
	return z ^ (x & (y ^ z));
}
static uint32_t
majority32(uint32_t x, uint32_t y, uint32_t z) {
	return ((x | y) & z) | (x & y);
}
static uint32_t
rotate32(uint32_t x, uint32_t n) {
	return (x >> (n & 31)) | (x << (32 - (n & 31)));
}
static uint32_t
shift32(uint32_t x, uint32_t n) {
	return x >> n;
}
static uint32_t
sigma0_32(uint32_t x) {
	return rotate32(x, 2) ^ rotate32(x, 13) ^ rotate32(x, 22);
}
static uint32_t
sigma1_32(uint32_t x) {
	return rotate32(x, 6) ^ rotate32(x, 11) ^ rotate32(x, 25);
}
static uint32_t
gamma0_32(uint32_t x) {
	return rotate32(x, 7) ^ rotate32(x, 18) ^ shift32(x, 3);
}
static uint32_t
gamma1_32(uint32_t x) {
	return rotate32(x, 17) ^ rotate32(x, 19) ^ shift32(x, 10);
}

static uint64_t
choise64(uint64_t x, uint64_t y, uint64_t z) {
	return z ^ (x & (y ^ z));
}
static uint64_t
majority64(uint64_t x, uint64_t y, uint64_t z) {
	return ((x | y) & z) | (x & y);
}
static uint64_t
rotate64(uint64_t x, uint64_t n) {
	return (x >> (n & 63)) | (x << (64 - (n & 63)));
}
static uint64_t
shift64(uint64_t x, uint64_t n) {
	return x >> n;
}
static uint64_t
sigma0_64(uint64_t x) {
	return rotate64(x, 28) ^ rotate64(x, 34) ^ rotate64(x, 39);
}
static uint64_t
sigma1_64(uint64_t x) {
	return rotate64(x, 14) ^ rotate64(x, 18) ^ rotate64(x, 41);
}
static uint64_t
gamma0_64(uint64_t x) {
	return rotate64(x, 1) ^ rotate64(x, 8) ^ shift64(x, 7);
}
static uint64_t
gamma1_64(uint64_t x) {
	return rotate64(x, 19) ^ rotate64(x, 61) ^ shift64(x, 6);
}

static void
compress32(const uint32_t* wbox, uint32_t a, uint32_t b, uint32_t c, uint32_t* d, uint32_t e, uint32_t f, uint32_t g,
           uint32_t* h, uint32_t i) {
	uint32_t t0 = *h + sigma1_32(e) + choise32(e, f, g) + K256[i] + wbox[i];
	uint32_t t1 = sigma0_32(a) + majority32(a, b, c);
	*d += t0;
	*h = t0 + t1;
}

static void
compress64(const uint64_t* wbox, uint64_t a, uint64_t b, uint64_t c, uint64_t* d, uint64_t e, uint64_t f, uint64_t g,
           uint64_t* h, uint64_t i) {
	uint64_t t0 = *h + sigma1_64(e) + choise64(e, f, g) + K512[i] + wbox[i];
	uint64_t t1 = sigma0_64(a) + majority64(a, b, c);
	*d += t0;
	*h = t0 + t1;
}

static uint32_t
sha_load32(const unsigned char* buffer) {
#if FOUNDATION_ARCH_ARM || FOUNDATION_ARCH_ARM_64
#if FOUNDATION_ARCH_ENDIAN_LITTLE
	return ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) | ((uint32_t)buffer[2] << 8) |
	       ((uint32_t)buffer[3] << 0);
#else
	return ((uint32_t)buffer[0] << 0) | ((uint32_t)buffer[1] << 8) | ((uint32_t)buffer[2] << 16) |
	       ((uint32_t)buffer[3] << 24);
#endif
#else
	return byteorder_bigendian32(*(const uint32_t*)buffer);
#endif
}

static uint64_t
sha_load64(const unsigned char* buffer) {
#if FOUNDATION_ARCH_ARM || FOUNDATION_ARCH_ARM_64
	uint64_t res = 0;
	for (uint64_t i = 0; i != 8; ++i)
#if FOUNDATION_ARCH_ENDIAN_LITTLE
		res |= (uint64_t)buffer[i] << ((7ULL - i) * 8ULL);
#else
		res |= (uint64_t)buffer[i] << (i * 8ULL);
#endif
	return res;
#else
	return byteorder_bigendian64(*(const uint64_t*)buffer);
#endif
}

/*static void sha_store32(unsigned char* buffer, uint32_t val) {
#if FOUNDATION_ARCH_ARM || FOUNDATION_ARCH_ARM_64
    for (uint32_t i = 0; i < 4; ++i)
#  if FOUNDATION_ARCH_ENDIAN_LITTLE
        buffer[i] = (val >> ((3UL-i) * 8UL)) & 0xFF;
#  else
        buffer[i] = (val >> (i * 8UL)) & 0xFF;
#  endif
#else
    *(uint32_t*)buffer = byteorder_bigendian32(val);
#endif
}*/

static void
sha_store64(unsigned char* buffer, uint64_t val) {
#if FOUNDATION_ARCH_ARM || FOUNDATION_ARCH_ARM_64
	for (uint64_t i = 0; i < 8; ++i)
#if FOUNDATION_ARCH_ENDIAN_LITTLE
		buffer[i] = (val >> ((7ULL - i) * 8ULL)) & 0xFF;
#else
		buffer[i] = (val >> (i * 8ULL)) & 0xFF;
#endif
#else
	*(uint64_t*)buffer = byteorder_bigendian64(val);
#endif
}

static void
sha256_compress(sha256_t* digest, const unsigned char* buffer) {
	uint32_t sbox[8];
	uint32_t wbox[64];
	uint32_t i;

	for (i = 0; i < 8; i++)
		sbox[i] = digest->state[i];

	for (i = 0; i < 16; ++i)
		wbox[i] = sha_load32(buffer + (4 * i));

	for (i = 16; i < 64; ++i)
		wbox[i] = gamma1_32(wbox[i - 2]) + wbox[i - 7] + gamma0_32(wbox[i - 15]) + wbox[i - 16];

	for (i = 0; i < 64;) {
		compress32(wbox, sbox[0], sbox[1], sbox[2], sbox + 3, sbox[4], sbox[5], sbox[6], sbox + 7, i++);

		compress32(wbox, sbox[7], sbox[0], sbox[1], sbox + 2, sbox[3], sbox[4], sbox[5], sbox + 6, i++);

		compress32(wbox, sbox[6], sbox[7], sbox[0], sbox + 1, sbox[2], sbox[3], sbox[4], sbox + 5, i++);

		compress32(wbox, sbox[5], sbox[6], sbox[7], sbox + 0, sbox[1], sbox[2], sbox[3], sbox + 4, i++);

		compress32(wbox, sbox[4], sbox[5], sbox[6], sbox + 7, sbox[0], sbox[1], sbox[2], sbox + 3, i++);

		compress32(wbox, sbox[3], sbox[4], sbox[5], sbox + 6, sbox[7], sbox[0], sbox[1], sbox + 2, i++);

		compress32(wbox, sbox[2], sbox[3], sbox[4], sbox + 5, sbox[6], sbox[7], sbox[0], sbox + 1, i++);

		compress32(wbox, sbox[1], sbox[2], sbox[3], sbox + 4, sbox[5], sbox[6], sbox[7], sbox + 0, i++);
	}

	for (i = 0; i < 8; ++i)
		digest->state[i] = digest->state[i] + sbox[i];
}

static void
sha512_compress(sha512_t* digest, const unsigned char* buffer) {
	uint64_t sbox[8];
	uint64_t wbox[80];
	uint32_t i;

	for (i = 0; i < 8; i++)
		sbox[i] = digest->state[i];

	for (i = 0; i < 16; ++i)
		wbox[i] = sha_load64(buffer + (8 * i));

	for (i = 16; i < 80; ++i)
		wbox[i] = gamma1_64(wbox[i - 2]) + wbox[i - 7] + gamma0_64(wbox[i - 15]) + wbox[i - 16];

	for (i = 0; i < 80;) {
		compress64(wbox, sbox[0], sbox[1], sbox[2], sbox + 3, sbox[4], sbox[5], sbox[6], sbox + 7, i++);

		compress64(wbox, sbox[7], sbox[0], sbox[1], sbox + 2, sbox[3], sbox[4], sbox[5], sbox + 6, i++);

		compress64(wbox, sbox[6], sbox[7], sbox[0], sbox + 1, sbox[2], sbox[3], sbox[4], sbox + 5, i++);

		compress64(wbox, sbox[5], sbox[6], sbox[7], sbox + 0, sbox[1], sbox[2], sbox[3], sbox + 4, i++);

		compress64(wbox, sbox[4], sbox[5], sbox[6], sbox + 7, sbox[0], sbox[1], sbox[2], sbox + 3, i++);

		compress64(wbox, sbox[3], sbox[4], sbox[5], sbox + 6, sbox[7], sbox[0], sbox[1], sbox + 2, i++);

		compress64(wbox, sbox[2], sbox[3], sbox[4], sbox + 5, sbox[6], sbox[7], sbox[0], sbox + 1, i++);

		compress64(wbox, sbox[1], sbox[2], sbox[3], sbox + 4, sbox[5], sbox[6], sbox[7], sbox + 0, i++);
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

	if (digest->current < 56)
		memset(digest->buffer + digest->current, 0, 56 - digest->current);

	sha_store64(digest->buffer + 56, digest->length);
	sha256_compress(digest, digest->buffer);

	digest->init = true;
}

uint256_t
sha256_get_digest_raw(const sha256_t* digest) {
	uint256_t val = {{((uint64_t)digest->state[0] << 32ULL) | (uint64_t)digest->state[1],
	                  ((uint64_t)digest->state[2] << 32ULL) | (uint64_t)digest->state[3],
	                  ((uint64_t)digest->state[4] << 32ULL) | (uint64_t)digest->state[5],
	                  ((uint64_t)digest->state[6] << 32ULL) | (uint64_t)digest->state[7]}};
	return val;
}

string_t
sha256_get_digest(const sha256_t* digest, char* str, size_t length) {
	uint256_t raw = sha256_get_digest_raw(digest);
	return string_from_uint256(str, length, raw);
}

sha512_t*
sha512_allocate(void) {
	sha512_t* digest = memory_allocate(0, sizeof(sha512_t), 0, MEMORY_PERSISTENT);
	sha512_initialize(digest);
	return digest;
}

void
sha512_deallocate(sha512_t* digest) {
	sha512_finalize(digest);
	memory_deallocate(digest);
}

void
sha512_initialize(sha512_t* digest) {
	digest->init = false;
	digest->current = 0;
	digest->length = 0;
	digest->state[0] = 0x6a09e667f3bcc908ULL;
	digest->state[1] = 0xbb67ae8584caa73bULL;
	digest->state[2] = 0x3c6ef372fe94f82bULL;
	digest->state[3] = 0xa54ff53a5f1d36f1ULL;
	digest->state[4] = 0x510e527fade682d1ULL;
	digest->state[5] = 0x9b05688c2b3e6c1fULL;
	digest->state[6] = 0x1f83d9abfb41bd6bULL;
	digest->state[7] = 0x5be0cd19137e2179ULL;
}

void
sha512_finalize(sha512_t* digest) {
	FOUNDATION_UNUSED(digest);
}

sha512_t*
sha512_digest(sha512_t* digest, const void* buffer, size_t size) {
	const size_t block_size = sizeof(digest->buffer);

	if (digest->init)
		sha512_initialize(digest);

	if (digest->current) {
		size_t remain = block_size - digest->current;
		size_t this_block = math_min(size, remain);
		memcpy(digest->buffer + digest->current, buffer, this_block);
		digest->current += this_block;
		buffer = pointer_offset_const(buffer, this_block);
		size -= this_block;

		if (digest->current == block_size) {
			sha512_compress(digest, digest->buffer);
			digest->length += block_size * 8;
			digest->current = 0;
		}
	}

	while (size >= block_size) {
		sha512_compress(digest, buffer);
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
sha512_digest_finalize(sha512_t* digest) {
	digest->length += digest->current * 8ULL;

	digest->buffer[digest->current++] = 0x80;

	if (digest->current > 112) {
		while (digest->current < 128)
			digest->buffer[digest->current++] = 0;
		sha512_compress(digest, digest->buffer);
		digest->current = 0;
	}

	if (digest->current < 120)
		memset(digest->buffer + digest->current, 0, 120 - digest->current);

	sha_store64(digest->buffer + 120, digest->length);
	sha512_compress(digest, digest->buffer);

	digest->init = true;
}

uint512_t
sha512_get_digest_raw(const sha512_t* digest) {
	uint512_t val = {{digest->state[0], digest->state[1], digest->state[2], digest->state[3], digest->state[4],
	                  digest->state[5], digest->state[6], digest->state[7]}};
	return val;
}

string_t
sha512_get_digest(const sha512_t* digest, char* buffer, size_t capacity) {
	uint512_t raw = sha512_get_digest_raw(digest);
	return string_from_uint512(buffer, capacity, raw);
}
