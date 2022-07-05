/* radixsort.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

static const unsigned int radixsort_data_size[] = {
    4,  // RADIXSORT_INT32,
    4,  // RADIXSORT_UINT32,
    8,  // RADIXSORT_INT64,
    8,  // RADIXSORT_UINT64,
    4,  // RADIXSORT_FLOAT32
    8   // RADIXSORT_FLOAT64
};

static const unsigned int radixsort_data_shift[] = {
    2,  // RADIXSORT_INT32,
    2,  // RADIXSORT_UINT32,
    3,  // RADIXSORT_INT64,
    3,  // RADIXSORT_UINT64,
    2,  // RADIXSORT_FLOAT32
    3   // RADIXSORT_FLOAT64
};

static const bool radixsort_data_signed[] = {
    true,   // RADIXSORT_INT32,
    false,  // RADIXSORT_UINT32,
    true,   // RADIXSORT_INT64,
    false,  // RADIXSORT_UINT64,
    true,   // RADIXSORT_FLOAT32
    true    // RADIXSORT_FLOAT64
};

/*lint -e647 -e679 Not truncations since data sizes are 4 or 8 */
/*lint -e744 We cover all cases */

static bool
radixsort_create_histograms(radixsort_t* sort, const void* input_raw, size_t count) {
	const radixsort_data_t data_type = sort->type;
	const size_t data_size = (data_type != RADIXSORT_CUSTOM) ? radixsort_data_size[data_type] : sort->custom_data_size;

	const unsigned char* loop = input_raw;
	const unsigned char* loop_end = loop + (count * data_size);
	const size_t indexsize = (size_t)sort->indextype;

	void* indices_raw = sort->indices[0];

	/*lint -e771 */
	// Histograms for all passes
	void* histogram_base[8];
	void** histogram_raw = histogram_base;
	if (data_size > 8)
		histogram_raw = memory_allocate(0, sizeof(void*) * data_size, 0, MEMORY_PERSISTENT);
	memset(sort->histogram, 0, 256 * data_size * indexsize);
	for (size_t ih = 0; ih < data_size; ++ih)
		histogram_raw[ih] = pointer_offset(sort->histogram, indexsize * (ih << 8));

	// Read values in previous sorted order and check if already sorted
	// Don't allow temporal coherence if increasing in size as it might introduce duplicate indices
	if ((count <= sort->lastused) && (data_type != RADIXSORT_CUSTOM)) {
		switch (data_type) {
			case RADIXSORT_INT32: {
				const int32_t* input = (const int32_t*)input_raw;

				int32_t val;
				int32_t prev_val = *input;

				if (indexsize == RADIXSORT_INDEX16) {
					uint16_t* indices = indices_raw;
					uint16_t* histogram[4] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3]};
					while (loop != loop_end) {
						uint16_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
#else
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				} else {
					uint32_t* indices = indices_raw;
					uint32_t* histogram[4] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3]};
					while (loop != loop_end) {
						uint32_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
#else
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				}

				break;
			}

			case RADIXSORT_UINT32: {
				const uint32_t* input = (const uint32_t*)input_raw;

				uint32_t val;
				uint32_t prev_val = *input;

				if (indexsize == RADIXSORT_INDEX16) {
					uint16_t* indices = indices_raw;
					uint16_t* histogram[4] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3]};
					while (loop != loop_end) {
						uint16_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
#else
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				} else {
					uint32_t* indices = indices_raw;
					uint32_t* histogram[4] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3]};
					while (loop != loop_end) {
						uint32_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
#else
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				}

				break;
			}

			case RADIXSORT_INT64: {
				const int64_t* input = (const int64_t*)input_raw;

				int64_t val;
				int64_t prev_val = *input;

				if (indexsize == RADIXSORT_INDEX16) {
					uint16_t* indices = indices_raw;
					uint16_t* histogram[8] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3],
					                          histogram_raw[4], histogram_raw[5], histogram_raw[6], histogram_raw[7]};
					while (loop != loop_end) {
						uint16_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[7][*loop++]);
#else
						++(histogram[7][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				} else {
					uint32_t* indices = indices_raw;
					uint32_t* histogram[8] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3],
					                          histogram_raw[4], histogram_raw[5], histogram_raw[6], histogram_raw[7]};
					while (loop != loop_end) {
						uint32_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[7][*loop++]);
