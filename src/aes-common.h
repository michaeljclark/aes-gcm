/*
 * aes-common.h
 *
 * Copyright (c) 2014, Michael Clark <mclark@meta.sg>
 *
 * derived from wpa_supplicant
 * Copyright (c) 2002-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef AES_COMMON_H
#define AES_COMMON_H

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


/* integer types */

typedef unsigned char       aes_uchar;
typedef unsigned short      aes_ushort;
typedef unsigned int        aes_uint;
typedef unsigned long long  aes_ulong;
typedef signed char         aes_char;
typedef signed short        aes_short;
typedef signed int          aes_int;
typedef signed long long    aes_long;


/* byte order */

#if defined(__linux__) || defined(__GLIBC__)
#include <endian.h>
#include <byteswap.h>
#endif /* defined(__linux__) || defined(__GLIBC__) */

#if defined(__FreeBSD__) || defined(__NetBSD__) || \
    defined(__DragonFly__) || defined(__OpenBSD__)
#include <sys/types.h>
#include <sys/endian.h>
#define __BYTE_ORDER	_BYTE_ORDER
#define	__LITTLE_ENDIAN	_LITTLE_ENDIAN
#define	__BIG_ENDIAN	_BIG_ENDIAN
#endif /* defined(__FreeBSD__) || defined(__NetBSD__) ||
        * defined(__DragonFly__) || defined(__OpenBSD__) */

#ifdef sun
#include <sys/isa_defs.h>
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#ifdef _LITTLE_ENDIAN
#define __BYTE_ORDER __LITTLE_ENDIAN
#else
#define __BYTE_ORDER __BIG_ENDIAN
#endif
#endif

#ifdef _MSC_VER
#define inline __inline
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif /* _MSC_VER */

#ifdef __APPLE__
#include <sys/types.h>
#include <machine/endian.h>
#define __BYTE_ORDER	BYTE_ORDER
#define __LITTLE_ENDIAN	LITTLE_ENDIAN
#define __BIG_ENDIAN	BIG_ENDIAN
#endif /* __APPLE__ */


/* byte swap macros */

#ifndef bswap_16
#define bswap_16(a) ((((aes_ushort) (a) & 0xff00) >> 8) | \
                     (((aes_ushort) (a) & 0x00ff) << 8))
#endif

#ifndef bswap_32
#if defined __GNUC__
#define bswap_32(a) __builtin_bswap32(a)
#else
#define bswap_32(a) ((((aes_uint) (a) & 0xff000000) >> 24) | \
                     (((aes_uint) (a) & 0x00ff0000) >> 8) | \
                     (((aes_uint) (a) & 0x0000ff00) << 8) | \
                     (((aes_uint) (a) & 0x000000ff) << 24))
#endif
#endif

#ifndef bswap_64
#if defined __GNUC__
#define bswap_64(a) __builtin_bswap64(a)
#else
#define bswap_64(a) ((((aes_ulong) (a) & 0xff00000000000000ull) >> 56) | \
                     (((aes_ulong) (a) & 0x00ff000000000000ull) >> 40) | \
                     (((aes_ulong) (a) & 0x0000ff0000000000ull) >> 24) | \
                     (((aes_ulong) (a) & 0x000000ff00000000ull) >> 8) | \
                     (((aes_ulong) (a) & 0x00000000ff000000ull) << 8) | \
                     (((aes_ulong) (a) & 0x0000000000ff0000ull) << 24) | \
                     (((aes_ulong) (a) & 0x000000000000ff00ull) << 40) | \
                     (((aes_ulong) (a) & 0x00000000000000ffull) << 56))
#endif
#endif


#if __BYTE_ORDER == __LITTLE_ENDIAN
#define __ENDIAN_LITTLE__ 1 /* OpenCL */
#define le_to_host16(n) (n)
#define host_to_le16(n) (n)
#define be_to_host16(n) bswap_16(n)
#define host_to_be16(n) bswap_16(n)
#define le_to_host32(n) (n)
#define host_to_le32(n) (n)
#define be_to_host32(n) bswap_32(n)
#define host_to_be32(n) bswap_32(n)
#define le_to_host64(n) (n)
#define host_to_le64(n) (n)
#define be_to_host64(n) bswap_64(n)
#define host_to_be64(n) bswap_64(n)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define le_to_host16(n) bswap_16(n)
#define host_to_le16(n) bswap_16(n)
#define be_to_host16(n) (n)
#define host_to_be16(n) (n)
#define le_to_host32(n) bswap_32(n)
#define host_to_le32(n) bswap_32(n)
#define be_to_host32(n) (n)
#define host_to_be32(n) (n)
#define le_to_host64(n) bswap_64(n)
#define host_to_le64(n) bswap_64(n)
#define be_to_host64(n) (n)
#define host_to_be64(n) (n)
#else
#error Could not determine CPU byte order
#endif


