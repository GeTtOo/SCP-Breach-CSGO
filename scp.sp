#include <sourcemod>
#include <cstrike>
#include <sdkhooks>
#include "scp/classes"

#pragma semicolon 1
#pragma newdecls required

#define HIDE_RADAR_CSGO 1<<12

bool g_AllowRoundEnd = false;

public Plugin myinfo = {
    name = "SCP gamemode",
    author = "Andrey::Dono, GeTtOo",
    description = "SCP gamemmode for CS:GO",
    version = "0.1",
    url = "https://github.com/GeTtOo/csgo_scp"
};

//////////////////////////////////////////////////////////////////////////////
//
//                                Main
//
//////////////////////////////////////////////////////////////////////////////

public void OnPluginStart() 
{
    // Declaration in "scp/classes.inc"
    Clients = new ClientSingleton();
    gamemode = new GameMode();
    
    HookEvent("round_start", OnRoundStart);
    HookEvent("round_end", OnRoundEnd);
    HookEntityOutput("func_button", "OnPressed", Event_OnButtonPressed);

    LoadFileToDownload();
}

//////////////////////////////////////////////////////////////////////////////
//
//                                Events
//
//////////////////////////////////////////////////////////////////////////////

public void OnMapStart() {
    char mapName[128];
    GetCurrentMap(mapName, sizeof(mapName));
    gamemode.config.SetDoorRules(mapName);
}

