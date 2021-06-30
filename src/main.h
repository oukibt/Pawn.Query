#ifndef MAIN_H
#define MAIN_H

#ifdef _WIN32

#define SUBHOOK_IMPLEMENTATION
#include <Windows.h>
#pragma comment (lib, "Ws2_32.lib")
#include <Psapi.h>

#else
#include <unistd.h>
#include <sys/mman.h>
#include <link.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int SOCKET;
typedef unsigned long DWORD;
typedef const char* PCHAR;

#endif // _WIN32

#endif // MAIN_H