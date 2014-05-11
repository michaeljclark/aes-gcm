/*
 * One-key CBC MAC (OMAC1) hash with AES-128
 *
 * Copyright (c) 2003-2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "aes.h"

static void gf_mulx(aes_uchar *pad)
{
	int i, carry;

	carry = pad[0] & 0x80;
	for (i = 0; i < AES_BLOCK_SIZE - 1; i++)
		pad[i] = (pad[i] << 1) | (pad[i + 1] >> 7);
	pad[AES_BLOCK_SIZE - 1] <<= 1;
	if (carry)
		pad[AES_BLOCK_SIZE - 1] ^= 0x87;
}


/**
 * omac1_aes_128_vector - One-Key CBC MAC (OMAC1) hash with AES-128
 * @key: 128-bit key for the hash operation
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for MAC (128 bits, i.e., 16 bytes)
 * Returns: 0 on success, -1 on failure
 *
 * This is a mode for using block cipher (AES in this case) for authentication.
 * OMAC1 was standardized with the name CMAC by NIST in a Special Publication
 * (SP) 800-38B.
 */
int omac1_aes_128_vector(const aes_uchar *key, size_t num_elem,
			 const aes_uchar *addr[], const size_t *len, aes_uchar *mac)
{
	void *ctx;
	aes_uchar cbc[AES_BLOCK_SIZE], pad[AES_BLOCK_SIZE];
	const aes_uchar *pos, *end;
	size_t i, e, left, total_len;

	ctx = aes_encrypt_init(key, 16);
	if (ctx == NULL)
		return -1;
	memset(cbc, 0, AES_BLOCK_SIZE);

	total_len = 0;
	for (e = 0; e < num_elem; e++)
		total_len += len[e];
	left = total_len;

	e = 0;
	pos = addr[0];
	end = pos + len[0];

	while (left >= AES_BLOCK_SIZE) {
		for (i = 0; i < AES_BLOCK_SIZE; i++) {
			cbc[i] ^= *pos++;
			if (pos >= end) {
				e++;
				pos = addr[e];
				end = pos + len[e];
			}
		}
		if (left > AES_BLOCK_SIZE)
			aes_encrypt(ctx, cbc, cbc);
		left -= AES_BLOCK_SIZE;
	}

	memset(pad, 0, AES_BLOCK_SIZE);
	aes_encrypt(ctx, pad, pad);
	gf_mulx(pad);

	if (left || total_len == 0) {
		for (i = 0; i < left; i++) {
			cbc[i] ^= *pos++;
			if (pos >= end) {
				e++;
				pos = addr[e];
				end = pos + len[e];
			}
		}
		cbc[left] ^= 0x80;
		gf_mulx(pad);
	}

	for (i = 0; i < AES_BLOCK_SIZE; i++)
		pad[i] ^= cbc[i];
	aes_encrypt(ctx, pad, mac);
	aes_encrypt_deinit(ctx);
	return 0;
}


/**
 * omac1_aes_128 - One-Key CBC MAC (OMAC1) hash with AES-128 (aka AES-CMAC)
 * @key: 128-bit key for the hash operation
 * @data: Data buffer for which a MAC is determined
 * @data_len: Length of data buffer in bytes
 * @mac: Buffer for MAC (128 bits, i.e., 16 bytes)
 * Returns: 0 on success, -1 on failure
 *
 * This is a mode for using block cipher (AES in this case) for authentication.
 * OMAC1 was standardized with the name CMAC by NIST in a Special Publication
 * (SP) 800-38B.
 */
int omac1_aes_128(const aes_uchar *key, const aes_uchar *data, size_t data_len, aes_uchar *mac)
{
	return omac1_aes_128_vector(key, 1, &data, &data_len, mac);
}
