#if defined _scpcore_included
    #endinput
#endif
#define _scpcore_included

#pragma dynamic 131072

// ¯\_(ツ)_/¯
#include "scp\structures\base"
#include "scp\structures\vector"
#include "scp\structures\angle"
#include "scp\structures\color"
#include "scp\structures\json"

#include "scp\classes\meta"
#include "scp\classes\gamemode"
#include "scp\classes\logger"

GameMode gamemode;

#include "scp\classes\entity"
#include "scp\classes\client"
#include "scp\classes\singletons"

#include "scp\classes\worldtext"

ClientSingleton Clients;
EntitySingleton Ents;
WorldTextSingleton WT;

#include "scp\classes\manager"
#include "scp\classes\nuke"

forward void SCP_RegisterMetaData();

forward void SCP_OnLoad();

forward void SCP_OnUnload();

forward void SCP_OnPlayerJoin(Client &ply);

forward void SCP_OnPlayerLeave(Client &ply);

forward void SCP_OnPlayerSpawn(Client &ply);

forward void SCP_OnPlayerReset(Client &ply);

forward void SCP_OnPlayerClear(Client &ply);

forward Action SCP_OnTakeDamage(Client &vic, Client &atk, float &damage, int &damagetype);

forward void SCP_OnPlayerDeath(Client &vic, Client &atk);

forward void SCP_OnButtonPressed(Client &ply, int doorId);

forward void SCP_OnRoundStart();

forward void SCP_OnRoundEnd();

forward void SCP_OnInput(Client &ply, int buttons);

forward void SCP_OnCallActionMenu(Client &ply);