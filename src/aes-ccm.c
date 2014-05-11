/*
 * Counter with CBC-MAC (CCM) with AES
 *
 * Copyright (c) 2010-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "aes.h"


static void xor_aes_block(aes_uchar *dst, const aes_uchar *src)
{
	aes_uint *d = (aes_uint *) dst;
	aes_uint *s = (aes_uint *) src;
	*d++ ^= *s++;
	*d++ ^= *s++;
	*d++ ^= *s++;
	*d++ ^= *s++;
}


static void aes_ccm_auth_start(void *aes, size_t M, size_t L, const aes_uchar *nonce,
			       const aes_uchar *aad, size_t aad_len, size_t plain_len,
			       aes_uchar *x)
{
	aes_uchar aad_buf[2 * AES_BLOCK_SIZE];
	aes_uchar b[AES_BLOCK_SIZE];

	/* Authentication */
	/* B_0: Flags | Nonce N | l(m) */
	b[0] = aad_len ? 0x40 : 0 /* Adata */;
	b[0] |= (((M - 2) / 2) /* M' */ << 3);
	b[0] |= (L - 1) /* L' */;
	memcpy(&b[1], nonce, 15 - L);
	AES_PUT_BE16(&b[AES_BLOCK_SIZE - L], plain_len);

	aes_hexdump_key(MSG_EXCESSIVE, "CCM B_0", b, AES_BLOCK_SIZE);
	aes_encrypt(aes, b, x); /* X_1 = E(K, B_0) */

	if (!aad_len)
		return;

	AES_PUT_BE16(aad_buf, aad_len);
	memcpy(aad_buf + 2, aad, aad_len);
	memset(aad_buf + 2 + aad_len, 0, sizeof(aad_buf) - 2 - aad_len);

	xor_aes_block(aad_buf, x);
	aes_encrypt(aes, aad_buf, x); /* X_2 = E(K, X_1 XOR B_1) */

	if (aad_len > AES_BLOCK_SIZE - 2) {
		xor_aes_block(&aad_buf[AES_BLOCK_SIZE], x);
		/* X_3 = E(K, X_2 XOR B_2) */
		aes_encrypt(aes, &aad_buf[AES_BLOCK_SIZE], x);
	}
}


static void aes_ccm_auth(void *aes, const aes_uchar *data, size_t len, aes_uchar *x)
{
	size_t last = len % AES_BLOCK_SIZE;
	size_t i;

	for (i = 0; i < len / AES_BLOCK_SIZE; i++) {
		/* X_i+1 = E(K, X_i XOR B_i) */
		xor_aes_block(x, data);
		data += AES_BLOCK_SIZE;
		aes_encrypt(aes, x, x);
	}
	if (last) {
		/* XOR zero-padded last block */
		for (i = 0; i < last; i++)
			x[i] ^= *data++;
		aes_encrypt(aes, x, x);
	}
}


static void aes_ccm_encr_start(size_t L, const aes_uchar *nonce, aes_uchar *a)
{
	/* A_i = Flags | Nonce N | Counter i */
	a[0] = L - 1; /* Flags = L' */
	memcpy(&a[1], nonce, 15 - L);
}


static void aes_ccm_encr(void *aes, size_t L, const aes_uchar *in, size_t len, aes_uchar *out,
			 aes_uchar *a)
{
	size_t last = len % AES_BLOCK_SIZE;
	size_t i;

	/* crypt = msg XOR (S_1 | S_2 | ... | S_n) */
	for (i = 1; i <= len / AES_BLOCK_SIZE; i++) {
		AES_PUT_BE16(&a[AES_BLOCK_SIZE - 2], i);
		/* S_i = E(K, A_i) */
		aes_encrypt(aes, a, out);
		xor_aes_block(out, in);
		out += AES_BLOCK_SIZE;
		in += AES_BLOCK_SIZE;
	}
	if (last) {
		AES_PUT_BE16(&a[AES_BLOCK_SIZE - 2], i);
		aes_encrypt(aes, a, out);
		/* XOR zero-padded last block */
		for (i = 0; i < last; i++)
			*out++ ^= *in++;
	}
}


