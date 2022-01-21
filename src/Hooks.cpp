#include <cstring>

#include "Hooks.h"

int ProcessQueryPacketAddr = NULL;
urmem::hook hook_ProcessQueryPacket;

DWORD FindPattern(char* pattern, char* mask);
bool memory_compare(const BYTE* data, const BYTE* pattern, const char* mask);

//

void CPacketHook::Initialize()
{
#ifdef _WIN32
	ProcessQueryPacketAddr = FindPattern(QUERY_PATTERN);
#else
	ProcessQueryPacketAddr = FindPattern(QUERY_PATTERN) - 0x3;
#endif

	hook_ProcessQueryPacket.install(ProcessQueryPacketAddr, urmem::get_func_addr(&HOOK_ProcessQueryPacket));
	hook_ProcessQueryPacket.enable();
}

//

bool memory_compare(const BYTE* data, const BYTE* pattern, const char* mask)
{
	for (; *mask; ++mask, ++data, ++pattern) if (*mask == 'x' && *data != *pattern) return false;
	return (*mask) == NULL;
}

DWORD FindPattern(char* pattern, char* mask)
{
#ifdef _WIN32
	MODULEINFO info = { 0 };

	DWORD address = (DWORD)GetModuleHandle(NULL);
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO));
	DWORD size = (DWORD)info.SizeOfImage;

	for (DWORD i = 0; i < size; ++i)
	{
		if (memory_compare((BYTE*)(address + i), (BYTE*)pattern, mask))
		{
			return (DWORD)(address + i);
		}
	}
	return 0;
#else
	struct
	{
		DWORD result;
		char* pattern;
		char* mask;
	} pinfo;

	pinfo.result = 0;
	pinfo.pattern = pattern;
	pinfo.mask = mask;

	dl_iterate_phdr([](struct dl_phdr_info* info, size_t, void* data)
		{
			auto info2 = reinterpret_cast<decltype(pinfo)*>(data);
			for (int s = 0; s < info->dlpi_phnum; s++)
			{
				DWORD address = info->dlpi_addr + info->dlpi_phdr[s].p_vaddr;
				DWORD size = info->dlpi_phdr[s].p_memsz;
				for (DWORD i = 0; i < size; ++i)
				{
					if (memory_compare((BYTE*)(address + i), (BYTE*)info2->pattern, info2->mask))
					{
						info2->result = (DWORD)(address + i);
						return 1;
					}
				}
			}
			return 1;
		}, reinterpret_cast<void*>(&pinfo));
	return pinfo.result;
#endif
}