#else
						++(histogram[7][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				}

				break;
			}

			case RADIXSORT_UINT64: {
				const uint64_t* input = (const uint64_t*)input_raw;

				uint64_t val;
				uint64_t prev_val = *input;

				if (indexsize == RADIXSORT_INDEX16) {
					uint16_t* indices = indices_raw;
					uint16_t* histogram[8] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3],
					                          histogram_raw[4], histogram_raw[5], histogram_raw[6], histogram_raw[7]};
					while (loop != loop_end) {
						uint16_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[7][*loop++]);
#else
						++(histogram[7][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				} else {
					uint32_t* indices = indices_raw;
					uint32_t* histogram[8] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3],
					                          histogram_raw[4], histogram_raw[5], histogram_raw[6], histogram_raw[7]};
					while (loop != loop_end) {
						uint32_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[7][*loop++]);
#else
						++(histogram[7][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				}

				break;
			}

			case RADIXSORT_FLOAT32: {
				const float32_t* input = (const float32_t*)input_raw;

				float32_t val;
				float32_t prev_val = *input;

				if (indexsize == RADIXSORT_INDEX16) {
					uint16_t* indices = indices_raw;
					uint16_t* histogram[4] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3]};
					while (loop != loop_end) {
						uint16_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
#else
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				} else {
					uint32_t* indices = indices_raw;
					uint32_t* histogram[4] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3]};
					while (loop != loop_end) {
						uint32_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
#else
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				}

				break;
			}

			case RADIXSORT_FLOAT64: {
				const float64_t* input = (const float64_t*)input_raw;

				float64_t val;
				float64_t prev_val = *input;

				if (indexsize == RADIXSORT_INDEX16) {
					uint16_t* indices = indices_raw;
					uint16_t* histogram[8] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3],
					                          histogram_raw[4], histogram_raw[5], histogram_raw[6], histogram_raw[7]};
					while (loop != loop_end) {
						uint16_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[7][*loop++]);
#else
						++(histogram[7][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				} else {
					uint32_t* indices = indices_raw;
					uint32_t* histogram[8] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3],
					                          histogram_raw[4], histogram_raw[5], histogram_raw[6], histogram_raw[7]};
					while (loop != loop_end) {
						uint32_t curindex = *indices++;
						if ((curindex >= count) || ((val = input[curindex]) < prev_val))
							break;
						prev_val = val;

#if FOUNDATION_ARCH_ENDIAN_LITTLE
						++(histogram[0][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[7][*loop++]);
#else
						++(histogram[7][*loop++]);
						++(histogram[6][*loop++]);
						++(histogram[5][*loop++]);
						++(histogram[4][*loop++]);
						++(histogram[3][*loop++]);
						++(histogram[2][*loop++]);
						++(histogram[1][*loop++]);
						++(histogram[0][*loop++]);
#endif
					}
				}

				break;
			}

			case RADIXSORT_CUSTOM:
			default:
				break;
		}
	}

	if (loop == loop_end)
		return true;

	if (count != sort->lastused) {
		if (indexsize == RADIXSORT_INDEX16) {
			uint16_t* indexarr[2] = {sort->indices[0], sort->indices[1]};
			for (size_t ih = 0; ih < count; ++ih) {
				indexarr[0][ih] = (uint16_t)ih;
				indexarr[1][ih] = (uint16_t)ih;
			}
		} else {
			uint32_t* indexarr[2] = {sort->indices[0], sort->indices[1]};
			for (size_t ih = 0; ih < count; ++ih) {
				indexarr[0][ih] = (uint32_t)ih;
				indexarr[1][ih] = (uint32_t)ih;
			}
		}
	}

	// Finish calculating the histograms, now without checks
	switch (data_size) {
		case 4:
			if (indexsize == RADIXSORT_INDEX16) {
				uint16_t* histogram[4] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3]};
				while (loop != loop_end) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
					++(histogram[0][*loop++]);
					++(histogram[1][*loop++]);
					++(histogram[2][*loop++]);
					++(histogram[3][*loop++]);
#else
					++(histogram[3][*loop++]);
					++(histogram[2][*loop++]);
					++(histogram[1][*loop++]);
					++(histogram[0][*loop++]);
