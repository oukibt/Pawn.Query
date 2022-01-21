#include "Natives.h"
#include "Data.h"

#define CHECK_PARAMS(name,params,count) if((params[0] / sizeof(cell)) < count) return logprintf("[%s] takes %d arguments, but %d were sent", name, count, (params[0] / sizeof(cell))), 0

DECLARE_NATIVE(Natives::EnableHookQuery)
{
	CHECK_PARAMS("EnableHookQuery", params, 2);

	if (params[1] >= 0 && params[1] < QUERY_TYPES) Data.SetState((BYTE)params[1], !!params[2]);

	return 1;
}

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
	{ "SetServerInformation", Natives::SetServerInformation },
	{ "SendPing", Natives::SendPing }
};

void Natives::Register(AMX * amx)
{
	amx_Register(amx, amx_natives, sizeof(amx_natives) / sizeof(AMX_NATIVE_INFO));
}