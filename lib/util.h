#pragma once

#include <stdio.h>

void check_gl_errors(const char *file, int line);

#ifndef NDEBUG
#define CheckGlErrors() check_gl_errors(__FILE__, __LINE__)
#define LOG(fmt, ...) printf("[%s:%d]: " fmt, __FILE__, __LINE__, __VA_ARGS__)
#else
#define CheckGlErrors()
#define LOG(fmt, ...)
#endif