#endif
				}
			} else {
				uint32_t* histogram[4] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3]};
				while (loop != loop_end) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
					++(histogram[0][*loop++]);
					++(histogram[1][*loop++]);
					++(histogram[2][*loop++]);
					++(histogram[3][*loop++]);
#else
					++(histogram[3][*loop++]);
					++(histogram[2][*loop++]);
					++(histogram[1][*loop++]);
					++(histogram[0][*loop++]);
#endif
				}
			}

			break;

		case 8:
			if (indexsize == RADIXSORT_INDEX16) {
				uint16_t* histogram[8] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3],
				                          histogram_raw[4], histogram_raw[5], histogram_raw[6], histogram_raw[7]};
				while (loop != loop_end) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
					++(histogram[0][*loop++]);
					++(histogram[1][*loop++]);
					++(histogram[2][*loop++]);
					++(histogram[3][*loop++]);
					++(histogram[4][*loop++]);
					++(histogram[5][*loop++]);
					++(histogram[6][*loop++]);
					++(histogram[7][*loop++]);
#else
					++(histogram[7][*loop++]);
					++(histogram[6][*loop++]);
					++(histogram[5][*loop++]);
					++(histogram[4][*loop++]);
					++(histogram[3][*loop++]);
					++(histogram[2][*loop++]);
					++(histogram[1][*loop++]);
					++(histogram[0][*loop++]);
#endif
				}
			} else {
				uint32_t* histogram[8] = {histogram_raw[0], histogram_raw[1], histogram_raw[2], histogram_raw[3],
				                          histogram_raw[4], histogram_raw[5], histogram_raw[6], histogram_raw[7]};
				while (loop != loop_end) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
					++(histogram[0][*loop++]);
					++(histogram[1][*loop++]);
					++(histogram[2][*loop++]);
					++(histogram[3][*loop++]);
					++(histogram[4][*loop++]);
					++(histogram[5][*loop++]);
					++(histogram[6][*loop++]);
					++(histogram[7][*loop++]);
#else
					++(histogram[7][*loop++]);
					++(histogram[6][*loop++]);
					++(histogram[5][*loop++]);
					++(histogram[4][*loop++]);
					++(histogram[3][*loop++]);
					++(histogram[2][*loop++]);
					++(histogram[1][*loop++]);
					++(histogram[0][*loop++]);
#endif
				}
			}

			break;

		default:
			if (indexsize == RADIXSORT_INDEX16) {
				while (loop != loop_end) {
					for (uint ibyte = 0; ibyte < data_size; ++ibyte) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
						uint16_t* histogram = (uint16_t*)histogram_raw[ibyte];
#else
						uint16_t* histogram = (uint16_t*)histogram_raw[data_size - (ibyte + 1)];
#endif
						++(histogram[*loop++]);
					}
				}
			} else {
				while (loop != loop_end) {
					for (uint ibyte = 0; ibyte < data_size; ++ibyte) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
						uint32_t* histogram = (uint32_t*)histogram_raw[ibyte];
#else
						uint32_t* histogram = (uint32_t*)histogram_raw[data_size - (ibyte + 1)];
#endif
						++(histogram[*loop++]);
					}
				}
			}

			break;
	}

	if (histogram_raw != histogram_base)
		memory_deallocate(histogram_raw);

	return false;
}

