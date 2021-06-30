#include "Natives.h"
#include "Hooks.h"

DECLARE_NATIVE(EnableHookQuery)
{
	CHECK_PARAMS(params, 1);

	CAddress::InitHook();

	CAddress::UnloadHook();

	return 1;
}