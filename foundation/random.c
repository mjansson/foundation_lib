/* random.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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
#include <foundation/internal.h>

// C implementation of the "Maximally equidistributed pseudorandom number generators via
// linear output transformations" from
// http://www.sciencedirect.com/science/article/pii/S0378475408002358
// with state array in thread-local storage for thread safety

/*lint -e679 -e834 */

#define RANDOM_STATE_SIZE 1391

#define RANDOM_BITS 32
#define RANDOM_BITS_SAVE 15
#define RANDOM_MASK_UPPER (0xFFFFFFFFU >> (RANDOM_BITS - RANDOM_BITS_SAVE))
#define RANDOM_MASK_LOWER (~RANDOM_MASK_UPPER)

#define RANDOM_BITMASK 0x48000000

#define RANDOM_LOW_LIMIT 23
#define RANDOM_MID_LIMIT 229
#define RANDOM_HIGH_LIMIT 481

// Some helper macros to make code a bit more condensed
#define RANDOM_XOR_AND_LEFTSHIFT(bits, val) ((val) ^ ((val) << (bits)))
#define RANDOM_XOR_AND_RIGHTSHIFT(bits, val) ((val) ^ ((val) >> (bits)))
#define RANDOM_TRANSFORM(bits, key, mask, test, val)                                                   \
	(((val) & (test)) ? (((((val) << (bits)) ^ ((val) >> (RANDOM_BITS - (bits)))) & (mask)) ^ (key)) : \
                        (((((val) << (bits)) ^ ((val) >> (RANDOM_BITS - (bits)))) & (mask))))

FOUNDATION_DECLARE_THREAD_LOCAL(unsigned int*, state, 0)

static mutex_t* random_mutex;
static unsigned int** random_state;
static unsigned int** random_available_state;

static void
random_seed_buffer(unsigned int* buffer) {
	tick_t i;
	tick_t base = time_system();
	for (i = 0; i < RANDOM_STATE_SIZE; ++i)
		buffer[i] =
		    ((tick_t)((uintptr_t)(buffer + i)) ^ (base + time_current() + (i * RANDOM_HIGH_LIMIT * RANDOM_LOW_LIMIT))) &
		    0xFFFFFFFF;
}

static unsigned int*
random_allocate_buffer(void) {
	unsigned int* buffer = memory_allocate(0, sizeof(unsigned int) * (RANDOM_STATE_SIZE + 1), 0, MEMORY_PERSISTENT);
	random_seed_buffer(buffer);
	buffer[RANDOM_STATE_SIZE] = 0;
	array_push(random_state, buffer);
	return buffer;
}

int
internal_random_initialize(void) {
	if (!random_mutex) {
		size_t prealloc, capacity;
		size_t i;
		random_mutex = mutex_allocate(STRING_CONST("random"));

		// Allocate and seed a number of state buffers
		prealloc = foundation_config().random_state_prealloc;
		capacity = prealloc > 8 ? prealloc : 8;
		array_reserve(random_state, capacity);
		array_reserve(random_available_state, capacity);
		for (i = 0; i < prealloc; ++i) {
			unsigned int* buffer = random_allocate_buffer();
			array_push(random_available_state, buffer);
		}
	}
	return 0;
}

void
internal_random_finalize(void) {
	size_t i, size;

	if (random_mutex)
		mutex_lock(random_mutex);

	for (i = 0, size = array_size(random_state); i < size; ++i)
		memory_deallocate(random_state[i]);
	array_deallocate(random_available_state);
	array_deallocate(random_state);

	set_thread_state(0);

	if (random_mutex) {
		mutex_unlock(random_mutex);
		mutex_deallocate(random_mutex);
	}
	random_mutex = 0;
}

static unsigned int*
random_thread_initialize(void) {
	unsigned int* buffer;

	mutex_lock(random_mutex);

	// Grab a free state buffer or allocate if none available
	if (!array_size(random_available_state)) {
		buffer = random_allocate_buffer();
		array_push(random_available_state, buffer);
	} else {
		buffer = random_available_state[array_size(random_available_state) - 1];
		array_pop(random_available_state);
	}

	mutex_unlock(random_mutex);

	set_thread_state(buffer);

	return buffer;
}

