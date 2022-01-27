#include "Natives.h"
#include "Data.h"

#include <cstring>

#define CHECK_PARAMS(name,params,count) if((params[0] / sizeof(cell)) < count) return logprintf("[%s] takes %d arguments, but %d were sent", name, count, (params[0] / sizeof(cell))), 0

// native EnableHookQuery(Type, bool: enable);
DECLARE_NATIVE(Natives::EnableHookQuery)
{
	CHECK_PARAMS("EnableHookQuery", params, 2);

	if (params[1] >= 0 && params[1] < QUERY_TYPES) Data.SetState((BYTE)params[1], !!params[2]);

	return 1;
}

// native SetServerPlayers(const Players[][Query_Players], const size = sizeof Players);
DECLARE_NATIVE(Natives::SetServerPlayers)
{
	CHECK_PARAMS("SetServerPlayers", params, 2);

	cell* addr;
	amx_GetAddr(amx, params[1], &addr);

	int i, len;

	QueryPlayer Player;

	Data.Players.resize(0);

	for (i = params[2]; i < params[2] * (MAX_PLAYER_NAME + 1) + params[2];)
	{
		amx_StrLen(&addr[i], &len);
		amx_GetString(Player.Name, &addr[i], NULL, len + 1);

		i += MAX_PLAYER_NAME;

		Player.Score = addr[i];

		i++;

		Data.Players.push_back(Player);
	}

	return 1;
}

// native SetServerRules(const Rules[][Query_Rules], const size = sizeof Rules);
DECLARE_NATIVE(Natives::SetServerRules)
{
	CHECK_PARAMS("SetServerRules", params, 2);

	cell* addr;
	amx_GetAddr(amx, params[1], &addr);

	int i, len;

	QueryRule Rule;

	Data.Rules.resize(0);

	for (i = params[2]; i < params[2] * (MAX_RULE_LENGTH * 2) + params[2];)
	{
		amx_StrLen(&addr[i], &len);
		amx_GetString(Rule.Name, &addr[i], NULL, len + 1);

		i += MAX_RULE_LENGTH;

		amx_StrLen(&addr[i], &len);
		amx_GetString(Rule.Value, &addr[i], NULL, len + 1);

		i += MAX_RULE_LENGTH;

		Data.Rules.push_back(Rule);
	}

	return 1;
}

// native SetServerRule(const name[], const value[]);
DECLARE_NATIVE(Natives::SetServerRule)
{
	CHECK_PARAMS("SetServerRule", params, 2);

	cell* addr;
	int len, i, size = Data.Rules.size();

	QueryRule Rule;

	amx_GetAddr(amx, params[1], &addr);
	amx_StrLen(addr, &len);
	amx_GetString(Rule.Name, addr, NULL, len + 1);

	amx_GetAddr(amx, params[2], &addr);
	amx_StrLen(addr, &len);
	amx_GetString(Rule.Value, addr, NULL, len + 1);

	for (i = 0; i < size; i++)
	{
		if (std::strcmp(Rule.Name, Data.Rules[i].Name) == false)
		{
			memcpy(Data.Rules[i].Name, Rule.Name, sizeof(Rule.Name));
			memcpy(Data.Rules[i].Value, Rule.Value, sizeof(Rule.Value));

			return 1;
		}
	}

	Data.Rules.push_back(Rule);

	return 1;
}

// native GetServerRule(const name[], dest[]);
DECLARE_NATIVE(Natives::GetServerRule)
{
	CHECK_PARAMS("GetServerRule", params, 2);

	cell* addr;
	int i, len = 0, size = Data.Rules.size();

	amx_GetAddr(amx, params[1], &addr);

	amx_StrLen(addr, &len);
	len++;

	if (len <= 0 || len >= MAX_RULE_LENGTH) return 0;

	char ruleName[MAX_RULE_LENGTH];
	memset(ruleName, 0, sizeof(ruleName));
	amx_GetString(ruleName, addr, NULL, MAX_RULE_LENGTH);

	amx_GetAddr(amx, params[2], &addr);

	for (i = 0; i < size; i++)
	{
		if (std::strcmp(ruleName, Data.Rules[i].Name) == false)
		{
			amx_SetString(addr, Data.Rules[i].Value, NULL, NULL, MAX_RULE_LENGTH);

			return 1;
		}
	}

	return 0;
}

