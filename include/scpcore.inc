#if defined _scpcore_included
  #endinput
#endif
#define _scpcore_included

// ¯\_(ツ)_/¯
#include "scp/structures/vector"
#include "scp/classes/gamemode"
#include "scp/classes/client"
ClientSingleton Clients;
GameMode gamemode;
#include "scp/classes/entity"

native any SCP_GetClient(int client);

forward void SCP_OnPlayerJoin(Client &ply);

forward void SCP_OnPlayerLeave(Client &ply);

forward void SCP_OnPlayerSpawn(Client &ply);

forward Action SCP_OnTakeDamage(Client vic, Client atk, float &damage, int &damagetype);

forward void SCP_OnButtonPressed(Client ply, int doorId);
