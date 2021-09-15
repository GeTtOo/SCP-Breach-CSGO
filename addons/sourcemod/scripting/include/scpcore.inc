#if defined _scpcore_included
    #endinput
#endif
#define _scpcore_included

#pragma dynamic 131072

// ¯\_(ツ)_/¯
#include "scp/structures/base"
#include "scp/structures/vector"
#include "scp/structures/color"
#include "scp/structures/json"
#include "scp/classes/meta"
#include "scp/classes/gamemode"

GameMode gamemode;

#include "scp/classes/entity"
#include "scp/classes/client"

#include "scp/classes/singletons"

#include "scp/classes/worldtext"

ClientSingleton Clients;
EntitySingleton Ents;
WorldTextSingleton WT;

#include "scp/classes/manager"
#include "scp/classes/nuke"
#include "scp/classes/logger"

forward void SCP_OnPlayerJoin(Client &ply);

forward void SCP_OnPlayerLeave(Client &ply);

forward void SCP_OnPlayerSpawn(Client &ply);

forward void SCP_OnPlayerReset(Client &ply);

forward void SCP_OnPlayerClear(Client &ply);

forward Action SCP_OnTakeDamage(Client vic, Client atk, float &damage, int &damagetype);

forward void SCP_OnPlayerDeath(Client &vic, Client &atk);

forward void SCP_OnButtonPressed(Client &ply, int doorId);

forward void SCP_OnRoundStart();

forward void SCP_OnRoundEnd();

forward void SCP_OnInput(Client &ply, int buttons);

forward void SCP_OnPRessF(Client &ply);

forward void SCP_RegisterMetaData();

public any NativeGameMode_Config(Handle Plugin, int numArgs) { return view_as<Config>(gamemode.GetObject("Config")); }

public any NativeGameMode_Meta(Handle Plugin, int numArgs) { return view_as<Meta>(gamemode.GetObject("Meta")); }

public any NativeGameMode_Manager(Handle Plugin, int numArgs) { return view_as<Manager>(gamemode.GetObject("Manager")); }

public any NativeGameMode_Nuke(Handle Plugin, int numArgs) { return view_as<NuclearWarhead>(gamemode.GetObject("Nuke")); }

public any NativeGameMode_Timers(Handle Plugin, int numArgs) { return view_as<Timers>(gamemode.GetObject("Timers")); }

public any NativeGameMode_Logger(Handle Plugin, int numArgs) { return view_as<Logger>(gamemode.GetObject("Logger")); }

public any NativeGameMode_GetTeam(Handle Plugin, int numArgs) {
    char name[32];
    GetNativeString(2, name, sizeof(name));
    return view_as<Teams>(gamemode.GetObject("Teams")).get(name);
}

public any NativeClients_GetAll(Handle Plugin, int numArgs) {
    ArrayList clients = Clients.GetList("Clients");
    ArrayList players = new ArrayList();
    for (int i=0; i < clients.Length; i++)
        if (view_as<Client>(clients.Get(i)) != null)
            players.Push(clients.Get(i, 1));
    return players;
}

public any NativeClients_Get(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    int idx = Clients.GetList("Clients").FindValue(id, 0);
    if (idx == -1) return view_as<Client>(null);
    return Clients.GetList("Clients").Get(idx, 1);
}

public any NativeClients_GetRandom(Handle Plugin, int numArgs) {
    ArrayList sortedPlayers = new ArrayList();
    ArrayList players = Clients.GetAll();
    for (int i=0; i < players.Length; i++) {
        Client player = players.Get(i);
        if (player.IsAlive())
            sortedPlayers.Push(players.Get(i));
    }

    return sortedPlayers.Get(GetRandomInt(0, sortedPlayers.Length-1));
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

    EntityMeta entdata = gamemode.meta.GetEntity(EntName);
    
    Entity entity;
    if (entdata != null)
    {
        entity = new Entity(CreateEntityByName("prop_physics_override"));

        char modelName[128], mass[12], name[32], bodygroup[12], skin[12];
        entdata.model(modelName, sizeof(modelName));
        entdata.name(name, sizeof(name));
        FloatToString(entdata.mass, mass, sizeof(mass));
        IntToString(entdata.bodygroup, bodygroup, sizeof(bodygroup));
        IntToString(entdata.skin, skin, sizeof(skin));
        
        entity.InvName(name);

        DispatchKeyValue(entity.id, "model", modelName);
        DispatchKeyValue(entity.id, "physicsmode", "2");
        DispatchKeyValue(entity.id, "massScale", mass);
        DispatchKeyValue(entity.id, "Solid", "6");
        DispatchKeyValue(entity.id, "body", bodygroup);
        DispatchKeyValue(entity.id, "skin", skin);
        if (entdata.spawnflags != 0) {
            char flags[8];
            IntToString(entdata.spawnflags, flags, sizeof(flags));
            DispatchKeyValue(entity.id, "spawnflags", flags);
        }
        if (entdata.usable) {
            char funcname[32];
            entdata.GetString("function", funcname, sizeof(funcname));
        }
    }
    else
    {
        entity = new Entity(CreateEntityByName(EntName));
    }

    entity.active = false;
    entity.SetString("class", EntName);
    
    if (view_as<bool>(GetNativeCell(3)))
        Ents.GetList("entities").Push(entity);

    return entity;
}

public any NativeEntities_Remove(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);

    ArrayList entArr;
    Ents.GetValue("entities", entArr);

    for(int i=0; i < entArr.Length; i++)
        if (view_as<Entity>(entArr.Get(i, 0)).id == id) {
            view_as<Entity>(entArr.Get(i, 0)).Remove();
            entArr.Erase(i);
        }
}

public any NativeEntities_Get(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    ArrayList entArr;
    Ents.GetValue("entities", entArr);

    for(int i=0; i < entArr.Length; i++)
        if (view_as<Entity>(entArr.Get(i)).id == id)
            return view_as<Entity>(entArr.Get(i));

    return view_as<Entity>(null);
}

public any NativeEntities_TryGetOrAdd(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    ArrayList entArr;
    Ents.GetValue("entities", entArr);

    for(int i=0; i < entArr.Length; i++)
        if (view_as<Entity>(entArr.Get(i)).id == id)
            return view_as<Entity>(entArr.Get(i));

    return view_as<Entity>(Ents.Add(id));
}