#pragma semicolon 1
#pragma newdecls required

#include "scp/classes/client"
#include "scp/classes/gm"


ClientSingleton Clients;
GameMode gamemode;

public void OnClientPostAdminCheck(int id) {
    Clients.Add(id);

    OnClientJoin(Clients.Get(id)); //Hook call
}

public void OnClientDisconnect(int id) {
    OnClientLeave(Clients.Get(id)); //Hook call

    Clients.Remove(id);
}

void SpawnPlayer(int id) {
    if (IsClientExist(id) && GetClientTeam(id) == 3)
        PlayerSpawn(Clients.Get(id)); //Hook call
}