static const void*
radixsort_int_index16(radixsort_t* sort, const void* input, size_t count) {
	const radixsort_data_t data_type = sort->type;

	const unsigned int data_size =
	    (data_type != RADIXSORT_CUSTOM) ? radixsort_data_size[data_type] : (uint)sort->custom_data_size;
	const bool data_signed = (data_type != RADIXSORT_CUSTOM) ? radixsort_data_signed[data_type] : false;
	const unsigned int data_shift = radixsort_data_shift[data_type];
	const size_t indexsize = (size_t)sort->indextype;
	uint16_t negatives = 0;
	unsigned int ipass, ival;

	if (!count || radixsort_create_histograms(sort, input, count)) {
		// Already sorted
		return sort->indices[0];
	}

	if (data_signed) {
		// Number of negatives is the last 128 values in the MSB histogram
		//(last, since we deal with sytstem byte ordering in
		// radixsort_create_histograms)
		uint16_t* histogram = pointer_offset(sort->histogram, indexsize * ((data_size - 1) << 8));
		for (ival = 128; ival < 256; ++ival)
			negatives += histogram[ival];
	}

	// Radix sort, ipass is the pass number (LSB is first histogram since
	// radixsort_create_histograms takes system byte order into account)
	for (ipass = 0; ipass < data_size; ++ipass) {
		uint16_t* current_count = pointer_offset(sort->histogram, indexsize * (ipass << 8));
#if FOUNDATION_ARCH_ENDIAN_LITTLE
		unsigned int byteofs = ipass;
#else
		unsigned int byteofs = (data_size - (ipass + 1));
#endif
		const unsigned char* input_bytes = pointer_offset_const(input, byteofs);

		if (current_count[*input_bytes] != count) {
			if ((ipass != (data_size - 1)) || !data_signed) {
				// Unsigned data or only positive values
				uint16_t next = 0, prev = 0;
				uint16_t* offset = sort->offset;
				*offset++ = 0;
				for (ival = 1; ival < 256; ++ival, ++offset, ++current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}
			} else {
				// Signed data, both positive and negative values
				uint16_t next = 0, prev;
				uint16_t* offset = sort->offset;

				// First positive data comes after the negative data
				*offset++ = negatives;
				prev = negatives;
				for (ival = 1; ival < 128; ++ival, ++offset, ++current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}

				// Fix position for negative values
				++current_count;
				*offset++ = 0;
				prev = 0;
				for (ival = 129; ival < 256; ++ival, ++offset, ++current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}
			}

			{
				uint16_t* indices = sort->indices[0];
				uint16_t* indices_next = sort->indices[1];
				uint16_t* indices_end = indices + count;
				uint16_t* offset = sort->offset;

				if (data_type != RADIXSORT_CUSTOM) {
					do {
						uint16_t id = *indices++;
						indices_next[offset[input_bytes[id << data_shift]]++] = id;
					} while (indices != indices_end);
				} else {
					do {
						uint16_t id = *indices++;
						indices_next[offset[input_bytes[id * data_size]]++] = id;
					} while (indices != indices_end);
				}
			}

			// After this swap, the valid indices (most recent) are in
			// sort->indices[0]
			{
				uint16_t* swap = sort->indices[0];
				sort->indices[0] = sort->indices[1];
				sort->indices[1] = swap;
			}
		}
	}

	return sort->indices[0];
}

static const void*
radixsort_int_index32(radixsort_t* sort, const void* input, size_t count) {
	const radixsort_data_t data_type = sort->type;

	const unsigned int data_size =
	    (data_type != RADIXSORT_CUSTOM) ? radixsort_data_size[data_type] : (uint)sort->custom_data_size;
	const bool data_signed = (data_type != RADIXSORT_CUSTOM) ? radixsort_data_signed[data_type] : false;
	const unsigned int data_shift = radixsort_data_shift[data_type];
	const size_t indexsize = (size_t)sort->indextype;
	uint32_t negatives = 0;
	unsigned int ipass, ival;

	if (!count || radixsort_create_histograms(sort, input, count)) {
		// Already sorted
		return sort->indices[0];
	}

	if (data_signed) {
		// Number of negatives is the last 128 values in the MSB histogram
		//(last, since we deal with sytstem byte ordering in
		// radixsort_create_histograms)
		uint32_t* histogram = pointer_offset(sort->histogram, indexsize * ((data_size - 1) << 8));
		for (ival = 128; ival < 256; ++ival)
			negatives += histogram[ival];
	}

	// Radix sort, j is the pass number (LSB is first histogram since
	// radixsort_create_histograms takes system byte order into account)
	for (ipass = 0; ipass < data_size; ++ipass) {
		uint32_t* current_count = pointer_offset(sort->histogram, indexsize * (ipass << 8));
#if FOUNDATION_ARCH_ENDIAN_LITTLE
		unsigned int byteofs = ipass;
#else
		unsigned int byteofs = (data_size - (ipass + 1));
#endif
		const unsigned char* input_bytes = pointer_offset_const(input, byteofs);

		if (current_count[*input_bytes] != count) {
			if ((ipass != (data_size - 1)) || !data_signed) {
				// Unsigned data or only positive values
				uint32_t next = 0, prev = 0;
				uint32_t* offset = sort->offset;
				*offset++ = 0;
				for (ival = 1; ival < 256; ++ival, ++offset, ++current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}
			} else {
				// Signed data, both positive and negative values
				uint32_t next = 0, prev;
				uint32_t* offset = sort->offset;

				// First positive data comes after the negative data
				*offset++ = negatives;
				prev = negatives;
				for (ival = 1; ival < 128; ++ival, ++offset, ++current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}

				// Fix position for negative values
				++current_count;
				*offset++ = 0;
				prev = 0;
				for (ival = 129; ival < 256; ++ival, ++offset, ++current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}
			}

			{
				uint32_t* indices = sort->indices[0];
				uint32_t* indices_next = sort->indices[1];
				uint32_t* indices_end = indices + count;
				uint32_t* offset = sort->offset;

				if (data_type != RADIXSORT_CUSTOM) {
					do {
						uint32_t id = *indices++;
						indices_next[offset[input_bytes[id << data_shift]]++] = id;
					} while (indices != indices_end);
				} else {
					do {
						uint32_t id = *indices++;
						indices_next[offset[input_bytes[id * data_size]]++] = id;
					} while (indices != indices_end);
				}
			}

			// After this swap, the valid indices (most recent) are in
			// sort->indices[0]
			{
				uint32_t* swap = sort->indices[0];
				sort->indices[0] = sort->indices[1];
				sort->indices[1] = swap;
			}
		}
	}

	return sort->indices[0];
}

