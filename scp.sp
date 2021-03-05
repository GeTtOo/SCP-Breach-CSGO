#include <sourcemod>
#include <sdkhooks>
#include "scp/classes"

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "SCP GameMode",
    author = "Andrey::Dono, GeTtOo",
    description = "SCP gamemmode for CS:GO",
    version = "0.1",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void OnPluginStart() 
{
    // Declaration in "scp/classes.inc"
    Clients = new ClientSingleton();
    Gamemode = new GameMode();
    
    HookEvent("round_start", OnRoundStart);
    HookEvent("round_end", OnRoundEnd);
    HookEntityOutput("func_button", "OnPressed", Event_OnButtonPressed);
}

public void OnClientJoin(Client ply) {
    PrintToServer("Client connected: %i", ply.id);
}

public void OnClientLeave(Client ply) {
    PrintToServer("Client disconnected: %i", ply.id);
}

public void OnRoundStart(Event ev, const char[] name, bool dbroadcast) 
{
    StringMapSnapshot gClassNameS = Gamemode.GetGlobalClassNames();
    int gClassCount, classCount, extra = 0;
    int keyLen;

    for (int i=0; i < gClassNameS.Length; i++) 
    {
        keyLen = gClassNameS.KeyBufferSize(i);
        char[] gClassKey = new char[keyLen];
        gClassNameS.GetKey(i, gClassKey, keyLen);
        if (json_is_meta_key(gClassKey)) continue;

        GlobalClass gclass = Gamemode.gclass(gClassKey);

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
                player.class(gClassKey);
                player.subclass(classKey);
                player.haveClass = true;

                extra++;
            }
        }
    }

    for (int i=1; i <= Clients.InGame() - extra; i++) 
    {
        Client player = Clients.GetRandomWithoutClass();
        char gclass[32], class[32];
        Gamemode.config.DefaultGlobalClass(gclass, sizeof(gclass));
        Gamemode.config.DefaultClass(class, sizeof(class));
        player.class(gclass);
        player.subclass(class);
        player.haveClass = true;
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

public Action Event_OnButtonPressed(const char[] output, int caller, int activator, float delay)
{
    if(IsClientExist(activator) && IsValidEntity(caller) && IsPlayerAlive(activator) && !IsCleintInSpec(activator))
    {
        Client ply = Clients.Get(activator);

        if(true)
        {
            PrintToChatAll("B_ID: %i", GetEntProp(caller, Prop_Data, "m_iHammerID"));
        }

        StringMapSnapshot doorsSnapshot = Gamemode.config.doors.GetAll();
        int doorKeyLen;

        for (int i = 0; i < doorsSnapshot.Length; i++)
        {
            doorKeyLen = doorsSnapshot.KeyBufferSize(i);
            char[] doorKey = new char[doorKeyLen];
            doorsSnapshot.GetKey(i, doorKey, doorKeyLen);
            
            if (json_is_meta_key(doorKey)) 
                continue;

            // ¯\_(ツ)_/¯
            Door door = Gamemode.config.doors.Get(doorKey);

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
    
    return Plugin_Stop;
}

stock void LoadFileToDownload()
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

stock void RemoveWeapons(int client)
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