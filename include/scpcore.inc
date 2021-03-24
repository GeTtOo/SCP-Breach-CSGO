#if defined _scpcore_included
    #endinput
#endif
#define _scpcore_included

methodmap Base < StringMap {
    
    public Base() { return view_as<Base>(CreateTrie()); }
    
    public bool SetInt(char[] index, int val) { return this.SetValue(index, val); }
    public bool SetFloat(char[] index, float val) { return this.SetValue(index, val); }
    public bool SetBool(char[] index, bool val) { return this.SetValue(index, val); }

    public int GetInt(char[] index, int defval = 0) { int val; return this.GetValue(index, val) ? val : defval; }
    public float GetFloat(char[] index, float defval = 0.0) { float val; return this.GetValue(index, val) ? val : defval; }
    public bool GetBool(char[] index, bool defval = false) { bool val; return this.GetValue(index, val) ? val : defval; }
}

// ¯\_(ツ)_/¯
#include "scp/structures/vector"
#include "scp/classes/gamemode"

GameMode gamemode;

#include "scp/classes/entity"
#include "scp/classes/client"

ClientSingleton Clients;
EntitySingleton Ents;

forward void SCP_OnPlayerJoin(Client &ply);

forward void SCP_OnPlayerLeave(Client &ply);

forward void SCP_OnPlayerSpawn(Client &ply);

forward Action SCP_OnTakeDamage(Client vic, Client atk, float &damage, int &damagetype);

forward void SCP_OnButtonPressed(Client ply, int doorId);

public any NativeGameMode_GetGClass(Handle Plugin, int numArgs) {
    char name[32];
    GetNativeString(2, name, sizeof(name));
    return view_as<GlobalClasses>(gamemode.GetObject("GlobalClasses")).get(name);
}

public any NativeClients_Get(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    Client clients[MAXPLAYERS + 1];
    Clients.GetArray("Clients", clients, sizeof(clients));
    if (id <= MAXPLAYERS)
        return view_as<Client>(clients[id]);
    else
        return view_as<Client>(null);
}

public any NativeClients_GetRandom(Handle Plugin, int numArgs) {
    Client clients[MAXPLAYERS + 1];
    Clients.GetArray("Clients", clients, sizeof(clients));
    int random = GetRandomInt(1, Clients.InGame());
    return view_as<Client>(clients[random]);
}

public any NativeClients_InGame(Handle Plugin, int numArgs) {
    int client = 1;
    while (IsClientInGame(client) && GetClientTeam(client) > 1)
        client++;
    client--;
    return client;
}

public any NativeClients_Alive(Handle Plugin, int numArgs) {
    int client = 1;
    int clientAlive = 1;
    while (IsClientInGame(client) && GetClientTeam(client) > 1) {
        if (IsPlayerAlive(client))
            clientAlive++;
        client++;
    }
    clientAlive--;
    return clientAlive;
}

public any NativeEntities_Get(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    ArrayList entArr;
    Ents.GetValue("Entities", entArr);

    for(int i=0; i < entArr.Length; i++)
        if (view_as<Entity>(entArr.Get(i, 0)).id == id)
            return view_as<Entity>(entArr.Get(i, 0));

    return view_as<Entity>(0);
}