static const void*
radixsort_float_index16(radixsort_t* sort, const void* input, size_t count) {
	const radixsort_data_t data_type = sort->type;
	const unsigned int data_size = radixsort_data_size[data_type];
	const unsigned int data_shift = radixsort_data_shift[data_type];
	const size_t indexsize = (size_t)sort->indextype;

	if (!count || radixsort_create_histograms(sort, input, count))
		return sort->indices[0];  // Already sorted

	uint16_t negatives = 0;

	// Number of negatives is the last 128 values in the MSB histogram
	//(last, since we deal with system byte ordering in radixsort_create_histograms)
	uint16_t* histogram = pointer_offset(sort->histogram, indexsize * ((data_size - 1) << 8));
	for (unsigned int ihist = 128; ihist < 256; ++ihist)
		negatives += histogram[ihist];

	// Radix sort, j is the pass number (0 = LSB, 3/7 = MSB)
	for (unsigned int ipass = 0; ipass < data_size; ++ipass) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
		unsigned int byteofs = ipass;
#else
		unsigned int byteofs = (data_size - ipass - 1);
#endif
		const unsigned char* input_bytes = pointer_offset_const(input, byteofs);

		uint16_t* current_count = pointer_offset(sort->histogram, indexsize * (ipass << 8));
		if (ipass != (data_size - 1)) {
			if (current_count[*input_bytes] != count) {
				// Only positive values
				uint16_t next = 0, prev = 0;
				uint16_t* offset = sort->offset;
				*offset++ = 0;
				for (unsigned int ival = 1; ival < 256; ++ival, ++offset, ++current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}

				{
					uint16_t* indices = sort->indices[0];
					uint16_t* indices_next = sort->indices[1];
					uint16_t* indices_end = indices + count;

					offset = sort->offset;
					while (indices != indices_end) {
						uint16_t id = *indices++;
						indices_next[offset[input_bytes[id << data_shift]]++] = id;
					}
				}

				// After this swap, the valid indices (most recent) are in
				// sort->indices[0]
				{
					uint16_t* swap = sort->indices[0];
					sort->indices[0] = sort->indices[1];
					sort->indices[1] = swap;
				}
			}
		} else {
			unsigned char unique_val = *input_bytes;

			if (current_count[unique_val] != count) {
				// Both positive and negative values
				uint16_t next = 0, prev;
				uint16_t* offset = sort->offset;
				uint16_t* count_base = current_count;

				// First positive data comes after the negative data
				*offset++ = negatives;
				prev = negatives;
				for (unsigned int ival = 1; ival < 128; ++ival, ++offset, ++current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}

				// Reverse order for negative values
				offset = (uint16_t*)sort->offset + 255;
				current_count = count_base + 255;
				*offset-- = 0;
				prev = 0;
				for (unsigned int ival = 0; ival < 127; ++ival, --offset, --current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}

				// Fix position for negative values
				offset = (uint16_t*)sort->offset + 128;
				current_count = count_base + 128;
				for (unsigned int ival = 128; ival < 256; ++ival, ++offset, ++current_count)
					*offset += *current_count;

				// Perform Radix Sort
				uint16_t* indices = sort->indices[0];
				uint16_t* indices_next = sort->indices[1];
				offset = sort->offset;
				if (data_type == RADIXSORT_FLOAT32) {
					const uint32_t* input_uint = input;
					for (unsigned int ival = 0; ival < count; ++ival) {
						unsigned int radix = input_uint[indices[ival]] >> 24;
						if (radix < 128) {
							// Positive
							indices_next[offset[radix]++] = indices[ival];
						} else {
							// Negative, reverse order
							indices_next[--offset[radix]] = indices[ival];
						}
					}
				} else {  // if( data_type == RADIXSORT_FLOAT64 )
					const uint64_t* input_uint = input;
					for (unsigned int ival = 0; ival < count; ++ival) {
						unsigned int radix = (unsigned int)(input_uint[indices[ival]] >> 56ULL);
						if (radix < 128) {
							// Positive
							indices_next[offset[radix]++] = indices[ival];
						} else {
							// Negative, reverse order
							indices_next[--offset[radix]] = indices[ival];
						}
					}
				}

				// After this swap, the valid indices (most recent) are in
				// sort->indices[0]
				uint16_t* swap = sort->indices[0];
				sort->indices[0] = sort->indices[1];
				sort->indices[1] = swap;
			} else {
				// Reverse order if all values are negative
				if (unique_val >= 128) {
					uint16_t* indices = sort->indices[0];
					uint16_t* indices_next = sort->indices[1];
					for (unsigned int ival = 0; ival < count; ++ival)
						indices_next[ival] = indices[count - (ival + 1)];

					// After this swap, the valid indices (most recent) are in
					// sort->indices[0]
					uint16_t* swap = sort->indices[0];
					sort->indices[0] = sort->indices[1];
					sort->indices[1] = swap;
				}
			}
		}
	}

	return sort->indices[0];
}

