/*
 * aes-debug.c
 *
 * derived from wpa_supplicant
 * Copyright (c) 2002-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#define AES_DEBUG

#include "aes.h"

int aes_debug_level = MSG_EXCESSIVE;
int aes_debug_show_keys = 1;
int aes_debug_timestamp = 1;

#if defined(_MSC_VER) || ( defined(__WIN32__) && !defined(__CYGWIN__))

#include <time.h>
#include <windows.h>

const __int64 DELTA_EPOCH_IN_MICROSECS= 11644473600000000;

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME ft;
    __int64 tmpres = 0;
    TIME_ZONE_INFORMATION tz_winapi;
    int ret = 0;
    
    ZeroMemory(&ft,sizeof(ft));
    ZeroMemory(&tz_winapi,sizeof(tz_winapi));
    
    GetSystemTimeAsFileTime(&ft);
    
    tmpres = ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
    
    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tv->tv_sec = (__int32)(tmpres*0.000001);
    tv->tv_usec =(tmpres%1000000);
    
    if (tz) {
        ret = GetTimeZoneInformation(&tz_winapi);
        tz->tz_dsttime = (ret == 2) ? true : false;
        tz->tz_minuteswest = tz_winapi.Bias + ((rez==2)?tz_winapi.DaylightBias:0);
    }
    
    return 0;
}

#endif


void aes_debug_print_timestamp(void)
{
	struct timeval tv;

	if (!aes_debug_timestamp)
		return;

	gettimeofday(&tv, NULL);
	printf("%ld.%06u: ", (long) tv.tv_sec, (unsigned int) tv.tv_usec);
}


void aes_printf(int level, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	if (level >= aes_debug_level) {
		aes_debug_print_timestamp();
		vprintf(fmt, ap);
		printf("\n");
	}
	va_end(ap);
}


static void _aes_hexdump(int level, const char *title, const aes_uchar *buf,
			 size_t len, int show)
{
	size_t i;
    
	if (level < aes_debug_level)
		return;

    aes_debug_print_timestamp();
	printf("%s - hexdump(len=%lu):", title, (unsigned long) len);
	if (buf == NULL) {
		printf(" [NULL]");
	} else if (show) {
		for (i = 0; i < len; i++)
			printf(" %02x", buf[i]);
	} else {
		printf(" [REDACTED]");
	}
	printf("\n");
}


void aes_hexdump(int level, const char *title, const void *buf, size_t len)
{
	_aes_hexdump(level, title, buf, len, 1);
}


void aes_hexdump_key(int level, const char *title, const void *buf, size_t len)
{
	_aes_hexdump(level, title, buf, len, aes_debug_show_keys);
}


static void _aes_hexdump_ascii(int level, const char *title, const void *buf,
			       size_t len, int show)
{
	size_t i, llen;
	const aes_uchar *pos = buf;
	const size_t line_len = 16;

	if (level < aes_debug_level)
		return;

	aes_debug_print_timestamp();
	if (!show) {
		printf("%s - hexdump_ascii(len=%lu): [REDACTED]\n",
		       title, (unsigned long) len);
		return;
	}
	if (buf == NULL) {
		printf("%s - hexdump_ascii(len=%lu): [NULL]\n",
		       title, (unsigned long) len);
		return;
	}
	printf("%s - hexdump_ascii(len=%lu):\n", title, (unsigned long) len);
	while (len) {
		llen = len > line_len ? line_len : len;
		printf("    ");
		for (i = 0; i < llen; i++)
			printf(" %02x", pos[i]);
		for (i = llen; i < line_len; i++)
			printf("   ");
		printf("   ");
		for (i = 0; i < llen; i++) {
			if (isprint(pos[i]))
				printf("%c", pos[i]);
			else
				printf("_");
		}
		for (i = llen; i < line_len; i++)
			printf(" ");
		printf("\n");
		pos += llen;
		len -= llen;
	}
}


void aes_hexdump_ascii(int level, const char *title, const void *buf,
		       size_t len)
{
	_aes_hexdump_ascii(level, title, buf, len, 1);
}


void aes_hexdump_ascii_key(int level, const char *title, const void *buf,
			   size_t len)
{
	_aes_hexdump_ascii(level, title, buf, len, aes_debug_show_keys);
}
