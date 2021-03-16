#if defined _scpcore_included
  #endinput
#endif
#define _scpcore_included

// ¯\_(ツ)_/¯
#include "scp/structures/vector"
#include "scp/classes/gamemode"
#include "scp/classes/client"
// ¯\_(ツ)_/¯
ClientSingleton Clients;
GameMode gamemode;
// ¯\_(ツ)_/¯
#include "scp/classes/entity"

native any SCP_GetClient(int client);

forward SCP_OnPlayerJoin()
