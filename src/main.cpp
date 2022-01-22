#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

#include <chrono>
#include <thread>
#include <cstring>

#include "Hooks.h"
#include "Natives.h"
#include "Utils.h"
#include "Data.h"

#define PLUGIN_NAME		"Pawn.Query"
#define PLUGIN_VERSION	"v1.0.1"
#define PLUGIN_AUTHOR	"oukibt"

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET -1
#endif

int HOOK_ProcessQueryPacket(unsigned int addr, unsigned short port, char* data, int len, SOCKET sock)
{
	static sockaddr_in to;

	if ((*(unsigned long*)(data) == 0x504D4153))
	{
		if (len >= 11)
		{
			to.sin_family = AF_INET;
			to.sin_port = htons(port);
			to.sin_addr.s_addr = addr;
			
			switch (data[10])
			{
				case 'p':
				{
					if (Data.GetState(QueryData::Type::ePing) == false || Data.SendPing)
					{
						hook_ProcessQueryPacket.disable();
						reinterpret_cast<int(*)(unsigned int, unsigned short, char*, int, SOCKET)>(ProcessQueryPacketAddr)(addr, port, data, len, sock);
						hook_ProcessQueryPacket.enable();

						return 1;
					}

					break;
				}
				case 'c':
				{
					if (Data.GetState(QueryData::Type::ePlayers) == false)
					{
						hook_ProcessQueryPacket.disable();
						reinterpret_cast<int(*)(unsigned int, unsigned short, char*, int, SOCKET)>(ProcessQueryPacketAddr)(addr, port, data, len, sock);
						hook_ProcessQueryPacket.enable();

						return 1;
					}

					WORD PlayersCount = static_cast<WORD>(Data.Players.size());

					char* newdata = (char*)malloc(13 + (PlayersCount * (30)));
					char* keep_ptr = newdata;

					memcpy(newdata, data, 11);
					newdata += 11;

					memcpy(newdata, &PlayersCount, sizeof(WORD));
					newdata += sizeof(WORD);

					BYTE NameLen;

					for (WORD i = 0; i < PlayersCount; i++)
					{
						NameLen = (BYTE)strlen(Data.Players[i].Name);
						memcpy(newdata, &NameLen, sizeof(BYTE));
						newdata += sizeof(BYTE);
						memcpy(newdata, Data.Players[i].Name, NameLen);
						newdata += NameLen;
						memcpy(newdata, &Data.Players[i].Score, sizeof(DWORD));
						newdata += sizeof(DWORD);
					}

					sendto(sock, keep_ptr, (int)(newdata - keep_ptr), 0, (sockaddr*)&to, sizeof(to));
					free(keep_ptr);

					break;
				}

				case 'r':
				{
					if (Data.GetState(QueryData::Type::eRules) == false)
					{
						hook_ProcessQueryPacket.disable();
						reinterpret_cast<int(*)(unsigned int, unsigned short, char*, int, SOCKET)>(ProcessQueryPacketAddr)(addr, port, data, len, sock);
						hook_ProcessQueryPacket.enable();

						return 1;
					}

					WORD RulesCount = static_cast<WORD>(Data.Rules.size());

					char* newdata = (char*)malloc(13 + (RulesCount * ((MAX_RULE_LENGTH * 2) + 2)));
					char* keep_ptr = newdata;

					memcpy(newdata, data, 11);
					newdata += 11;

					memcpy(newdata, &RulesCount, sizeof(WORD));
					newdata += sizeof(WORD);

					BYTE NameLen;

					for (WORD i = 0; i < RulesCount; i++)
					{
						NameLen = (BYTE)strlen(Data.Rules[i].Name);
						memcpy(newdata, &NameLen, sizeof(BYTE));
						newdata += sizeof(BYTE);
						memcpy(newdata, Data.Rules[i].Name, NameLen);
						newdata += NameLen;

						NameLen = (BYTE)strlen(Data.Rules[i].Value);
						memcpy(newdata, &NameLen, sizeof(BYTE));
						newdata += sizeof(BYTE);
						memcpy(newdata, Data.Rules[i].Value, NameLen);
						newdata += NameLen;
					}

					sendto(sock, keep_ptr, (int)(newdata - keep_ptr), 0, (sockaddr*)&to, sizeof(to));
					free(keep_ptr);

					break;
				}
				
				case 'i':
				{
					if (Data.GetState(QueryData::Type::eInfo) == false)
					{
						hook_ProcessQueryPacket.disable();
						reinterpret_cast<int(*)(unsigned int, unsigned short, char*, int, SOCKET)>(ProcessQueryPacketAddr)(addr, port, data, len, sock);
						hook_ProcessQueryPacket.enable();

						return 1;
					}

					char* newdata = (char*)malloc(13 + (MAX_HOSTNAME_LENGTH + MAX_GAMEMODE_LENGTH + MAX_LANGUAGE_LENGTH) + 17);
					char* keep_ptr = newdata;
					
					WORD PlayerCount = static_cast<WORD>(Data.Players.size());

					memcpy(newdata, data, 11);
					newdata += 11;

					memcpy(newdata, &Data.ServerInfo.Password, sizeof(BYTE));
					newdata += sizeof(BYTE);

					memcpy(newdata, &PlayerCount, sizeof(WORD));
					newdata += sizeof(WORD);

					memcpy(newdata, &Data.ServerInfo.MaxPlayers, sizeof(WORD));
					newdata += sizeof(WORD);

					DWORD NameLen;

					NameLen = strlen(Data.ServerInfo.Hostname);
					memcpy(newdata, &NameLen, sizeof(DWORD));
					newdata += sizeof(DWORD);
					memcpy(newdata, &Data.ServerInfo.Hostname, NameLen);
					newdata += NameLen;

					NameLen = strlen(Data.ServerInfo.Gamemode);
					memcpy(newdata, &NameLen, sizeof(DWORD));
					newdata += sizeof(DWORD);
					memcpy(newdata, &Data.ServerInfo.Gamemode, NameLen);
					newdata += NameLen;

					NameLen = strlen(Data.ServerInfo.Language);
					memcpy(newdata, &NameLen, sizeof(DWORD));
					newdata += sizeof(DWORD);
					memcpy(newdata, &Data.ServerInfo.Language, NameLen);
					newdata += NameLen;
					

					sendto(sock, keep_ptr, (int)(newdata - keep_ptr), 0, (sockaddr*)&to, sizeof(to));
					free(keep_ptr);

					break;
				}

				case 'd':
				{
					if (Data.GetState(QueryData::Type::eDPlayers) == false)
					{
						hook_ProcessQueryPacket.disable();
						reinterpret_cast<int(*)(unsigned int, unsigned short, char*, int, SOCKET)>(ProcessQueryPacketAddr)(addr, port, data, len, sock);
						hook_ProcessQueryPacket.enable();

						return 1;
					}

					WORD PlayersCount = static_cast<WORD>(Data.DetailedPlayers.size());

					char* newdata = (char*)malloc(13 + PlayersCount * (34));
					char* keep_ptr = newdata;

					BYTE NameLen;

					for (WORD i = 0; i < PlayersCount; i++)
					{
						memcpy(newdata, &Data.DetailedPlayers[i].ID, sizeof(BYTE));
						newdata += sizeof(BYTE);

						NameLen = (BYTE)strlen(Data.DetailedPlayers[i].Name);
						memcpy(newdata, &NameLen, sizeof(BYTE));
						newdata += sizeof(BYTE);

						memcpy(newdata, &Data.DetailedPlayers[i].Name, NameLen);
						newdata += NameLen;

						memcpy(newdata, &Data.DetailedPlayers[i].Score, sizeof(DWORD));
						newdata += NameLen;

						memcpy(newdata, &Data.DetailedPlayers[i].Ping, sizeof(DWORD));
						newdata += NameLen;
					}

					sendto(sock, keep_ptr, (int)(newdata - keep_ptr), 0, (sockaddr*)&to, sizeof(to));
					free(keep_ptr);

					break;
				}

				default: return 0;
			}
		}
		else return 0;
	}
	else return 0;

	return 1;
}

extern void *pAMXFunctions;

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];

	logprintf = reinterpret_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);

	logprintf("%s %s by %s loaded.", PLUGIN_NAME, PLUGIN_VERSION, PLUGIN_AUTHOR);

	CPacketHook::Initialize();

	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	logprintf("%s successfully unloaded");
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* amx) 
{
	Natives::Register(amx);
	return 1;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* amx) 
{
	return AMX_ERR_NONE;
}