static const void*
radixsort_float_index32(radixsort_t* sort, const void* input, size_t count) {
	const radixsort_data_t data_type = sort->type;
	const unsigned int data_size = radixsort_data_size[data_type];
	const unsigned int data_shift = radixsort_data_shift[data_type];
	const size_t indexsize = (size_t)sort->indextype;

	if (!count || radixsort_create_histograms(sort, input, count))
		return sort->indices[0];  // Already sorted

	uint32_t negatives = 0;

	// Number of negatives is the last 128 values in the MSB histogram
	//(last, since we deal with system byte ordering in radixsort_create_histograms)
	uint32_t* histogram = pointer_offset(sort->histogram, indexsize * ((data_size - 1) << 8));
	for (unsigned int ihist = 128; ihist < 256; ++ihist)
		negatives += histogram[ihist];

	// Radix sort, j is the pass number (0 = LSB, 3/7 = MSB)
	for (unsigned int ipass = 0; ipass < data_size; ++ipass) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
		unsigned int byteofs = ipass;
#else
		unsigned int byteofs = (data_size - ipass - 1);
#endif
		const unsigned char* input_bytes = pointer_offset_const(input, byteofs);

		uint32_t* current_count = pointer_offset(sort->histogram, indexsize * (ipass << 8));
		if (ipass != (data_size - 1)) {
			if (current_count[*input_bytes] != count) {
				// Only positive values
				uint32_t next = 0, prev = 0;
				uint32_t* offset = sort->offset;
				*offset++ = 0;
				for (unsigned int ival = 1; ival < 256; ++ival, ++offset, ++current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}

				{
					uint32_t* indices = sort->indices[0];
					uint32_t* indices_next = sort->indices[1];
					uint32_t* indices_end = indices + count;

					offset = sort->offset;
					while (indices != indices_end) {
						uint32_t id = *indices++;
						indices_next[offset[input_bytes[id << data_shift]]++] = id;
					}
				}

				// After this swap, the valid indices (most recent) are in
				// sort->indices[0]
				{
					uint32_t* swap = sort->indices[0];
					sort->indices[0] = sort->indices[1];
					sort->indices[1] = swap;
				}
			}
		} else {
			unsigned char unique_val = *input_bytes;

			if (current_count[unique_val] != count) {
				// Both positive and negative values
				uint32_t next = 0, prev;
				uint32_t* offset = sort->offset;
				uint32_t* count_base = current_count;

				// First positive data comes after the negative data
				*offset++ = negatives;
				prev = negatives;
				for (unsigned int ival = 1; ival < 128; ++ival, ++offset, ++current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}

				// Reverse order for negative values
				offset = (uint32_t*)sort->offset + 255;
				current_count = count_base + 255;
				*offset-- = 0;
				prev = 0;
				for (unsigned int ival = 0; ival < 127; ++ival, --offset, --current_count) {
					next = prev + *current_count;
					prev = next;
					*offset = next;
				}

				// Fix position for negative values
				offset = (uint32_t*)sort->offset + 128;
				current_count = count_base + 128;
				for (unsigned int ival = 128; ival < 256; ++ival, ++offset, ++current_count)
					*offset += *current_count;

				// Perform Radix Sort
				uint32_t* indices = sort->indices[0];
				uint32_t* indices_next = sort->indices[1];
				offset = sort->offset;
				if (data_type == RADIXSORT_FLOAT32) {
					const uint32_t* input_uint = input;
					for (unsigned int ival = 0; ival < count; ++ival) {
						unsigned int radix = input_uint[indices[ival]] >> 24;
						if (radix < 128) {
							// Positive
							indices_next[offset[radix]++] = indices[ival];
						} else {
							// Negative, reverse order
							indices_next[--offset[radix]] = indices[ival];
						}
					}
				} else {  // if( data_type == RADIXSORT_FLOAT64 )
					const uint64_t* input_uint = input;
					for (unsigned int ival = 0; ival < count; ++ival) {
						unsigned int radix = (unsigned int)(input_uint[indices[ival]] >> 56ULL);
						if (radix < 128) {
							// Positive
							indices_next[offset[radix]++] = indices[ival];
						} else {
							// Negative, reverse order
							indices_next[--offset[radix]] = indices[ival];
						}
					}
				}

				// After this swap, the valid indices (most recent) are in
				// sort->indices[0]
				uint32_t* swap = sort->indices[0];
				sort->indices[0] = sort->indices[1];
				sort->indices[1] = swap;
			} else {
				// Reverse order if all values are negative
				if (unique_val >= 128) {
					uint32_t* indices = sort->indices[0];
					uint32_t* indices_next = sort->indices[1];
					for (unsigned int ival = 0; ival < count; ++ival)
						indices_next[ival] = indices[count - (ival + 1)];

					// After this swap, the valid indices (most recent) are in
					// sort->indices[0]
					uint32_t* swap = sort->indices[0];
					sort->indices[0] = sort->indices[1];
					sort->indices[1] = swap;
				}
			}
		}
	}

	return sort->indices[0];
}