public void OnClientJoin(Client ply) {
    if (gamemode.config.debug)
        PrintToServer("Client joined - localId: (%i), steamId: (%i)", ply.id, GetSteamAccountID(ply.id));

    SDKHook(ply.id, SDKHook_SpawnPost, OnPlayerSpawnPost);
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void OnClientLeave(Client ply) {
    if (gamemode.config.debug)
        PrintToServer("Client disconnected: %i", ply.id);
}

public void OnRoundStart(Event ev, const char[] name, bool dbroadcast) 
{
    if(!IsWarmup())
    {
        g_AllowRoundEnd = false;
        
        StringMapSnapshot gClassNameS = gamemode.GetGlobalClassNames();
        int gClassCount, classCount, extra = 0;
        int keyLen;

        for (int i=0; i < gClassNameS.Length; i++) 
        {
            keyLen = gClassNameS.KeyBufferSize(i);
            char[] gClassKey = new char[keyLen];
            gClassNameS.GetKey(i, gClassKey, keyLen);
            if (json_is_meta_key(gClassKey)) continue;

            GlobalClass gclass = gamemode.gclass(gClassKey);

            gClassCount = Clients.InGame() * gclass.percent / 100;
            gClassCount = (gClassCount != 0 || !gclass.priority) ? gClassCount : 1;
            
            StringMapSnapshot classNameS = gclass.GetClassNames();
            int classKeyLen;

            for (int v=0; v < classNameS.Length; v++) 
            {
                classKeyLen = classNameS.KeyBufferSize(v);
                char[] classKey = new char[classKeyLen];
                classNameS.GetKey(v, classKey, classKeyLen);
                if (json_is_meta_key(classKey)) continue;

                Class class = gclass.class(classKey);

                classCount = gClassCount * class.percent / 100;
                classCount = (classCount != 0 || !class.priority) ? classCount : 1;

                for (int scc=1; scc <= classCount; scc++) 
                {
                    if (extra > Clients.InGame()) break;
                    Client player = Clients.GetRandomWithoutClass();
                    player.gclass(gClassKey);
                    player.class(classKey);
                    player.haveClass = true;

                    extra++;
                }
            }
        }

        for (int i=1; i <= Clients.InGame() - extra; i++) 
        {
            Client player = Clients.GetRandomWithoutClass();
            char gclass[32], class[32];
            gamemode.config.DefaultGlobalClass(gclass, sizeof(gclass));
            gamemode.config.DefaultClass(class, sizeof(class));
            player.gclass(gclass);
            player.class(class);
            player.haveClass = true;
        }
    }
}

public void OnRoundEnd(Event ev, const char[] name, bool dbroadcast) 
{
    for (int cig=1; cig <= Clients.InGame(); cig++) 
    {
        Client client = Clients.Get(cig);
        client.haveClass = false;
    }
}

public Action CS_OnTerminateRound(float& delay, CSRoundEndReason& reason)
{
    if(IsWarmup())
	{
		return Plugin_Continue;
	}
    else if(g_AllowRoundEnd)
    {
        return Plugin_Continue;
    }
    else
    {
        return Plugin_Handled;
    }
}

public Action Event_OnButtonPressed(const char[] output, int caller, int activator, float delay)
{
    if(IsClientExist(activator) && IsValidEntity(caller) && IsPlayerAlive(activator) && !IsCleintInSpec(activator))
    {
        Client ply = Clients.Get(activator);

        if (gamemode.config.debug)
            PrintToChatAll("Door/Button id: (%i)", GetEntProp(caller, Prop_Data, "m_iHammerID"));

        StringMapSnapshot doorsSnapshot = gamemode.config.doors.GetAll();
        int doorKeyLen;

        for (int i = 0; i < doorsSnapshot.Length; i++)
        {
            doorKeyLen = doorsSnapshot.KeyBufferSize(i);
            char[] doorKey = new char[doorKeyLen];
            doorsSnapshot.GetKey(i, doorKey, doorKeyLen);
            
            if (json_is_meta_key(doorKey)) 
                continue;

            // ¯\_(ツ)_/¯
            Door door = gamemode.config.doors.Get(doorKey);

            if(GetEntProp(caller, Prop_Data, "m_iHammerID") == StringToInt(doorKey))
            {
                /*if(g_IgnoreDoorAccess[activator] == true)
                {
                    return Plugin_Continue;
                } */
                if(ply.IsSCP)
                {
                    if(door.scp)
                    {
                        return Plugin_Stop;
                    }
                }
                else if(ply.card >= door.access)
                {
                    return Plugin_Continue;
                }
                else
                {
                    return Plugin_Stop;
                }
            }
        }
    }
    
    return Plugin_Continue;
}

public Action Event_PlayerSpawn(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(GetEventInt(event, "userid"));

    if(IsClientExist(client))
    {
        CreateTimer(0.2, OnPlayerSpawn, client, TIMER_FLAG_NO_MAPCHANGE);
    }

    return Plugin_Continue;
}

public Action OnPlayerSpawnPost(int client)
{
    SetEntProp(client, Prop_Send, "m_iHideHUD", HIDE_RADAR_CSGO);
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{   
    if(IsClientExist(victim) && IsClientExist(attacker))
    {
        Client vic = Clients.Get(victim);
        Client atk = Clients.Get(attacker);
        
        if(vic.IsSCP && atk.IsSCP)
        {
            return Plugin_Stop;
        }
    }

    return Plugin_Continue;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 Timers
//
//////////////////////////////////////////////////////////////////////////////

public Action OnPlayerSpawn(Handle hTimer, any client)
{
    if(IsClientExist(client) && IsPlayerAlive(client))
    {
        Client ply = Clients.Get(client);
        /*if(!g_AllowPlayerAppear && !g_RespawnProtect[client])
        {
            ForcePlayerSuicide(client);
        }*/

        RemoveWeapons(client);
        //SetEntData(client, g_offsCollisionGroup, 2, 4, true);
        //g_PlayerCard[client] = CARD_LEVEL_NON;
        ply.card = 0;
        //g_RespawnProtect[client] = false;
        SpawnPlayer(client);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//                               Load Configs 
//
//////////////////////////////////////////////////////////////////////////////

void LoadFileToDownload()
{
    Handle hFile = OpenFile("addons/sourcemod/configs/scp/downloads.txt", "r");
    
    if(hFile)
    {
        char buffer[PLATFORM_MAX_PATH];
        while(!IsEndOfFile(hFile) && ReadFileLine(hFile, buffer, sizeof(buffer)))
        {
            if(TrimString(buffer) > 2 && IsCharAlpha(buffer[0]))
            {
                AddFileToDownloadsTable(buffer);
            }
        }

        CloseHandle(hFile);
    }
    else
    {
        LogError("Can't find downloads.txt");
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//                                Functions
//
//////////////////////////////////////////////////////////////////////////////

public void PlayerSpawn(Client ply)
{
    //EquipPlayerWeapon(client, GivePlayerItem(client, "weapon_fists"));
    
    //ply.Loadouts = [];
    char gclass[32], class[32];
    ply.gclass(gclass, sizeof(gclass));
    ply.class(class, sizeof(class));
    ply.health = gamemode.gclass(gclass).class(class).health;
    ply.speed = gamemode.gclass(gclass).class(class).speed;
    ply.armor = gamemode.gclass(gclass).class(class).armor;

    // Set player and hands model
    char model[256], handsModel[256];
    gamemode.gclass(gclass).class(class).Model(model, sizeof(model));
    gamemode.gclass(gclass).class(class).HandsModel(handsModel, sizeof(handsModel));
    
    ply.SetModel(model);
    ply.SetHandsModel(handsModel);

    // ply.armor = gamemode.class(ply.class).subclass(ply.subclass).weapon_1;
    // ply.armor = gamemode.class(ply.class).subclass(ply.subclass).weapon_pistol;
    // ply.armor = gamemode.class(ply.class).subclass(ply.subclass).weapon_granade;

    // Teleport player to pos 
}

void RemoveWeapons(int client)
{
    int m_hMyWeapons_size = GetEntPropArraySize(client, Prop_Send, "m_hMyWeapons");
    int item; 

    for(int index = 0; index < m_hMyWeapons_size; index++) 
    { 
        item = GetEntPropEnt(client, Prop_Send, "m_hMyWeapons", index); 

        if(item != -1) 
        { 
            RemovePlayerItem(client, item);
            AcceptEntityInput(item, "Kill");
        } 
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//                              Event validation
//
//////////////////////////////////////////////////////////////////////////////

stock bool IsClientExist(int client)
{
    if((0 < client < MaxClients) && IsClientInGame(client) && !IsFakeClient(client) && !IsClientSourceTV(client))
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