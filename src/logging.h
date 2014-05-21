//
//  logging.h
//

#ifndef logging_h
#define logging_h

std::string format_string(const char* fmt, ...);
extern void log_error_exit(const char* fmt, ...);
extern void log_error(const char* fmt, ...);
extern void log_info(const char* fmt, ...);
extern void log_debug(const char* fmt, ...);

#endif