const void*
radixsort_sort(radixsort_t* sort, const void* input, size_t count) {
	const radixsort_data_t data_type = sort->type;

	FOUNDATION_ASSERT(count <= sort->size);
	if (count > sort->size)
		count = sort->size;

	const void* result = nullptr;
	if ((data_type == RADIXSORT_FLOAT32) || (data_type == RADIXSORT_FLOAT64)) {
		if (sort->indextype == RADIXSORT_INDEX16)
			result = radixsort_float_index16(sort, input, count);
		else
			result = radixsort_float_index32(sort, input, count);
	} else {
		if (sort->indextype == RADIXSORT_INDEX16)
			result = radixsort_int_index16(sort, input, count);
		else
			result = radixsort_int_index32(sort, input, count);
	}

	sort->lastused = count;

	return result;
}

radixsort_t*
radixsort_allocate_custom(size_t data_size, size_t count) {
	radixsort_t* sort;
	radixsort_indextype_t indextype = RADIXSORT_INDEX16;
	if (count > 0xFFFF)
		indextype = RADIXSORT_INDEX32;

	size_t indexsize = (size_t)indextype;
	sort = memory_allocate(0,
	                       sizeof(radixsort_t) +
	                           /* 2 index tables */ (2 * indexsize * count) +
	                           /* histograms */ (256 * data_size * indexsize) +
	                           /* offset table */ (256 * indexsize),
	                       0, MEMORY_PERSISTENT);
	sort->custom_data_size = data_size;
	sort->indices[0] = pointer_offset(sort, sizeof(radixsort_t));
	sort->indices[1] = pointer_offset(sort->indices[0], indexsize * count);
	sort->histogram = pointer_offset(sort->indices[1], indexsize * count);
	sort->offset = pointer_offset(sort->histogram, indexsize * 256 * data_size);

	radixsort_initialize_custom(sort, data_size, count);

	return sort;
}

