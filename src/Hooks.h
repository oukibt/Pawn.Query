#ifndef HOOKS_H
#define HOOKS_H

#include "includes/platform.h"
#include "includes/types.h"

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

#endif

using logprintf_t = void(*)(const char* fmt, ...);
logprintf_t logprintf = nullptr;

#include <string>

#include "subhook/subhook.h"

extern "C"
{
	#include "subhook/subhook_private.h"
}

#ifdef _WIN32
	#define QUERY_PATTERN "\x83\xEC\x24\x53\x55\x56\x57\x8B\x7C\x24","xxxxxxxxxx"
#else
	#define QUERY_PATTERN "\x81\xEC\xA8\x00\x00\x00\x89\x5D\xF4\x8B\x5D\x14\x89\x75\xF8","xxxxxxxxxxxxxxx"
#endif

namespace aux
{
	template <class T>
	struct is_function_pointer : std::false_type {};

#define DEF_IS_FUNCTION_POINTER_CQ(CALLCONV, QUALIFIERS) \
	template <class TRet, class... TArgs> \
	struct is_function_pointer<TRet(CALLCONV * QUALIFIERS)(TArgs...)> : std::true_type { }

#define DEF_IS_FUNCTION_POINTER(CALLCONV) \
	DEF_IS_FUNCTION_POINTER_CQ(CALLCONV,); \
	DEF_IS_FUNCTION_POINTER_CQ(CALLCONV, const); \
	DEF_IS_FUNCTION_POINTER_CQ(CALLCONV, volatile); \
	DEF_IS_FUNCTION_POINTER_CQ(CALLCONV, const volatile)

#ifdef _WIN32
	DEF_IS_FUNCTION_POINTER(__cdecl);
	DEF_IS_FUNCTION_POINTER(__stdcall);
	DEF_IS_FUNCTION_POINTER(__fastcall);
	DEF_IS_FUNCTION_POINTER(__thiscall);
#else
	DEF_IS_FUNCTION_POINTER();
#endif


	
	template <class TRet, class... TArgs>
	struct is_function_pointer<TRet(*)(TArgs..., ...)> : std::true_type { };
#undef DEF_IS_FUNCTION_POINTER_CQ
#undef DEF_IS_FUNCTION_POINTER
}