/* unaligned memory accesses */

static inline aes_ushort AES_GET_BE16(const aes_uchar *a)
{
	return (a[0] << 8) | a[1];
}

static inline void AES_PUT_BE16(aes_uchar *a, aes_ushort val)
{
	a[0] = val >> 8;
	a[1] = val & 0xff;
}

static inline aes_ushort AES_GET_LE16(const aes_uchar *a)
{
	return (a[1] << 8) | a[0];
}

static inline void AES_PUT_LE16(aes_uchar *a, aes_ushort val)
{
	a[1] = val >> 8;
	a[0] = val & 0xff;
}

static inline aes_uint AES_GET_BE24(const aes_uchar *a)
{
	return (a[0] << 16) | (a[1] << 8) | a[2];
}

static inline void AES_PUT_BE24(aes_uchar *a, aes_uint val)
{
	a[0] = (val >> 16) & 0xff;
	a[1] = (val >> 8) & 0xff;
	a[2] = val & 0xff;
}

static inline aes_uint AES_GET_BE32(const aes_uchar *a)
{
	return (a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3];
}

static inline void AES_PUT_BE32(aes_uchar *a, aes_uint val)
{
	a[0] = (val >> 24) & 0xff;
	a[1] = (val >> 16) & 0xff;
	a[2] = (val >> 8) & 0xff;
	a[3] = val & 0xff;
}

static inline aes_uint AES_GET_LE32(const aes_uchar *a)
{
	return (a[3] << 24) | (a[2] << 16) | (a[1] << 8) | a[0];
}

static inline void AES_PUT_LE32(aes_uchar *a, aes_uint val)
{
	a[3] = (val >> 24) & 0xff;
	a[2] = (val >> 16) & 0xff;
	a[1] = (val >> 8) & 0xff;
	a[0] = val & 0xff;
}

static inline aes_ulong AES_GET_BE64(const aes_uchar *a)
{
	return (((aes_ulong) a[0]) << 56) | (((aes_ulong) a[1]) << 48) |
            (((aes_ulong) a[2]) << 40) | (((aes_ulong) a[3]) << 32) |
            (((aes_ulong) a[4]) << 24) | (((aes_ulong) a[5]) << 16) |
            (((aes_ulong) a[6]) << 8) | ((aes_ulong) a[7]);
}

static inline void AES_PUT_BE64(aes_uchar *a, aes_ulong val)
{
	a[0] = val >> 56;
	a[1] = val >> 48;
	a[2] = val >> 40;
	a[3] = val >> 32;
	a[4] = val >> 24;
	a[5] = val >> 16;
	a[6] = val >> 8;
	a[7] = val & 0xff;
}

static inline aes_ulong AES_GET_LE64(const aes_uchar *a)
{
	return (((aes_ulong) a[7]) << 56) | (((aes_ulong) a[6]) << 48) |
            (((aes_ulong) a[5]) << 40) | (((aes_ulong) a[4]) << 32) |
            (((aes_ulong) a[3]) << 24) | (((aes_ulong) a[2]) << 16) |
            (((aes_ulong) a[1]) << 8) | ((aes_ulong) a[0]);
}

static inline void AES_PUT_LE64(aes_uchar *a, aes_ulong val)
{
	a[7] = val >> 56;
	a[6] = val >> 48;
	a[5] = val >> 40;
	a[4] = val >> 32;
	a[3] = val >> 24;
	a[2] = val >> 16;
	a[1] = val >> 8;
	a[0] = val & 0xff;
}


/* function attribute macros */

#ifndef AES_WARN_UNUSED_RESULT
#if defined __GNUC__
#define AES_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
#else
#define AES_WARN_UNUSED_RESULT
#endif /* __GNUC__ */
#endif /* AES_WARN_UNUSED_RESULT */

#endif /* AES_COMMON_H */