// native RemoveServerRule(const name[]);
DECLARE_NATIVE(Natives::RemoveServerRule)
{
	CHECK_PARAMS("RemoveServerRule", params, 1);

	char name[MAX_RULE_LENGTH];

	cell* addr;
	int i, len, size = Data.Rules.size();

	amx_GetAddr(amx, params[1], &addr);
	amx_StrLen(addr, &len);
	amx_GetString(name, addr, NULL, len + 1);

	for (i = 0; i < size; i++)
	{
		if (std::strcmp(name, Data.Rules[i].Name) == false)
		{
			Data.Rules.erase(Data.Rules.begin() + i);

			return 1;
		}
	}

	return 0;
}

// native bool: IsValidServerRule(const name[]);
DECLARE_NATIVE(Natives::IsValidServerRule)
{
	CHECK_PARAMS("IsValidServerRule", params, 1);

	char name[MAX_RULE_LENGTH];

	cell* addr;
	int i, len, size = Data.Rules.size();

	amx_GetAddr(amx, params[1], &addr);
	amx_StrLen(addr, &len);
	amx_GetString(name, addr, NULL, len + 1);

	for (i = 0; i < size; i++)
	{
		if (std::strcmp(name, Data.Rules[i].Name) == false) return 1;
	}

	return 0;
}

// native ClearRules();
DECLARE_NATIVE(Natives::ClearRules)
{
	CHECK_PARAMS("ClearRules", params, 0);

	Data.Rules.clear();

	return 0;
}

// native SetServerInformation(bool: havePassword, MaxPlayers, const hostname[], const gamemode[], const language[]);
DECLARE_NATIVE(Natives::SetServerInformation)
{
	CHECK_PARAMS("SetServerInformation", params, 5);

	cell* addr;

	int len;

	Data.ServerInfo.Password = !!params[1];
	Data.ServerInfo.MaxPlayers = params[2];

	amx_GetAddr(amx, params[3], &addr);
	amx_StrLen(addr, &len);
	amx_GetString(Data.ServerInfo.Hostname, addr, NULL, len + 1);

	amx_GetAddr(amx, params[4], &addr);
	amx_StrLen(addr, &len);
	amx_GetString(Data.ServerInfo.Gamemode, addr, NULL, len + 1);

	amx_GetAddr(amx, params[5], &addr);
	amx_StrLen(addr, &len);
	amx_GetString(Data.ServerInfo.Language, addr, NULL, len + 1);

	return 1;
}

// native SetServerDetailedPlayers(const DetailedPlayers[][Query_DetailedPlayers], const size = sizeof DetailedPlayers);
DECLARE_NATIVE(Natives::SetServerDetailedPlayers)
{
	CHECK_PARAMS("SetServerDetailedPlayers", params, 2);

	cell* addr;
	amx_GetAddr(amx, params[1], &addr);

	int i, len;

	QueryDetailedPlayer DetailedPlayer;

	Data.DetailedPlayers.resize(0);

	for (i = params[2]; i < params[2] * (1 + MAX_PLAYER_NAME + 1 + 1) + params[2];)
	{
		DetailedPlayer.ID = addr[i];

		i++;

		amx_StrLen(&addr[i], &len);
		amx_GetString(DetailedPlayer.Name, &addr[i], NULL, len + 1);

		i += MAX_PLAYER_NAME;

		DetailedPlayer.Score = addr[i];

		i++;

		DetailedPlayer.Ping = addr[i];

		i++;

		Data.DetailedPlayers.push_back(DetailedPlayer);
	}

	return 1;
}

// native SendPing(bool: send);
DECLARE_NATIVE(Natives::SendPing)
{
	CHECK_PARAMS("SendPing", params, 1);

	Data.SendPing = !!params[1];

	return 1;
}

AMX_NATIVE_INFO amx_natives[] = {

	{ "EnableHookQuery", Natives::EnableHookQuery },
	{ "SetServerPlayers", Natives::SetServerPlayers },
	{ "SetServerDetailedPlayers", Natives::SetServerDetailedPlayers },
	{ "SetServerRules", Natives::SetServerRules },
	{ "SetServerRule", Natives::SetServerRule },
	{ "GetServerRule", Natives::GetServerRule },
	{ "RemoveServerRule", Natives::RemoveServerRule },
	{ "IsValidServerRule", Natives::IsValidServerRule },
	{ "ClearRules", Natives::ClearRules },
	{ "SetServerInformation", Natives::SetServerInformation },
	{ "SendPing", Natives::SendPing }
};

void Natives::Register(AMX * amx)
{
	amx_Register(amx, amx_natives, sizeof(amx_natives) / sizeof(AMX_NATIVE_INFO));
}