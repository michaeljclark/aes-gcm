/*
 * Galois/Counter Mode (GCM) and GMAC with AES
 *
 * Copyright (c) 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "aes.h"

static void inc32(aes_uchar *block)
{
	aes_uint val;
	val = AES_GET_BE32(block + AES_BLOCK_SIZE - 4);
	val++;
	AES_PUT_BE32(block + AES_BLOCK_SIZE - 4, val);
}


static void xor_block(aes_uchar *dst, const aes_uchar *src)
{
	aes_uint *d = (aes_uint *) dst;
	aes_uint *s = (aes_uint *) src;
	*d++ ^= *s++;
	*d++ ^= *s++;
	*d++ ^= *s++;
	*d++ ^= *s++;
}


static void shift_right_block(aes_uchar *v)
{
	aes_uint val;

	val = AES_GET_BE32(v + 12);
	val >>= 1;
	if (v[11] & 0x01)
		val |= 0x80000000;
	AES_PUT_BE32(v + 12, val);

	val = AES_GET_BE32(v + 8);
	val >>= 1;
	if (v[7] & 0x01)
		val |= 0x80000000;
	AES_PUT_BE32(v + 8, val);

	val = AES_GET_BE32(v + 4);
	val >>= 1;
	if (v[3] & 0x01)
		val |= 0x80000000;
	AES_PUT_BE32(v + 4, val);

	val = AES_GET_BE32(v);
	val >>= 1;
	AES_PUT_BE32(v, val);
}


/* Multiplication in GF(2^128) */
static void gf_mult(const aes_uchar *x, const aes_uchar *y, aes_uchar *z)
{
	aes_uchar v[16];
	int i, j;

	memset(z, 0, 16); /* Z_0 = 0^128 */
	memcpy(v, y, 16); /* V_0 = Y */

	for (i = 0; i < 16; i++) {
		for (j = 0; j < 8; j++) {
			if (x[i] & 1 << (7 - j)) {
				/* Z_(i + 1) = Z_i XOR V_i */
				xor_block(z, v);
			} else {
				/* Z_(i + 1) = Z_i */
			}

			if (v[15] & 0x01) {
				/* V_(i + 1) = (V_i >> 1) XOR R */
				shift_right_block(v);
				/* R = 11100001 || 0^120 */
				v[0] ^= 0xe1;
			} else {
				/* V_(i + 1) = V_i >> 1 */
				shift_right_block(v);
			}
		}
	}
}


static void ghash_start(aes_uchar *y)
{
	/* Y_0 = 0^128 */
	memset(y, 0, 16);
}


static void ghash(const aes_uchar *h, const aes_uchar *x, size_t xlen, aes_uchar *y)
{
	size_t m, i;
	const aes_uchar *xpos = x;
	aes_uchar tmp[16];

	m = xlen / 16;

	for (i = 0; i < m; i++) {
		/* Y_i = (Y^(i-1) XOR X_i) dot H */
		xor_block(y, xpos);
		xpos += 16;

		/* dot operation:
		 * multiplication operation for binary Galois (finite) field of
		 * 2^128 elements */
		gf_mult(y, h, tmp);
		memcpy(y, tmp, 16);
	}

	if (x + xlen > xpos) {
		/* Add zero padded last block */
		size_t last = x + xlen - xpos;
		memcpy(tmp, xpos, last);
		memset(tmp + last, 0, sizeof(tmp) - last);

		/* Y_i = (Y^(i-1) XOR X_i) dot H */
		xor_block(y, tmp);

		/* dot operation:
		 * multiplication operation for binary Galois (finite) field of
		 * 2^128 elements */
		gf_mult(y, h, tmp);
		memcpy(y, tmp, 16);
	}

	/* Return Y_m */
}


static void aes_gctr(void *aes, const aes_uchar *icb, const aes_uchar *x, size_t xlen, aes_uchar *y)
{
	size_t i, n, last;
	aes_uchar cb[AES_BLOCK_SIZE], tmp[AES_BLOCK_SIZE];
	const aes_uchar *xpos = x;
	aes_uchar *ypos = y;

	if (xlen == 0)
		return;

	n = xlen / 16;

	memcpy(cb, icb, AES_BLOCK_SIZE);
	/* Full blocks */
	for (i = 0; i < n; i++) {
		aes_encrypt(aes, cb, ypos);
		xor_block(ypos, xpos);
		xpos += AES_BLOCK_SIZE;
		ypos += AES_BLOCK_SIZE;
		inc32(cb);
	}

	last = x + xlen - xpos;
	if (last) {
		/* Last, partial block */
		aes_encrypt(aes, cb, tmp);
		for (i = 0; i < last; i++)
			*ypos++ = *xpos++ ^ tmp[i];
	}
}


static void * aes_gcm_init_hash_subkey(const aes_uchar *key, size_t key_len, aes_uchar *H)
{
	void *aes;

	aes = aes_encrypt_init(key, key_len);
	if (aes == NULL)
		return NULL;

	/* Generate hash subkey H = AES_K(0^128) */
	memset(H, 0, AES_BLOCK_SIZE);
	aes_encrypt(aes, H, H);
	aes_hexdump_key(MSG_EXCESSIVE, "Hash subkey H for GHASH", H, AES_BLOCK_SIZE);
	return aes;
}