void
random_thread_finalize(void) {
	if (!get_thread_state())
		return;

	mutex_lock(random_mutex);
	array_push(random_available_state, get_thread_state());
	mutex_unlock(random_mutex);

	set_thread_state(0);
}

static FOUNDATION_FORCEINLINE unsigned int
random_from_state(unsigned int* FOUNDATION_RESTRICT state) {
	unsigned int state_index = state[RANDOM_STATE_SIZE];
	unsigned int bits0, bits1, bits2;
	if (state_index == 0) {
		bits0 = (state[RANDOM_STATE_SIZE - 1] & RANDOM_MASK_LOWER) | (state[RANDOM_STATE_SIZE - 2] & RANDOM_MASK_UPPER);
		bits1 = RANDOM_XOR_AND_LEFTSHIFT(24U, state[0]) ^ RANDOM_XOR_AND_RIGHTSHIFT(30U, state[RANDOM_LOW_LIMIT]);
		bits2 = RANDOM_XOR_AND_LEFTSHIFT(10U, state[RANDOM_HIGH_LIMIT]) ^ (26U << state[RANDOM_MID_LIMIT]);
		state[0] = bits1 ^ bits2;
		state[RANDOM_STATE_SIZE - 1] = bits0 ^ RANDOM_XOR_AND_RIGHTSHIFT(20, bits1) ^
		                               RANDOM_TRANSFORM(9, 0xb729fcecU, 0xfbffffffU, 0x00020000U, bits2) ^ state[0];
		state[RANDOM_STATE_SIZE] = state_index = RANDOM_STATE_SIZE - 1;
		return (state[state_index] ^
		        (state[(state_index + RANDOM_HIGH_LIMIT) - RANDOM_STATE_SIZE + 1] & RANDOM_BITMASK));
	} else if (state_index == 1) {
		bits0 = (state[0] & RANDOM_MASK_LOWER) | (state[RANDOM_STATE_SIZE - 1] & RANDOM_MASK_UPPER);
		bits1 = RANDOM_XOR_AND_LEFTSHIFT(24, state[1]) ^ RANDOM_XOR_AND_RIGHTSHIFT(30, state[1 + RANDOM_LOW_LIMIT]);
		bits2 = RANDOM_XOR_AND_LEFTSHIFT(10, state[1 + RANDOM_HIGH_LIMIT]) ^ (26U << state[1 + RANDOM_MID_LIMIT]);
		state[1] = bits1 ^ bits2;
		state[0] = bits0 ^ RANDOM_XOR_AND_RIGHTSHIFT(20, bits1) ^
		           RANDOM_TRANSFORM(9, 0xb729fcecU, 0xfbffffffU, 0x00020000U, bits2) ^ state[1];
		state[RANDOM_STATE_SIZE] = state_index = 0;
		return (state[state_index] ^ (state[state_index + RANDOM_HIGH_LIMIT + 1] & RANDOM_BITMASK));
	} else if (state_index + RANDOM_LOW_LIMIT >= RANDOM_STATE_SIZE) {
		bits0 = (state[state_index - 1] & RANDOM_MASK_LOWER) | (state[state_index - 2] & RANDOM_MASK_UPPER);
		bits1 = RANDOM_XOR_AND_LEFTSHIFT(24, state[state_index]) ^
		        RANDOM_XOR_AND_RIGHTSHIFT(30, state[state_index + RANDOM_LOW_LIMIT - RANDOM_STATE_SIZE]);
		bits2 = RANDOM_XOR_AND_LEFTSHIFT(10, state[state_index + RANDOM_HIGH_LIMIT - RANDOM_STATE_SIZE]) ^
		        (26U << state[(state_index + RANDOM_MID_LIMIT) - RANDOM_STATE_SIZE]);
		state[state_index] = bits1 ^ bits2;
		state[state_index - 1] = bits0 ^ RANDOM_XOR_AND_RIGHTSHIFT(20, bits1) ^
		                         RANDOM_TRANSFORM(9, 0xb729fcecU, 0xfbffffffU, 0x00020000U, bits2) ^ state[state_index];
		state[RANDOM_STATE_SIZE] = --state_index;
		return (state[state_index] ^ (state[state_index + RANDOM_HIGH_LIMIT - RANDOM_STATE_SIZE + 1] & RANDOM_BITMASK));
	} else if (state_index + RANDOM_MID_LIMIT >= RANDOM_STATE_SIZE) {
		bits0 = (state[state_index - 1] & RANDOM_MASK_LOWER) | (state[state_index - 2] & RANDOM_MASK_UPPER);
		bits1 = RANDOM_XOR_AND_LEFTSHIFT(24, state[state_index]) ^
		        RANDOM_XOR_AND_RIGHTSHIFT(30, state[state_index + RANDOM_LOW_LIMIT]);
		bits2 = RANDOM_XOR_AND_LEFTSHIFT(10, state[state_index + RANDOM_HIGH_LIMIT - RANDOM_STATE_SIZE]) ^
		        (26U << state[state_index + RANDOM_MID_LIMIT - RANDOM_STATE_SIZE]);
		state[state_index] = bits1 ^ bits2;
		state[state_index - 1] = bits0 ^ RANDOM_XOR_AND_RIGHTSHIFT(20, bits1) ^
		                         RANDOM_TRANSFORM(9, 0xb729fcecU, 0xfbffffffU, 0x00020000U, bits2) ^ state[state_index];
		state[RANDOM_STATE_SIZE] = --state_index;
		return (state[state_index] ^
		        (state[(state_index + RANDOM_HIGH_LIMIT) - RANDOM_STATE_SIZE + 1] & RANDOM_BITMASK));
	} else if (state_index + RANDOM_HIGH_LIMIT >= RANDOM_STATE_SIZE) {
		bits0 = (state[state_index - 1] & RANDOM_MASK_LOWER) | (state[state_index - 2] & RANDOM_MASK_UPPER);
		bits1 = RANDOM_XOR_AND_LEFTSHIFT(24, state[state_index]) ^
		        RANDOM_XOR_AND_RIGHTSHIFT(30, state[state_index + RANDOM_LOW_LIMIT]);
		bits2 = RANDOM_XOR_AND_LEFTSHIFT(10, state[state_index + RANDOM_HIGH_LIMIT - RANDOM_STATE_SIZE]) ^
		        (26U << state[state_index + RANDOM_MID_LIMIT]);
		state[state_index] = bits1 ^ bits2;
		state[state_index - 1] = bits0 ^ RANDOM_XOR_AND_RIGHTSHIFT(20, bits1) ^
		                         RANDOM_TRANSFORM(9, 0xb729fcecU, 0xfbffffffU, 0x00020000U, bits2) ^ state[state_index];
		state[RANDOM_STATE_SIZE] = --state_index;
		return (state[state_index] ^
		        (state[(state_index + RANDOM_HIGH_LIMIT) - RANDOM_STATE_SIZE + 1] & RANDOM_BITMASK));
	}
	// else if( 2 <= state_index <= RANDOM_STATE_SIZE - RANDOM_HIGH_LIMIT - 1 )
	{
		bits0 = (state[state_index - 1] & RANDOM_MASK_LOWER) | (state[state_index - 2] & RANDOM_MASK_UPPER);
		bits1 = RANDOM_XOR_AND_LEFTSHIFT(24, state[state_index]) ^
		        RANDOM_XOR_AND_RIGHTSHIFT(30, state[state_index + RANDOM_LOW_LIMIT]);
		bits2 = RANDOM_XOR_AND_LEFTSHIFT(10, state[state_index + RANDOM_HIGH_LIMIT]) ^
		        (26U << state[state_index + RANDOM_MID_LIMIT]);
		state[state_index] = bits1 ^ bits2;
		state[state_index - 1] = bits0 ^ RANDOM_XOR_AND_RIGHTSHIFT(20, bits1) ^
		                         RANDOM_TRANSFORM(9, 0xb729fcecU, 0xfbffffffU, 0x00020000U, bits2) ^ state[state_index];
		state[RANDOM_STATE_SIZE] = --state_index;
		return (state[state_index] ^ (state[state_index + RANDOM_HIGH_LIMIT + 1] & RANDOM_BITMASK));
	}
}