static void aes_ccm_encr_auth(void *aes, size_t M, aes_uchar *x, aes_uchar *a, aes_uchar *auth)
{
	size_t i;
	aes_uchar tmp[AES_BLOCK_SIZE];

	aes_hexdump_key(MSG_EXCESSIVE, "CCM T", x, M);
	/* U = T XOR S_0; S_0 = E(K, A_0) */
	AES_PUT_BE16(&a[AES_BLOCK_SIZE - 2], 0);
	aes_encrypt(aes, a, tmp);
	for (i = 0; i < M; i++)
		auth[i] = x[i] ^ tmp[i];
	aes_hexdump_key(MSG_EXCESSIVE, "CCM U", auth, M);
}


static void aes_ccm_decr_auth(void *aes, size_t M, aes_uchar *a, const aes_uchar *auth, aes_uchar *t)
{
	size_t i;
	aes_uchar tmp[AES_BLOCK_SIZE];

	aes_hexdump_key(MSG_EXCESSIVE, "CCM U", auth, M);
	/* U = T XOR S_0; S_0 = E(K, A_0) */
	AES_PUT_BE16(&a[AES_BLOCK_SIZE - 2], 0);
	aes_encrypt(aes, a, tmp);
	for (i = 0; i < M; i++)
		t[i] = auth[i] ^ tmp[i];
	aes_hexdump_key(MSG_EXCESSIVE, "CCM T", t, M);
}


/* AES-CCM with fixed L=2 and aad_len <= 30 assumption */
int aes_ccm_ae(const aes_uchar *key, size_t key_len, const aes_uchar *nonce,
	       size_t M, const aes_uchar *plain, size_t plain_len,
	       const aes_uchar *aad, size_t aad_len, aes_uchar *crypt, aes_uchar *auth)
{
	const size_t L = 2;
	void *aes;
	aes_uchar x[AES_BLOCK_SIZE], a[AES_BLOCK_SIZE];

	if (aad_len > 30 || M > AES_BLOCK_SIZE)
		return -1;

	aes = aes_encrypt_init(key, key_len);
	if (aes == NULL)
		return -1;

	aes_ccm_auth_start(aes, M, L, nonce, aad, aad_len, plain_len, x);
	aes_ccm_auth(aes, plain, plain_len, x);

	/* Encryption */
	aes_ccm_encr_start(L, nonce, a);
	aes_ccm_encr(aes, L, plain, plain_len, crypt, a);
	aes_ccm_encr_auth(aes, M, x, a, auth);

	aes_encrypt_deinit(aes);

	return 0;
}


/* AES-CCM with fixed L=2 and aad_len <= 30 assumption */
int aes_ccm_ad(const aes_uchar *key, size_t key_len, const aes_uchar *nonce,
	       size_t M, const aes_uchar *crypt, size_t crypt_len,
	       const aes_uchar *aad, size_t aad_len, const aes_uchar *auth, aes_uchar *plain)
{
	const size_t L = 2;
	void *aes;
	aes_uchar x[AES_BLOCK_SIZE], a[AES_BLOCK_SIZE];
	aes_uchar t[AES_BLOCK_SIZE];

	if (aad_len > 30 || M > AES_BLOCK_SIZE)
		return -1;

	aes = aes_encrypt_init(key, key_len);
	if (aes == NULL)
		return -1;

	/* Decryption */
	aes_ccm_encr_start(L, nonce, a);
	aes_ccm_decr_auth(aes, M, a, auth, t);

	/* plaintext = msg XOR (S_1 | S_2 | ... | S_n) */
	aes_ccm_encr(aes, L, crypt, crypt_len, plain, a);

	aes_ccm_auth_start(aes, M, L, nonce, aad, aad_len, crypt_len, x);
	aes_ccm_auth(aes, plain, crypt_len, x);

	aes_encrypt_deinit(aes);

	if (memcmp(x, t, M) != 0) {
		aes_printf(MSG_EXCESSIVE, "CCM: Auth mismatch");
		return -1;
	}

	return 0;
}
