/*
 * aes-debug.h
 *
 * derived from wpa_supplicant
 * Copyright (c) 2002-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef AES_DEBUG_H
#define AES_DEBUG_H

/* gettimeofday */

#if defined(_MSC_VER) || ( defined(__WIN32__) && !defined(__CYGWIN__))

struct timezone
{
    int     tz_minuteswest; /* minutes west of Greenwich */
    int     tz_dsttime;     /* type of dst correction */
};

struct timeval {
    int    tv_sec;
    int    tv_usec;
};

extern int gettimeofday(struct timeval *tv, struct timezone *tz);

#else
#include <sys/time.h>
#endif

/* Debugging function - conditional printf and hex dump */

enum {
	MSG_EXCESSIVE, MSG_MSGDUMP, MSG_DEBUG, MSG_INFO, MSG_WARNING, MSG_ERROR
};

extern int aes_debug_level;
extern int aes_debug_show_keys;
extern int aes_debug_timestamp;

#ifndef AES_DEBUG

#define aes_debug_print_timestamp() do { } while (0)
#define aes_printf(args...) do { } while (0)
#define aes_hexdump(l,t,b,le) do { } while (0)
#define aes_hexdump_buf(l,t,b) do { } while (0)
#define aes_hexdump_key(l,t,b,le) do { } while (0)
#define aes_hexdump_buf_key(l,t,b) do { } while (0)
#define aes_hexdump_ascii(l,t,b,le) do { } while (0)
#define aes_hexdump_ascii_key(l,t,b,le) do { } while (0)
#define aes_dbg(args...) do { } while (0)

#else

/**
 * aes_debug_printf_timestamp - Print timestamp for debug output
 *
 * This function prints a timestamp in seconds_from_1970.microsoconds
 * format if debug output has been configured to include timestamps in debug
 * messages.
 */
void aes_debug_print_timestamp(void);

/**
 * aes_printf - conditional printf
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration.
 *
 * Note: New line '\n' is added to the end of the text when printing to stdout.
 */
void aes_printf(int level, const char *fmt, ...) __attribute__ ((format (printf, (2), (3))));

/**
 * aes_hexdump - conditional hex dump
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of buf is printed out has hex dump.
 */
void aes_hexdump(int level, const char *title, const void *buf, size_t len);

/**
 * aes_hexdump_key - conditional hex dump, hide keys
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of buf is printed out has hex dump. This works
 * like aes_hexdump(), but by default, does not include secret keys (passwords,
 * etc.) in debug output.
 */
void aes_hexdump_key(int level, const char *title, const void *buf, size_t len);

/**
 * aes_hexdump_ascii - conditional hex dump
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of buf is printed out has hex dump with both
 * the hex numbers and ASCII characters (for printable range) are shown. 16
 * bytes per line will be shown.
 */
void aes_hexdump_ascii(int level, const char *title, const void *buf,
		       size_t len);

/**
 * aes_hexdump_ascii_key - conditional hex dump, hide keys
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of buf is printed out has hex dump with both
 * the hex numbers and ASCII characters (for printable range) are shown. 16
 * bytes per line will be shown. This works like aes_hexdump_ascii(), but by
 * default, does not include secret keys (passwords, etc.) in debug output.
 */
void aes_hexdump_ascii_key(int level, const char *title, const void *buf,
			   size_t len);

#endif /* AES_DEBUG_STDOUT */

#endif /* AES_DEBUG_H */
