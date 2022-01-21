#ifndef HOOKS_H
#define HOOKS_H

#ifdef _WIN32 //

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
typedef unsigned short WORD;
typedef unsigned char BYTE;

#endif //

#include "urmem.hpp"

#ifdef _WIN32
#define QUERY_PATTERN "\x83\xEC\x24\x53\x55\x56\x57\x8B\x7C\x24","xxxxxxxxxx"
#else
#define QUERY_PATTERN "\x81\xEC\xA8\x00\x00\x00\x89\x5D\xF4\x8B\x5D\x14\x89\x75\xF8","xxxxxxxxxxxxxxx"
#endif

extern int ProcessQueryPacketAddr;
extern urmem::hook hook_ProcessQueryPacket;

int HOOK_ProcessQueryPacket(unsigned int addr, unsigned short port, char* data, int len, SOCKET sock);

class CPacketHook
{
public:
	static void Initialize();
};

#endif