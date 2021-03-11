#pragma semicolon 1
#pragma newdecls required

#include "scp/classes/client"
#include "scp/classes/gm"

ClientSingleton Clients;
GameMode gamemode;

//-----------Advanced hooks-----------//

public void OnClientConnected(int id) {
    Clients.Add(id);
}

public void OnClientPostAdminCheck(int id) {
    OnClientJoin(Clients.Get(id));
}

public void OnClientDisconnect(int id) {
    OnClientLeave(Clients.Get(id));

    Clients.Remove(id);
}

public void SpawnPlayer(int id) {
    if (IsClientExist(id) && GetClientTeam(id) == 1)
        PlayerSpawn(Clients.Get(id));
}