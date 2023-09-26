#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>

void check_gl_errors(const char *file, int line);

#define CheckGlErrors() check_gl_errors(__FILE__, __LINE__)
#define LOG(fmt, ...) printf("[%s:%d]: " fmt, __FILE__, __LINE__, __VA_ARGS__)

#endif /* _UTIL_H_ */