static void aes_gcm_prepare_j0(const aes_uchar *iv, size_t iv_len, const aes_uchar *H, aes_uchar *J0)
{
	aes_uchar len_buf[16];

	if (iv_len == 12) {
		/* Prepare block J_0 = IV || 0^31 || 1 [len(IV) = 96] */
		memcpy(J0, iv, iv_len);
		memset(J0 + iv_len, 0, AES_BLOCK_SIZE - iv_len);
		J0[AES_BLOCK_SIZE - 1] = 0x01;
	} else {
		/*
		 * s = 128 * ceil(len(IV)/128) - len(IV)
		 * J_0 = GHASH_H(IV || 0^(s+64) || [len(IV)]_64)
		 */
		ghash_start(J0);
		ghash(H, iv, iv_len, J0);
		AES_PUT_BE64(len_buf, 0);
		AES_PUT_BE64(len_buf + 8, iv_len * 8);
		ghash(H, len_buf, sizeof(len_buf), J0);
	}
}


static void aes_gcm_gctr(void *aes, const aes_uchar *J0, const aes_uchar *in, size_t len,
			 aes_uchar *out)
{
	aes_uchar J0inc[AES_BLOCK_SIZE];

	if (len == 0)
		return;

	memcpy(J0inc, J0, AES_BLOCK_SIZE);
	inc32(J0inc);
	aes_gctr(aes, J0inc, in, len, out);
}


static void aes_gcm_ghash(const aes_uchar *H, const aes_uchar *aad, size_t aad_len,
			  const aes_uchar *crypt, size_t crypt_len, aes_uchar *S)
{
	aes_uchar len_buf[16];

	/*
	 * u = 128 * ceil[len(C)/128] - len(C)
	 * v = 128 * ceil[len(A)/128] - len(A)
	 * S = GHASH_H(A || 0^v || C || 0^u || [len(A)]64 || [len(C)]64)
	 * (i.e., zero padded to block size A || C and lengths of each in bits)
	 */
	ghash_start(S);
	ghash(H, aad, aad_len, S);
	ghash(H, crypt, crypt_len, S);
	AES_PUT_BE64(len_buf, aad_len * 8);
	AES_PUT_BE64(len_buf + 8, crypt_len * 8);
	ghash(H, len_buf, sizeof(len_buf), S);

	aes_hexdump_key(MSG_EXCESSIVE, "S = GHASH_H(...)", S, 16);
}


/**
 * aes_gcm_ae - GCM-AE_K(IV, P, A)
 */
int aes_gcm_ae(const aes_uchar *key, size_t key_len, const aes_uchar *iv, size_t iv_len,
	       const aes_uchar *plain, size_t plain_len,
	       const aes_uchar *aad, size_t aad_len, aes_uchar *crypt, aes_uchar *tag)
{
	aes_uchar H[AES_BLOCK_SIZE];
	aes_uchar J0[AES_BLOCK_SIZE];
	aes_uchar S[16];
	void *aes;

	aes = aes_gcm_init_hash_subkey(key, key_len, H);
	if (aes == NULL)
		return -1;

	aes_gcm_prepare_j0(iv, iv_len, H, J0);

	/* C = GCTR_K(inc_32(J_0), P) */
	aes_gcm_gctr(aes, J0, plain, plain_len, crypt);

	aes_gcm_ghash(H, aad, aad_len, crypt, plain_len, S);

	/* T = MSB_t(GCTR_K(J_0, S)) */
	aes_gctr(aes, J0, S, sizeof(S), tag);

	/* Return (C, T) */

	aes_encrypt_deinit(aes);

	return 0;
}


/**
 * aes_gcm_ad - GCM-AD_K(IV, C, A, T)
 */
int aes_gcm_ad(const aes_uchar *key, size_t key_len, const aes_uchar *iv, size_t iv_len,
	       const aes_uchar *crypt, size_t crypt_len,
	       const aes_uchar *aad, size_t aad_len, const aes_uchar *tag, aes_uchar *plain)
{
	aes_uchar H[AES_BLOCK_SIZE];
	aes_uchar J0[AES_BLOCK_SIZE];
	aes_uchar S[16], T[16];
	void *aes;

	aes = aes_gcm_init_hash_subkey(key, key_len, H);
	if (aes == NULL)
		return -1;

	aes_gcm_prepare_j0(iv, iv_len, H, J0);

	/* P = GCTR_K(inc_32(J_0), C) */
	aes_gcm_gctr(aes, J0, crypt, crypt_len, plain);

	aes_gcm_ghash(H, aad, aad_len, crypt, crypt_len, S);

	/* T' = MSB_t(GCTR_K(J_0, S)) */
	aes_gctr(aes, J0, S, sizeof(S), T);

	aes_encrypt_deinit(aes);

	if (memcmp(tag, T, 16) != 0) {
		aes_printf(MSG_EXCESSIVE, "GCM: Tag mismatch");
		return -1;
	}

	return 0;
}


int aes_gmac(const aes_uchar *key, size_t key_len, const aes_uchar *iv, size_t iv_len,
	     const aes_uchar *aad, size_t aad_len, aes_uchar *tag)
{
	return aes_gcm_ae(key, key_len, iv, iv_len, NULL, 0, aad, aad_len, NULL, tag);
}
