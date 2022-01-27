/* Pawn.Query 1.1.0 | oukibt | 22.01.2022 */

#if defined PAWNQUERY_INC
	#endinput
#endif

#if !defined _samp_included
	#include <a_samp>
#endif

#define MAX_RULE_LENGTH 30

#define MAX_HOSTNAME_LENGTH 60
#define MAX_GAMEMODE_LENGTH 50
#define MAX_LANGUAGE_LENGTH 30

enum _:Query_HookType
{
	QHOOK_INFO,
	QHOOK_RULES,
	QHOOK_PLAYERS,
	QHOOK_DETAILED_PLAYERS,
	QHOOK_PING,
};

enum _:Query_Players
{
	PQ_pName[MAX_PLAYER_NAME],
	PQ_pScore,
};

enum _:Query_DetailedPlayers
{
	PQ_dID,
	PQ_dName[MAX_PLAYER_NAME],
	PQ_dScore,
	PQ_dPing,
};

enum _:Query_Rules 
{
	PQ_rName[MAX_RULE_LENGTH],
	PQ_rValue[MAX_RULE_LENGTH],
};

native EnableHookQuery(Type, bool: enable);
native SetServerPlayers(const Players[][Query_Players], const size = sizeof Players);

native SetServerRules(const Rules[][Query_Rules], const size = sizeof Rules);

native SetServerRule(const name[], const value[]);
native GetServerRule(const name[], dest[]);
native RemoveServerRule(const name[]);
native bool: IsValidServerRule(const name[]);
native ClearRules();

native SetServerInformation(bool: havePassword, MaxPlayers, const hostname[], const gamemode[], const language[]);
native SetServerDetailedPlayers(const DetailedPlayers[][Query_DetailedPlayers], const size = sizeof DetailedPlayers);
native SendPing(bool: send);