bool Unlock(void* address, size_t len)
{
#ifdef _WIN32
	DWORD oldp;
	return !!VirtualProtect(address, len, PAGE_EXECUTE_READWRITE, &oldp);
#else
	size_t
		iPageSize = getpagesize(),
		iAddr = ((reinterpret_cast <uint32_t>(address) / iPageSize) * iPageSize);
	return !mprotect(reinterpret_cast <void*>(iAddr), len, PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
}

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

void InstallJump(unsigned long addr, void* func)
{
#ifdef WIN32
	unsigned long dwOld;
	VirtualProtect((LPVOID)addr, 5, PAGE_EXECUTE_READWRITE, &dwOld);
#else
	int pagesize = sysconf(_SC_PAGE_SIZE);
	void* unpraddr = (void*)(((int)addr + pagesize - 1) & ~(pagesize - 1)) - pagesize;
	mprotect(unpraddr, pagesize, PROT_WRITE);
#endif
	*(unsigned char*)addr = 0xE9;
	*(unsigned long*)((unsigned long)addr + 0x1) = (unsigned long)func - (unsigned long)addr - 5;
#ifdef WIN32
	VirtualProtect((LPVOID)addr, 5, dwOld, &dwOld);
#else
	mprotect(unpraddr, pagesize, PROT_READ | PROT_EXEC);
#endif
}

template <class T>
class ADDR
{
	typedef typename std::conditional<aux::is_function_pointer<T>::value, T, T*>::type ptr_type;

	ptr_type ptr;
public:
	ADDR() : ptr(nullptr) { }
	ADDR(decltype(nullptr)) : ptr(nullptr) { }
	ADDR(DWORD addr) : ptr(reinterpret_cast<ptr_type>(addr)) { }

	operator DWORD() const
	{
		return reinterpret_cast<DWORD>(ptr);
	}

	ptr_type get() const
	{
		return ptr;
	}
};

#define DECLARE_FUNC_PTR(name) static ADDR<name ## _t> FUNC_ ## name
#define DEFINE_FUNC_PTR(name) ADDR<name ## _t> CAddress::FUNC_ ## name = nullptr

int HOOK_ProcessQueryPacket(unsigned int binaryAddress, unsigned short port, char* data, int length, SOCKET s);

template <class Func>
class FuncHook
{
	subhook_t hook;

	class hook_restorer
	{
		subhook_t hook;

	public:
		hook_restorer(subhook_t hook) : hook(hook)
		{
			subhook_remove(hook);
		}

		~hook_restorer()
		{
			subhook_install(hook);
		}
	};

	class hook_restorer_dst
	{
		subhook_t hook;
		void* dst;
		void* olddst;

	public:
		hook_restorer_dst(subhook_t hook, void* dst, void* olddst) : hook(hook), dst(dst), olddst(olddst)
		{
			hook->dst = dst;
			subhook_remove(hook);
		}

		~hook_restorer_dst()
		{
			subhook_install(hook);
			hook->dst = olddst;
		}
	};

public:
	FuncHook() = default;

	FuncHook(const ADDR<Func>& addr, Func dst) : hook(
		subhook_new(reinterpret_cast<void*>(addr.get()), reinterpret_cast<void*>(dst), {})
	)
	{
		subhook_install(hook);
	}

	FuncHook(Func addr, Func dst) : hook(
		subhook_new(reinterpret_cast<void*>(addr), reinterpret_cast<void*>(dst), {})
	)
	{
		subhook_install(hook);
	}

	template <class... Args>
	auto operator()(Args&&... args) -> decltype(static_cast<Func>(nullptr)(std::forward<Args>(args)...))
	{
		auto tptr = subhook_get_trampoline(hook);
		if (!tptr)
		{
			auto src = subhook_get_src(hook);
			auto dst = subhook_read_dst(src);
			auto olddst = subhook_get_dst(hook);
			if (dst != olddst)
			{
				hook_restorer_dst restorer(hook, dst, olddst);
				return reinterpret_cast<Func>(src)(std::forward<Args>(args)...);
			}
			else if (!dst)
			{
				return reinterpret_cast<Func>(src)(std::forward<Args>(args)...);
			}
			else {
				hook_restorer restorer(hook);
				return reinterpret_cast<Func>(src)(std::forward<Args>(args)...);
			}
		}
		return reinterpret_cast<Func>(tptr)(std::forward<Args>(args)...);
	}

	FuncHook<Func>& operator=(const FuncHook<Func>& obj) = default;

	void free()
	{
		subhook_remove(hook);
		subhook_free(hook);
		hook = {};
	}
};

typedef int (*ProcessQueryPacket_t)(unsigned int binaryAddress, unsigned short port, char* data, int length, SOCKET s);

class CAddress
{
	typedef void(logprintf_t)(const char* format, ...);
public:
	static void	Initialize();
	static void InitHook();
	static void UnloadHook();

	DECLARE_FUNC_PTR(ProcessQueryPacket);

#ifdef _WIN32
	static ADDR<unsigned char[82]> ADDR_WrongPacketIDBranch;
#else
	static ADDR<unsigned char[114]> ADDR_WrongPacketIDBranch;
#endif
};

#define DEFINE_HOOK(name) static FuncHook<decltype(CAddress::FUNC_##name.get())> name##_hook;

DEFINE_HOOK(ProcessQueryPacket);

int HOOK_ProcessQueryPacket(unsigned int binaryAddress, unsigned short port, char* data, int length, SOCKET s);

DEFINE_FUNC_PTR(ProcessQueryPacket);

void hook_func() {}
template <class TFunc, class THook>
FuncHook<TFunc> Hook(const char* name, ADDR<TFunc>& func, THook hook)
{
	FuncHook<TFunc> var;
	if (!func) var = FuncHook<TFunc>(reinterpret_cast<TFunc>(hook_func), reinterpret_cast<TFunc>(hook));
	else var = FuncHook<TFunc>(func, reinterpret_cast<TFunc>(hook));
	return var;
}

#define HOOK(name) Hook(#name, CAddress::FUNC_##name, HOOK_##name)

void CAddress::Initialize()
{
#ifdef _WIN32
	FUNC_ProcessQueryPacket = FindPattern(QUERY_PATTERN);
#else
	FUNC_ProcessQueryPacket = FindPattern(QUERY_PATTERN) - 0x3;
#endif
}

void CAddress::InitHook()
{
	ProcessQueryPacket_hook = HOOK(ProcessQueryPacket);
}

void CAddress::UnloadHook()
{
	ProcessQueryPacket_hook.free();
}

#endif