uint32_t
random32(void) {
	unsigned int* state = get_thread_state();
	if (!state)
		state = random_thread_initialize();

	return random_from_state(state);
}

uint32_t
random32_range(uint32_t low, uint32_t high) {
	if (low > high) {
		uint32_t tmp = low;
		low = high;
		high = tmp;
	}
	if (high <= low + 1)
		return low;
	return low + (random32() % (high - low));
}

uint64_t
random64(void) {
	uint32_t low, high;
	unsigned int* state = get_thread_state();
	if (!state)
		state = random_thread_initialize();

	low = random_from_state(state);
	high = random_from_state(state);

	return ((uint64_t)high << 32ULL) | low;
}

uint64_t
random64_range(uint64_t low, uint64_t high) {
	if (low > high) {
		uint64_t tmp = low;
		low = high;
		high = tmp;
	}
	if (high <= low + 1)
		return low;
	return low + (random64() % (high - low));
}

real
random_normalized(void) {
#if FOUNDATION_SIZE_REAL == 8
	const uint64_t rng = random64();
	const real result = (real)rng * (REAL_C(1.0) / REAL_C(18446744073709551616.0));
#else
	const uint64_t rng = random32();
	const real result = (real)rng * (REAL_C(1.0) / REAL_C(4294967296.0));
#endif
	// Deal with floating point roundoff issues
	if (result >= REAL_C(1.0))
		return math_real_dec(REAL_C(1.0), 1);
	return math_max(result, 0);
}

