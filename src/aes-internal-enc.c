/*
 * AES (Rijndael) cipher - encrypt
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
 * Expand the cipher key into the encryption key schedule.
 *
 * @return	the number of rounds for the given cipher key size.
 */
int aes_rijndael_key_setup_enc(aes_uint rk[], const aes_uchar cipherKey[], size_t keyBits)
{
	int i;
	aes_uint temp;
    
	rk[0] = GETU32(cipherKey     );
	rk[1] = GETU32(cipherKey +  4);
	rk[2] = GETU32(cipherKey +  8);
	rk[3] = GETU32(cipherKey + 12);
    
	if (keyBits == 128) {
		for (i = 0; i < 10; i++) {
			temp  = rk[3];
			rk[4] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
                            TE443(temp) ^ TE414(temp) ^ RCON(i);
			rk[5] = rk[1] ^ rk[4];
			rk[6] = rk[2] ^ rk[5];
			rk[7] = rk[3] ^ rk[6];
			rk += 4;
		}
		return 10;
	}
    
	rk[4] = GETU32(cipherKey + 16);
	rk[5] = GETU32(cipherKey + 20);
    
	if (keyBits == 192) {
		for (i = 0; i < 8; i++) {
			temp  = rk[5];
			rk[6] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
                            TE443(temp) ^ TE414(temp) ^ RCON(i);
			rk[7] = rk[1] ^ rk[6];
			rk[8] = rk[2] ^ rk[7];
			rk[9] = rk[3] ^ rk[8];
			if (i == 7)
				return 12;
			rk[10] = rk[4] ^ rk[9];
			rk[11] = rk[5] ^ rk[10];
			rk += 6;
		}
	}
    
	rk[6] = GETU32(cipherKey + 24);
	rk[7] = GETU32(cipherKey + 28);
    
	if (keyBits == 256) {
		for (i = 0; i < 7; i++) {
			temp  = rk[7];
			rk[8] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
                            TE443(temp) ^ TE414(temp) ^ RCON(i);
			rk[9] = rk[1] ^ rk[8];
			rk[10] = rk[2] ^ rk[9];
			rk[11] = rk[3] ^ rk[10];
			if (i == 6)
				return 14;
			temp  = rk[11];
			rk[12] = rk[4] ^ TE411(temp) ^ TE422(temp) ^
                             TE433(temp) ^ TE444(temp);
			rk[13] = rk[5] ^ rk[12];
			rk[14] = rk[6] ^ rk[13];
			rk[15] = rk[7] ^ rk[14];
			rk += 8;
		}
	}
    
	return -1;
}

void aes_rijndael_encrypt(const aes_uint rk[], int Nr, const aes_uchar pt[16], aes_uchar ct[16])
{
	aes_uint s0, s1, s2, s3, t0, t1, t2, t3;
#ifndef AES_FULL_UNROLL
	int r;
#endif /* AES_FULL_UNROLL */

	/*
	 * map byte array block to cipher state
	 * and add initial round key:
	 */
	s0 = GETU32(pt     ) ^ rk[0];
	s1 = GETU32(pt +  4) ^ rk[1];
	s2 = GETU32(pt +  8) ^ rk[2];
	s3 = GETU32(pt + 12) ^ rk[3];

#define ROUND(i,d,s) \
d##0 = TE0(s##0) ^ TE1(s##1) ^ TE2(s##2) ^ TE3(s##3) ^ rk[4 * i]; \
d##1 = TE0(s##1) ^ TE1(s##2) ^ TE2(s##3) ^ TE3(s##0) ^ rk[4 * i + 1]; \
d##2 = TE0(s##2) ^ TE1(s##3) ^ TE2(s##0) ^ TE3(s##1) ^ rk[4 * i + 2]; \
d##3 = TE0(s##3) ^ TE1(s##0) ^ TE2(s##1) ^ TE3(s##2) ^ rk[4 * i + 3]

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
	s0 = TE41(t0) ^ TE42(t1) ^ TE43(t2) ^ TE44(t3) ^ rk[0];
	PUTU32(ct     , s0);
	s1 = TE41(t1) ^ TE42(t2) ^ TE43(t3) ^ TE44(t0) ^ rk[1];
	PUTU32(ct +  4, s1);
	s2 = TE41(t2) ^ TE42(t3) ^ TE43(t0) ^ TE44(t1) ^ rk[2];
	PUTU32(ct +  8, s2);
	s3 = TE41(t3) ^ TE42(t0) ^ TE43(t1) ^ TE44(t2) ^ rk[3];
	PUTU32(ct + 12, s3);
}


/* AES encrypt interface */

void * aes_encrypt_init(const aes_uchar *key, size_t len)
{
	aes_uint *rk;
	int res;
	rk = malloc(AES_PRIV_SIZE);
	if (rk == NULL)
		return NULL;
	res = aes_rijndael_key_setup_enc(rk, key, len * 8);
	if (res < 0) {
		free(rk);
		return NULL;
	}
	rk[AES_PRIV_NR_POS] = res;
	return rk;
}


void aes_encrypt(void *ctx, const aes_uchar *plain, aes_uchar *crypt)
{
	aes_uint *rk = ctx;
	aes_rijndael_encrypt(ctx, rk[AES_PRIV_NR_POS], plain, crypt);
}


void aes_encrypt_deinit(void *ctx)
{
	memset(ctx, 0, AES_PRIV_SIZE);
	free(ctx);
}

