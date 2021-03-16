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

forward void SCP_OnPlayerJoin(Client ply)
forward void SCP_OnPlayerLeave(Client ply)
forward void SCP_OnPlayerSpawn(Client ply)
forward void SCP_OnTakeDamage(Client vic, Client atk, float damage)
forward void SCP_OnButtonPressed(Client ply, int doorId)
