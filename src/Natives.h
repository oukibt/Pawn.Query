#ifndef NATIVES_H
#define NATIVES_H

#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

#include "Utils.h"
#include "Hooks.h"

#define DECLARE_NATIVE(name) cell AMX_NATIVE_CALL name(AMX* amx, cell* params)

extern AMX_NATIVE_INFO amx_natives[];

namespace Natives
{
	DECLARE_NATIVE(EnableHookQuery);
	DECLARE_NATIVE(SetServerPlayers);
	DECLARE_NATIVE(SetServerRules);
	DECLARE_NATIVE(SetServerRule);
	DECLARE_NATIVE(RemoveServerRule);
	DECLARE_NATIVE(IsValidServerRule);
	DECLARE_NATIVE(ClearRules);
	DECLARE_NATIVE(GetServerRuleCount);
	DECLARE_NATIVE(SetServerRuleValueByID);
	DECLARE_NATIVE(RemoveServerRuleByID);
	DECLARE_NATIVE(GetServerRuleByID);
	DECLARE_NATIVE(GetServerRuleID);
	DECLARE_NATIVE(GetServerRuleValue);
	DECLARE_NATIVE(GetServerRuleValueByID);
	DECLARE_NATIVE(SetServerInformation);
	DECLARE_NATIVE(SetServerDetailedPlayers);
	DECLARE_NATIVE(SendPing);

	void Register(AMX* amx);
};

#endif