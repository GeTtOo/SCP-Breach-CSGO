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
#include "scp/classes/logger"

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

forward void SCP_RegisterMetaData();

public any NativeGameMode_Config(Handle Plugin, int numArgs) { return view_as<Config>(view_as<JSON_OBJECT>(gamemode).GetObject("Config")); }

public any NativeGameMode_Meta(Handle Plugin, int numArgs) { return view_as<Meta>(view_as<JSON_OBJECT>(gamemode).GetObject("Meta")); }

public any NativeGameMode_Manager(Handle Plugin, int numArgs) { return view_as<Manager>(view_as<JSON_OBJECT>(gamemode).GetObject("Manager")); }

public any NativeGameMode_Nuke(Handle Plugin, int numArgs) { return view_as<NuclearWarhead>(view_as<JSON_OBJECT>(gamemode).GetObject("Nuke")); }

public any NativeGameMode_Timers(Handle Plugin, int numArgs) { return view_as<Timers>(view_as<JSON_OBJECT>(gamemode).GetObject("Timers")); }

public any NativeGameMode_Logger(Handle Plugin, int numArgs) { return view_as<Logger>(view_as<JSON_OBJECT>(gamemode).GetObject("Logger")); }

public any NativeGameMode_TeamList(Handle Plugin, int numArgs) {
    bool filter = GetNativeCell(2);
    ArrayList list = new ArrayList(32);
    StringMapSnapshot snap = view_as<JSON_OBJECT>(gamemode).GetObject("Teams").Snapshot();
    int keylength;
    for (int i=0; i < snap.Length; i++) {
        keylength = snap.KeyBufferSize(i);
        char[] teamName = new char[keylength];
        snap.GetKey(i, teamName, keylength);
        if (json_is_meta_key(teamName)) continue;
        if (filter && gamemode.team(teamName).percent == 0) continue;
        list.PushString(teamName);
    }
    delete snap;
    return list;
}

public any NativeGameMode_GetTeam(Handle Plugin, int numArgs) {
    char name[32];
    GetNativeString(2, name, sizeof(name));
    return view_as<Teams>(view_as<JSON_OBJECT>(gamemode).GetObject("Teams")).get(name);
}

public any NativeClients_Add(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    any data[2];
    data[0] = id;
    data[1] = new Client(id);
    Ents.list.PushArray(data);
}

public any NativeClients_Remove(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    ArrayList ents = Ents.list;
    int idx = ents.FindValue(id, 0);
    view_as<Client>(ents.Get(idx, 1)).Dispose();
    ents.Erase(idx);
}

public any NativeClients_Get(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    ArrayList ents = Ents.list;
    int idx = ents.FindValue(id, 0);
    if (idx == -1) return view_as<Client>(null);
    return ents.Get(idx, 1);
}

public any NativeClients_GetAll(Handle Plugin, int numArgs) {
    ArrayList ents = Ents.list;

    ArrayList players = new ArrayList();
    for (int i=0; i < ents.Length; i++)
    {
        int id = ents.Get(i, 0);
        if (id <= MaxClients && id > 0)
        {
            Client ply = ents.Get(i, 1);
            if (ply != null)
                players.Push(ply);
        }
    }
    return players;
}

public any NativeClients_GetRandom(Handle Plugin, int numArgs) {
    ArrayList sortedPlayers = new ArrayList();
    ArrayList players = Clients.GetAll();
    for (int i=0; i < players.Length; i++) {
        Client player = players.Get(i);
        if (player.IsAlive())
            sortedPlayers.Push(players.Get(i));
    }

    return sortedPlayers.Get(GetRandomInt(0, sortedPlayers.Length - 1));
}

public any NativeClients_InGame(Handle Plugin, int numArgs) {
    int client = 1;
    while (IsClientInGame(client) && GetClientTeam(client) >= 2)
        client++;
    client--;
    return client;
}

public any NativeClients_Alive(Handle Plugin, int numArgs) {
    int client = 1;
    int clientAlive = 1;
    while (IsClientInGame(client) && GetClientTeam(client) >= 2) {
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
        entity = new Entity();
        entity.meta = entdata;
        entity.Create();
    }
    else
    {
        entity = new Entity(CreateEntityByName(EntName));
    }

    entity.spawned = false;
    entity.SetClass(EntName);
    
    if (view_as<bool>(GetNativeCell(3)))
    {
        any data[2];
        data[0] = entity.id;
        data[1] = entity;

        Ents.list.PushArray(data);
    }

    return entity;
}

public any NativeEntities_Remove(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    ArrayList ents = Ents.list;
    int idx = ents.FindValue(id, 0);
    view_as<Entity>(ents.Get(idx, 1)).Remove();
    Ents.list.Erase(idx);
}

public any NativeEntities_IndexUpdate(Handle Plugin, int numArgs) {
    Entity ent = GetNativeCell(2);
    ArrayList ents = Ents.list;
    int idx = ents.FindValue(ent, 1);
    if (idx != -1)
    {
        ents.Set(idx, ent.id, 0);
        return true;
    }
    else
        return false;
}

public any NativeEntities_Clear(Handle Plugin, int numArgs) {
    ArrayList ents = Ents.list;

    for(int i=0; i < ents.Length; i++)
    {
        int id = ents.Get(i, 0);

        if (id > MaxClients)
        {
            Entity ent = ents.Get(i, 1);
            ent.Dispose();
        }
    }
    
    ents.Resize(Clients.InGame());
}

public any NativeEntities_Get(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    ArrayList ents = Ents.list;
    int idx = ents.FindValue(id, 0);
    if (idx == -1) return view_as<Entity>(null);
    return ents.Get(idx, 1);
}

public any NativeEntities_GetAll(Handle Plugin, int numArgs) {
    ArrayList entities = Ents.list;
    ArrayList ents = new ArrayList();
    for (int i=0; i < entities.Length; i++)
    {
        Entity ent = entities.Get(i, 1);
        if (ent != null)
            ents.Push(ent);
    }
    return ents;
}

public any NativeEntities_TryGet(Handle Plugin, int numArgs) {
    int id = GetNativeCell(2);
    ArrayList ents = Ents.list;
    int idx = ents.FindValue(id, 0);
    if (idx == -1) return false;
    SetNativeCellRef(3, ents.Get(idx, 1));
    return true;
}