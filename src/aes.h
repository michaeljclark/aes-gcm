/*
 * AES functions
 * Copyright (c) 2003-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef AES_H
#define AES_H

#ifdef __cplusplus
extern "C" {
#endif

#define AES_FULL_UNROLL
#define AES_SMALL_TABLES
#define AES_BLOCK_SIZE 16

#include "aes-common.h"
#include "aes-internal.h"
#include "aes-debug.h"

void * aes_encrypt_init(const aes_uchar *key, size_t len);
void aes_encrypt(void *ctx, const aes_uchar *plain, aes_uchar *crypt);
void aes_encrypt_deinit(void *ctx);
void * aes_decrypt_init(const aes_uchar *key, size_t len);
void aes_decrypt(void *ctx, const aes_uchar *crypt, aes_uchar *plain);
void aes_decrypt_deinit(void *ctx);

int AES_WARN_UNUSED_RESULT aes_wrap(const aes_uchar *kek, int n, const aes_uchar *plain, aes_uchar *cipher);
int AES_WARN_UNUSED_RESULT aes_unwrap(const aes_uchar *kek, int n, const aes_uchar *cipher, aes_uchar *plain);
int AES_WARN_UNUSED_RESULT omac1_aes_128_vector(const aes_uchar *key, size_t num_elem,
                                                const aes_uchar *addr[], const size_t *len,
                                                aes_uchar *mac);
int AES_WARN_UNUSED_RESULT omac1_aes_128(const aes_uchar *key, const aes_uchar *data, size_t data_len,
                                         aes_uchar *mac);
int AES_WARN_UNUSED_RESULT aes_128_encrypt_block(const aes_uchar *key, const aes_uchar *in, aes_uchar *out);
int AES_WARN_UNUSED_RESULT aes_128_ctr_encrypt(const aes_uchar *key, const aes_uchar *nonce,
                                               aes_uchar *data, size_t data_len);
int AES_WARN_UNUSED_RESULT aes_128_eax_encrypt(const aes_uchar *key,
                                               const aes_uchar *nonce, size_t nonce_len,
                                               const aes_uchar *hdr, size_t hdr_len,
                                               aes_uchar *data, size_t data_len, aes_uchar *tag);
int AES_WARN_UNUSED_RESULT aes_128_eax_decrypt(const aes_uchar *key,
                                               const aes_uchar *nonce, size_t nonce_len,
                                               const aes_uchar *hdr, size_t hdr_len,
                                               aes_uchar *data, size_t data_len, const aes_uchar *tag);
int AES_WARN_UNUSED_RESULT aes_128_cbc_encrypt(const aes_uchar *key, const aes_uchar *iv, aes_uchar *data,
                                               size_t data_len);
int AES_WARN_UNUSED_RESULT aes_128_cbc_decrypt(const aes_uchar *key, const aes_uchar *iv, aes_uchar *data,
                                               size_t data_len);
int AES_WARN_UNUSED_RESULT aes_gcm_ae(const aes_uchar *key, size_t key_len,
                                      const aes_uchar *iv, size_t iv_len,
                                      const aes_uchar *plain, size_t plain_len,
                                      const aes_uchar *aad, size_t aad_len,
                                      aes_uchar *crypt, aes_uchar *tag);
int AES_WARN_UNUSED_RESULT aes_gcm_ad(const aes_uchar *key, size_t key_len,
                                      const aes_uchar *iv, size_t iv_len,
                                      const aes_uchar *crypt, size_t crypt_len,
                                      const aes_uchar *aad, size_t aad_len, const aes_uchar *tag,
                                      aes_uchar *plain);
int AES_WARN_UNUSED_RESULT aes_gmac(const aes_uchar *key, size_t key_len,
                                    const aes_uchar *iv, size_t iv_len,
                                    const aes_uchar *aad, size_t aad_len, aes_uchar *tag);
int AES_WARN_UNUSED_RESULT aes_ccm_ae(const aes_uchar *key, size_t key_len, const aes_uchar *nonce,
                                      size_t M, const aes_uchar *plain, size_t plain_len,
                                      const aes_uchar *aad, size_t aad_len, aes_uchar *crypt, aes_uchar *auth);
int AES_WARN_UNUSED_RESULT aes_ccm_ad(const aes_uchar *key, size_t key_len, const aes_uchar *nonce,
                                      size_t M, const aes_uchar *crypt, size_t crypt_len,
                                      const aes_uchar *aad, size_t aad_len, const aes_uchar *auth,
                                      aes_uchar *plain);

#ifdef __cplusplus
}
#endif

#endif /* AES_H */
