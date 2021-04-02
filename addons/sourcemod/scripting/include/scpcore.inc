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
    public ArrayList GetList(char[] index) { ArrayList list; return this.GetValue(index, list) ? list : null; }
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

forward void SCP_OnPlayerDeath(Client &vic, Client &atk);

forward void SCP_OnButtonPressed(Client &ply, int doorId);

public any NativeGameMode_Config(Handle Plugin, int numArgs) {
    return view_as<Config>(gamemode.GetObject("Config"));
}

public any NativeGameMode_Manager(Handle Plugin, int numArgs) {
    return view_as<Manager>(gamemode.GetObject("Manager"));
}

public any NativeGameMode_Timers(Handle Plugin, int numArgs) {
    return view_as<Timers>(gamemode.GetObject("Timers"));
}

public any NativeGameMode_GetTeam(Handle Plugin, int numArgs) {
    char name[32];
    GetNativeString(2, name, sizeof(name));
    return view_as<Teams>(gamemode.GetObject("Teams")).get(name);
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

public any NativeEntities_Create(Handle Plugin, int numArgs) {
    char EntName[32];
    GetNativeString(2, EntName, sizeof(EntName));
    
    Entity entity;
    if (gamemode.entities.HasKey(EntName))
    {
        EntConfig entcfg = gamemode.entities.get(EntName);

        entity = new Entity(CreateEntityByName("prop_physics_override"));

        char modelName[128], mass[12], name[32];
        entcfg.model(modelName, sizeof(modelName));
        entcfg.mass(mass, sizeof(mass));
        entcfg.name(name, sizeof(name));
        
        entity.name(name);

        DispatchKeyValue(entity.id, "model", modelName);
        DispatchKeyValue(entity.id, "physicsmode", "2");
        DispatchKeyValue(entity.id, "massScale", mass);
        DispatchKeyValue(entity.id, "Solid", "6");
        if (entcfg.spawnflags != 0) {
            char flags[8];
            IntToString(entcfg.spawnflags, flags, sizeof(flags));
            DispatchKeyValue(entity.id, "spawnflags", flags);
        }
    }
    else
    {
        entity = new Entity(CreateEntityByName(EntName));
    }

    entity.active = false;
    entity.SetString("class", EntName);

    ArrayList entArr;
    Ents.GetValue("Entities", entArr);

    PushArrayCell(entArr, entity);

    return entity;
}

public any NativeEntities_Remove(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);

    ArrayList entArr;
    Ents.GetValue("Entities", entArr);

    for(int i=0; i < entArr.Length; i++)
        if (view_as<Entity>(entArr.Get(i, 0)).id == id) {
            view_as<Entity>(entArr.Get(i, 0)).Remove();
            entArr.Erase(i);
        }
}

public any NativeEntities_Get(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    ArrayList entArr;
    Ents.GetValue("Entities", entArr);

    for(int i=0; i < entArr.Length; i++)
        if (view_as<Entity>(entArr.Get(i)).id == id)
            return view_as<Entity>(entArr.Get(i));

    return view_as<Entity>(null);
}

public any NativeEntities_TryGetOrAdd(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    ArrayList entArr;
    Ents.GetValue("Entities", entArr);

    for(int i=0; i < entArr.Length; i++)
        if (view_as<Entity>(entArr.Get(i)).id == id)
            return view_as<Entity>(entArr.Get(i));

    return view_as<Entity>(Ents.Add(id));
}