radixsort_t*
radixsort_allocate(radixsort_data_t type, size_t count) {
	radixsort_t* sort;
	radixsort_indextype_t indextype = RADIXSORT_INDEX16;
	if (count > 0xFFFF)
		indextype = RADIXSORT_INDEX32;

	size_t indexsize = (size_t)indextype;
	sort = memory_allocate(0,
	                       sizeof(radixsort_t) +
	                           /* 2 index tables */ (2 * indexsize * count) +
	                           /* histograms */ (256 * radixsort_data_size[type] * indexsize) +
	                           /* offset table */ (256 * indexsize),
	                       0, MEMORY_PERSISTENT);
	sort->indices[0] = pointer_offset(sort, sizeof(radixsort_t));
	sort->indices[1] = pointer_offset(sort->indices[0], indexsize * count);
	sort->histogram = pointer_offset(sort->indices[1], indexsize * count);
	sort->offset = pointer_offset(sort->histogram, indexsize * 256 * radixsort_data_size[type]);

	radixsort_initialize(sort, type, count);

	return sort;
}

void
radixsort_initialize_custom(radixsort_t* sort, size_t data_size, size_t count) {
	sort->type = RADIXSORT_CUSTOM;
	sort->size = count;
	sort->lastused = count;
	sort->custom_data_size = data_size;

	FOUNDATION_ASSERT(count <= 0xFFFFFFFF);
	if (count >= 0xFFFFFFFF)
		count = 0xFFFFFFFE;

	if (count > 0xFFFF) {
		uint32_t* indices[2] = {sort->indices[0], sort->indices[1]};
		sort->indextype = RADIXSORT_INDEX32;
		for (uint32_t i = 0; i < count; ++i) {
			indices[0][i] = i;
			indices[1][i] = i;
		}
	} else {
		uint16_t* indices[2] = {sort->indices[0], sort->indices[1]};
		sort->indextype = RADIXSORT_INDEX16;
		for (uint32_t i = 0; i < count; ++i) {
			indices[0][i] = (uint16_t)i;
			indices[1][i] = (uint16_t)i;
		}
	}
}

void
radixsort_initialize(radixsort_t* sort, radixsort_data_t type, size_t count) {
	sort->type = type;
	sort->size = count;
	sort->lastused = count;
	sort->custom_data_size = radixsort_data_size[type];

	FOUNDATION_ASSERT(count <= 0xFFFFFFFF);
	if (count >= 0xFFFFFFFF)
		count = 0xFFFFFFFE;

	if (count > 0xFFFF) {
		uint32_t* indices[2] = {sort->indices[0], sort->indices[1]};
		sort->indextype = RADIXSORT_INDEX32;
		for (uint32_t i = 0; i < count; ++i) {
			indices[0][i] = i;
			indices[1][i] = i;
		}
	} else {
		uint16_t* indices[2] = {sort->indices[0], sort->indices[1]};
		sort->indextype = RADIXSORT_INDEX16;
		for (uint32_t i = 0; i < count; ++i) {
			indices[0][i] = (uint16_t)i;
			indices[1][i] = (uint16_t)i;
		}
	}
}

void
radixsort_deallocate(radixsort_t* sort) {
	radixsort_finalize(sort);
	memory_deallocate(sort);
}

void
radixsort_finalize(radixsort_t* sort) {
	FOUNDATION_UNUSED(sort);
}
