//
//  logging.cc
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>

#include "logging.h"

std::string format_string(const char* fmt, ...)
{
    char msgbuf[128];
    va_list ap;
    
    va_start(ap, fmt);
    int len = vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);
    va_end(ap);
    
    std::string str;
    if (len > 127) {
        char *bigbuf = new char[len + 1];
        va_start(ap, fmt);
        vsnprintf(bigbuf, len + 1, fmt, ap);
        va_end(ap);
        str = bigbuf;
        delete [] bigbuf;
    } else {
        str = msgbuf;
    }
    
    return str;
}

void log_error_exit(const char* fmt, ...)
{
    char msgbuf[1024];
    va_list ap;
    
    va_start(ap, fmt);
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);
    va_end(ap);
    
    fprintf(stderr, "FATAL ERROR: %s\n", msgbuf);
    
    exit(9);
}

void log_error(const char* fmt, ...)
{
    char msgbuf[1024];
    va_list ap;
    
    va_start(ap, fmt);
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);
    va_end(ap);
    
    fprintf(stderr, "ERROR: %s\n", msgbuf);
}

void log_info(const char* fmt, ...)
{
    char msgbuf[1024];
    va_list ap;
    
    va_start(ap, fmt);
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);
    va_end(ap);
    
    fprintf(stdout, "INFO: %s\n", msgbuf);
}

void log_debug(const char* fmt, ...)
{
    char msgbuf[1024];
    va_list ap;
    
    va_start(ap, fmt);
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);
    va_end(ap);
    
    fprintf(stdout, "DEBUG: %s\n", msgbuf);
}
