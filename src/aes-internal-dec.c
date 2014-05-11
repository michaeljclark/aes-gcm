/*
 * AES (Rijndael) cipher - decrypt
 *
 * Modifications to public domain implementation:
 * - cleanup
 * - use C pre-processor to make it easier to change S table access
 * - added option (AES_SMALL_TABLES) for reducing code size by about 8 kB at
 *   cost of reduced throughput (quite small difference on Pentium 4,
 *   10-25% when using -O1 or -O2 optimization)
 *
 * Copyright (c) 2003-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "aes.h"
#include "aes-internal.h"

/**
 * Expand the cipher key into the decryption key schedule.
 *
 * @return	the number of rounds for the given cipher key size.
 */
int aes_rijndael_key_setup_dec(aes_uint rk[], const aes_uchar cipherKey[], size_t keyBits)
{
	int Nr, i, j;
	aes_uint temp;

	/* expand the cipher key: */
	Nr = aes_rijndael_key_setup_enc(rk, cipherKey, keyBits);
	if (Nr < 0)
		return Nr;
	/* invert the order of the round keys: */
	for (i = 0, j = 4*Nr; i < j; i += 4, j -= 4) {
		temp = rk[i    ]; rk[i    ] = rk[j    ]; rk[j    ] = temp;
		temp = rk[i + 1]; rk[i + 1] = rk[j + 1]; rk[j + 1] = temp;
		temp = rk[i + 2]; rk[i + 2] = rk[j + 2]; rk[j + 2] = temp;
		temp = rk[i + 3]; rk[i + 3] = rk[j + 3]; rk[j + 3] = temp;
	}
	/* apply the inverse MixColumn transform to all round keys but the
	 * first and the last: */
	for (i = 1; i < Nr; i++) {
		rk += 4;
		for (j = 0; j < 4; j++) {
			rk[j] = TD0_(TE4((rk[j] >> 24)       )) ^
				TD1_(TE4((rk[j] >> 16) & 0xff)) ^
				TD2_(TE4((rk[j] >>  8) & 0xff)) ^
				TD3_(TE4((rk[j]      ) & 0xff));
		}
	}

	return Nr;
}

void aes_rijndael_decrypt(const aes_uint rk[], int Nr, const aes_uchar ct[16], aes_uchar pt[16])
{
	aes_uint s0, s1, s2, s3, t0, t1, t2, t3;
#ifndef AES_FULL_UNROLL
	int r;
#endif /* AES_FULL_UNROLL */

	/*
	 * map byte array block to cipher state
	 * and add initial round key:
	 */
	s0 = GETU32(ct     ) ^ rk[0];
	s1 = GETU32(ct +  4) ^ rk[1];
	s2 = GETU32(ct +  8) ^ rk[2];
	s3 = GETU32(ct + 12) ^ rk[3];

#define ROUND(i,d,s) \
d##0 = TD0(s##0) ^ TD1(s##3) ^ TD2(s##2) ^ TD3(s##1) ^ rk[4 * i]; \
d##1 = TD0(s##1) ^ TD1(s##0) ^ TD2(s##3) ^ TD3(s##2) ^ rk[4 * i + 1]; \
d##2 = TD0(s##2) ^ TD1(s##1) ^ TD2(s##0) ^ TD3(s##3) ^ rk[4 * i + 2]; \
d##3 = TD0(s##3) ^ TD1(s##2) ^ TD2(s##1) ^ TD3(s##0) ^ rk[4 * i + 3]

#ifdef AES_FULL_UNROLL

	ROUND(1,t,s);
	ROUND(2,s,t);
	ROUND(3,t,s);
	ROUND(4,s,t);
	ROUND(5,t,s);
	ROUND(6,s,t);
	ROUND(7,t,s);
	ROUND(8,s,t);
	ROUND(9,t,s);
	if (Nr > 10) {
		ROUND(10,s,t);
		ROUND(11,t,s);
		if (Nr > 12) {
			ROUND(12,s,t);
			ROUND(13,t,s);
		}
	}

	rk += Nr << 2;

#else

	/* Nr - 1 full rounds: */
	r = Nr >> 1;
	for (;;) {
		ROUND(1,t,s);
		rk += 8;
		if (--r == 0)
			break;
		ROUND(0,s,t);
	}

#endif /* AES_FULL_UNROLL */

#undef ROUND

	/*
	 * apply last round and
	 * map cipher state to byte array block:
	 */
	s0 = TD41(t0) ^ TD42(t3) ^ TD43(t2) ^ TD44(t1) ^ rk[0];
	PUTU32(pt     , s0);
	s1 = TD41(t1) ^ TD42(t0) ^ TD43(t3) ^ TD44(t2) ^ rk[1];
	PUTU32(pt +  4, s1);
	s2 = TD41(t2) ^ TD42(t1) ^ TD43(t0) ^ TD44(t3) ^ rk[2];
	PUTU32(pt +  8, s2);
	s3 = TD41(t3) ^ TD42(t2) ^ TD43(t1) ^ TD44(t0) ^ rk[3];
	PUTU32(pt + 12, s3);
}


/* AES decrypt interface */

void * aes_decrypt_init(const aes_uchar *key, size_t len)
{
	aes_uint *rk;
	int res;
	rk = malloc(AES_PRIV_SIZE);
	if (rk == NULL)
		return NULL;
	res = aes_rijndael_key_setup_dec(rk, key, len * 8);
	if (res < 0) {
		free(rk);
		return NULL;
	}
	rk[AES_PRIV_NR_POS] = res;
	return rk;
}


void aes_decrypt(void *ctx, const aes_uchar *crypt, aes_uchar *plain)
{
	aes_uint *rk = ctx;
	aes_rijndael_decrypt(ctx, rk[AES_PRIV_NR_POS], crypt, plain);
}


void aes_decrypt_deinit(void *ctx)
{
	memset(ctx, 0, AES_PRIV_SIZE);
	free(ctx);
}
