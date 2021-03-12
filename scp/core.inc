#pragma semicolon 1
#pragma newdecls required

#include "scp/classes/gm"
#include "scp/classes/client"

ClientSingleton Clients;
GameMode gamemode;

public void OnPluginStart() 
{
    // Declaration in "scp/classes.inc"
    Clients = new ClientSingleton();
    gamemode = new GameMode();

    HookEvent("player_spawn", Event_PlayerSpawn, EventHookMode_Post);

    OnPluginLoad();
}

//---------------Events---------------//

public Action Event_PlayerSpawn(Event event, const char[] name, bool dontBroadcast)
{
    Client ply = Clients.Get(GetClientOfUserId(GetEventInt(event, "userid")));

    if (IsClientExist(ply.id))
        CreateTimer(0.2, Timer_PlayerSpawn, ply, TIMER_FLAG_NO_MAPCHANGE);

    return Plugin_Continue;
}

public Action Timer_PlayerSpawn(Handle hTimer, Client ply)
{
    if(IsClientExist(ply.id)) {
        int m_hMyWeapons_size = GetEntPropArraySize(ply.id, Prop_Send, "m_hMyWeapons");
        int item; 

        for(int index = 0; index < m_hMyWeapons_size; index++) 
        { 
            item = GetEntPropEnt(ply.id, Prop_Send, "m_hMyWeapons", index);

            if(item != -1) 
            { 
                RemovePlayerItem(ply.id, item);
                AcceptEntityInput(item, "Kill");
            } 
        }

        OnPlayerSpawn(ply);
    }
}

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

//--------------Functions------------//

stock bool IsClientExist(int client)
{
    if((0 < client < MaxClients) && IsClientInGame(client) && !IsClientSourceTV(client))
    {
        return true;
    }

    return false;
}

stock bool IsCleintInSpec(int client)
{
    if(GetClientTeam(client) != 1)
    {
        return false;
    }

    return true;
}

stock bool IsWarmup()
{
    if(GameRules_GetProp("m_bWarmupPeriod"))
    {
        return true;
    }

    return false;
}