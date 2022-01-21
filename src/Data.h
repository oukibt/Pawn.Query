#ifndef DATA_H
#define DATA_H

#define MAX_PLAYER_NAME			24
#define MAX_RULE_LENGTH			30

#define MAX_HOSTNAME_LENGTH		60
#define MAX_GAMEMODE_LENGTH		50
#define MAX_LANGUAGE_LENGTH		30

#define QUERY_TYPES				5

#include <vector>

class QueryPlayer
{
public:
	char Name[MAX_PLAYER_NAME];
	int Score;
};

class QueryRule
{
public:
	char Name[MAX_RULE_LENGTH],
		Value[MAX_RULE_LENGTH];
};

class QueryInformation
{
public:
	bool Password;
	unsigned short MaxPlayers;
	char Hostname[MAX_HOSTNAME_LENGTH];
	char Gamemode[MAX_GAMEMODE_LENGTH];
	char Language[MAX_LANGUAGE_LENGTH];
};

class QueryDetailedPlayer
{
public:
	unsigned char ID;
	char Name[MAX_PLAYER_NAME];
	int Score;
	int Ping;
};

class QueryData
{
	bool Hooked[5];
public:

	enum class Type
	{
		eInfo = 0,
		eRules,
		ePlayers,
		eDPlayers,
		ePing,
	};

	bool SendPing = true;

	bool GetState(Type type)
	{
		return Hooked[static_cast<char>(type)];
	};

	void SetState(char type, bool state)
	{
		Hooked[type] = state;
	}

	std::vector<QueryPlayer> Players;
	std::vector<QueryRule> Rules;
	std::vector<QueryDetailedPlayer> DetailedPlayers;
	QueryInformation ServerInfo;
};

extern QueryData Data;

#endif