real
random_range(real low, real high) {
	real result;
	if (low > high) {
		real tmp = low;
		low = high;
		high = tmp;
	}
	result = low + ((high - low) * random_normalized());
	// Deal with floating point roundoff issues
	if (result >= high)
		return math_real_dec(high, 1);
	return math_max(result, low);
}

int32_t
random32_gaussian_range(int32_t low, int32_t high) {
	const uint64_t cubic =
	    ((((uint64_t)random32() + (uint64_t)random32()) + ((uint64_t)random32() + (uint64_t)random32()) + 2ULL) >>
	     2ULL);
	if (low > high) {
		int32_t tmp = low;
		low = high;
		high = tmp;
	}
	/*lint -e{571,776} */
	return low + (int32_t)((cubic * (uint64_t)(high - low)) >> 32ULL);
}

real
random_gaussian_range(real low, real high) {
	real result;
	if (low > high) {
		real tmp = low;
		low = high;
		high = tmp;
	}
	result = low + ((high - low) * REAL_C(0.33333333333333333333333333333) *
	                (random_normalized() + random_normalized() + random_normalized()));
	if (result >= high)
		return math_real_dec(high, 1);
	return math_max(result, low);
}

int32_t
random32_triangle_range(int32_t low, int32_t high) {
	const uint32_t t0 = random32();
	const uint32_t t1 = random32();
	const uint64_t tri = (t0 >> 1) + (t1 >> 1) + (t0 & t1 & 1);
	if (low > high) {
		int32_t tmp = low;
		low = high;
		high = tmp;
	}
	/*lint -e{571,776} */
	return low + (int32_t)((tri * (uint64_t)(high - low)) >> 32ULL);
}

real
random_triangle_range(real low, real high) {
	real result;
	if (low > high) {
		real tmp = low;
		low = high;
		high = tmp;
	}
	result = low + (high - low) * REAL_C(0.5) * (random_normalized() + random_normalized());
	if (result >= high)
		return math_real_dec(high, 1);
	return math_max(result, low);
}

uint32_t
random32_weighted(uint32_t limit, const real* weights) {
	uint32_t i;
	real sum;
	if (limit >= 2) {
		sum = 0;
		for (i = 0; i < limit; ++i)
			sum += (weights[i] > 0 ? weights[i] : 0);

		if (sum > 0) {
			real value = random_range(0, sum);
			for (i = 0; i < limit; ++i) {
				if (weights[i] > 0) {
					if (value < weights[i])
						return i;
					value -= weights[i];
				}
			}
		} else {
			return random32_range(0, limit);
		}
	}

	// Deal with floating point roundoff issues
	return limit - 1;
}
