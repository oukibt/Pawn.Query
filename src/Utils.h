#ifndef UTILS_H
#define UTILS_H

using logprintf_t = void(*)(const char* fmt, ...);
extern logprintf_t logprintf;

#endif