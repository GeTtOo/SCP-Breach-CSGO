#include <sourcemod>
#include <cstrike>
#include <sdkhooks>

#include "scp/core"

#pragma semicolon 1
#pragma newdecls required

#define HIDE_RADAR_CSGO 1<<12

bool g_IgnoreDoorAccess[MAXPLAYERS+1];
bool g_AllowRoundEnd = false;
bool g_IsNuckExpl = false;
int g_offsCollisionGroup;

Handle OnClientJoinForward;
Handle OnClientLeaveForward;
Handle OnClientSpawnForward;
Handle OnTakeDamageForward;
Handle OnButtonPressedForward;

public Plugin myinfo = {
    name = "[SCP] GameMode",
    author = "Andrey::Dono, GeTtOo",
    description = "SCP gamemmode for CS:GO",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

//////////////////////////////////////////////////////////////////////////////
//
//                                Main
//
//////////////////////////////////////////////////////////////////////////////

public void OnPluginLoad() 
{   
    AddCommandListener(OnLookAtWeaponPressed, "+lookatweapon");
    AddCommandListener(GetClientPos, "getmypos");
    AddCommandListener(TpTo914, "tp914");

    RegAdminCmd("scp_admin", Command_AdminMenu, ADMFLAG_BAN);
    
    HookEvent("round_start", OnRoundStart);
    HookEvent("round_end", OnRoundEnd);
    HookEvent("player_death", Event_PlayerDeath, EventHookMode_Pre);
    HookEntityOutput("func_button", "OnPressed", Event_OnButtonPressed);
    HookEntityOutput("trigger_teleport", "OnStartTouch", Event_OnTriggerActivation);

    g_offsCollisionGroup = FindSendPropInfo("CBaseEntity", "m_CollisionGroup");

    LoadFileToDownload();
}

public APLRes AskPluginLoad2(Handle self, bool late, char[] error, int err_max) {
    CreateNative("SCP_GetClient", NativeGetClient);
    
    OnClientJoinForward = CreateGlobalForward("SCP_OnPlayerJoin", ET_Event, Param_Cell);
    OnClientLeaveForward = CreateGlobalForward("SCP_OnPlayerLeave", ET_Event, Param_Cell);
    OnClientSpawnForward = CreateGlobalForward("SCP_OnPlayerSpawn", ET_Event, Param_Cell);
    OnTakeDamageForward = CreateGlobalForward("SCP_OnTakeDamage", ET_Event, Param_Cell, Param_Cell, Param_Float);
    OnButtonPressedForward = CreateGlobalForward("SCP_OnButtonPressed", ET_Event, Param_Cell, Param_Cell);

    RegPluginLibrary("scp_core");
    return APLRes_Success;
}

public any NativeGetClient(Handle plugin, int numArgs)
{ 
    return Clients.Get(GetNativeCell(1)); 
}

public Action GetClientPos(int client, const char[] command, int argc)
{
    Client ply = Clients.Get(client);

    Vector plyPos = ply.GetPos();
    PrintToChat(ply.id, "Your pos is: %f, %f, %f", plyPos.x, plyPos.y, plyPos.z);

    Vector vec1 = new Vector(plyPos.x - 200.0, plyPos.y - 200.0, plyPos.z - 200.0);
    Vector vec2 = new Vector(plyPos.x + 200.0, plyPos.y + 200.0, plyPos.z + 200.0);

    ArrayList entArr = Ents.FindInBox(vec1, vec2);

    for(int i=0; i < entArr.Length; i++) {
        Entity ent = GetArrayCell(entArr, i, 0);
        PrintToServer("%i", ent.id);
    }
}

public Action TpTo914(int client, const char[] command, int argc)
{
    float pos[3] = {3223.215576,-2231.152587,0.031250};
    float ang[3] = {0.0,0.0,0.0};
    TeleportEntity(client, pos, ang, NULL_VECTOR);
}

//////////////////////////////////////////////////////////////////////////////
//
//                                Events
//
//////////////////////////////////////////////////////////////////////////////

public void OnMapStart() 
{
    char mapName[128];
    GetCurrentMap(mapName, sizeof(mapName));
    gamemode = new GameMode(mapName);

    FakePrecacheSound(gamemode.config.NukeSound);
}

public void OnClientJoin(Client ply) 
{
    if (gamemode.config.debug)
    {
        PrintToServer("Client joined - localId: (%i), steamId: (%i)", ply.id, GetSteamAccountID(ply.id));
    }

    g_IgnoreDoorAccess[client] = false;

    SDKHook(ply.id, SDKHook_WeaponCanUse, OnWeaponTake);
    SDKHook(ply.id, SDKHook_SpawnPost, OnPlayerSpawnPost);
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);

    Call_StartForward(OnClientJoinForward);
    Call_PushCell(ply);
    Call_Finish();
}

public void OnClientLeave(Client ply) 
{
    if (gamemode.config.debug)
    {
        PrintToServer("Client disconnected: %i", ply.id);
    }

    Call_StartForward(OnClientLeaveForward);
    Call_PushCell(ply);
    Call_Finish();
}

public void OnPlayerSpawn(Client ply)
{
    SetEntData(ply.id, g_offsCollisionGroup, 2, 4, true);
    EquipPlayerWeapon(ply.id, GivePlayerItem(ply.id, "weapon_fists"));

    if (ply.class != null) 
    {
        Call_StartForward(OnClientSpawnForward);
        Call_PushCell(ply);
        Call_Finish();

        ply.Spawn();

        if (gamemode.config.debug) 
        {
            char gClassName[32], className[32];
            ply.gclass(gClassName, sizeof(gClassName));
            ply.class.Name(className, sizeof(className));
            PrintToChat(ply.id, " \x07[SCP] \x01Твой класс %s - %s", gClassName, className);
        }
    }
}

public Action Event_PlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    if(!IsWarmup())
    {
        StringMap players = new StringMap();
        Client ply;
        
        for(int client = 0; client < MAXPLAYERS; client++)
        {
            if(IsClientExist(client) && IsPlayerAlive(client) && !IsClientInSpec(client))
            {
                ply = Clients.Get(client);
                player.SetValue(ply.class.Name(), 1);
            }
        }

        if(players.Length == 0)
        {
            SCP_EndRound("nuke_explosion");
        } 
        else if(players.Length <= 1) 
        {
            char key[64];
            players.GetKey(i, key, sizeof(key);

            SCP_EndRound(key);
        }

        delete players;
    }
}

public void OnRoundStart(Event ev, const char[] name, bool dbroadcast) 
{
    if(!IsWarmup())
    {
        g_AllowRoundEnd = false;
        g_IsNuckExpl = false;
        
        StringMapSnapshot gClassNameS = gamemode.GetGlobalClassNames();
        int gClassCount, classCount, extra = 0;
        int keyLen;

        for (int i = 0; i < gClassNameS.Length; i++) 
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

            for (int v = 0; v < classNameS.Length; v++) 
            {
                classKeyLen = classNameS.KeyBufferSize(v);
                char[] classKey = new char[classKeyLen];
                classNameS.GetKey(v, classKey, classKeyLen);
                if (json_is_meta_key(classKey)) continue;

                Class class = gclass.class(classKey);

                classCount = gClassCount * class.percent / 100;
                classCount = (classCount != 0 || !class.priority) ? classCount : 1;

                for (int scc = 1; scc <= classCount; scc++) 
                {
                    if (extra > Clients.InGame()) break;
                    Client player = Clients.GetRandomWithoutClass();
                    player.gclass(gClassKey);
                    //player.class(classKey);
                    player.class = class;
                    player.haveClass = true;

                    extra++;
                }
            }
        }

        for (int i = 1; i <= Clients.InGame() - extra; i++) 
        {
            Client player = Clients.GetRandomWithoutClass();
            char gclass[32], class[32];
            gamemode.config.DefaultGlobalClass(gclass, sizeof(gclass));
            gamemode.config.DefaultClass(class, sizeof(class));
            player.gclass(gclass);
            //player.class(class);
            player.class = gamemode.gclass(gclass).class(class);
            player.haveClass = true;
        }
    }
}

public void OnRoundEnd(Event ev, const char[] name, bool dbroadcast) 
{
    for (int cig=1; cig <= Clients.InGame(); cig++) 
    {
        Client client = Clients.Get(cig);
        client.class = null;
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
    if(IsClientExist(activator) && IsValidEntity(caller) && IsPlayerAlive(activator) && !IsClientInSpec(activator))
    {
        Client ply = Clients.Get(activator);
        int doorId = GetEntProp(caller, Prop_Data, "m_iHammerID");

        if (gamemode.config.debug)
            PrintToChatAll(" \x07[SCP] \x01Door/Button id: (%i)", doorId);

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

            if (doorId == StringToInt(doorKey))
            {
                if (IsWarmup())
                {
                    return Plugin_Continue;
                }
                else if(g_IgnoreDoorAccess[activator] == true)
                {
                    return Plugin_Continue;
                }
                else if (ply.IsSCP && door.scp)
                {
                    return Plugin_Continue;
                }
                else if (ply.access >= door.access)
                {
                    return Plugin_Continue;
                }
                else
                {
                    return Plugin_Stop;
                }
            }
        }

        Call_StartForward(OnButtonPressedForward);
        Call_PushCell(ply);
        Call_PushCell(doorId);
        Call_Finish();
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
        
        Call_StartForward(OnTakeDamageForward);
        Call_PushCell(vic);
        Call_PushCell(atk);
        Call_PushFloat(damage);
        Call_Finish();
    }

    return Plugin_Continue;
}

public void Event_OnTriggerActivation(const char[] output, int caller, int activator, float delay)
{
    if(IsClientExist(activator) && IsValidEntity(caller) && IsPlayerAlive(activator) && !IsClientInSpec(activator))
    {
        int iTrigger = GetEntProp(caller, Prop_Data, "m_iHammerID");

        if(gamemode.config.debug)
        {
            PrintToChatAll(" \x07[SCP] \x01T_ID: %i", iTrigger);
        }
    }
}

public Action OnLookAtWeaponPressed(int client, const char[] command, int argc)
{
    if(IsClientExist(client) && !IsClientInSpec(client))
    {
        Client ply = Clients.Get(client);
        
        if(!ply.IsSCP)
        {
            DisplayCardMenu(client);
        }
    }
}

public Action OnWeaponTake(int client, int iWeapon)
{
    Client ply = Clients.Get(client);

    if(ply.IsSCP)
    {
        return Plugin_Stop;
    }

    char classname[64];
    GetEntityClassname(iWeapon, classname, sizeof(classname));

    if (StrEqual(classname, "weapon_melee") || StrEqual(classname, "weapon_knife"))
    {
        EquipPlayerWeapon(client, iWeapon);
    }

    return Plugin_Continue;
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

void SCP_EndRound(const char[] team)
{
    g_AllowRoundEnd = true;
    
    if(StrEqual("nuke_explosion", team))
    {
        CS_TerminateRound(GetConVarFloat(FindConVar("mp_round_restart_delay")), CSRoundEnd_TargetBombed, false);
        PrintToChatAll(" \x07[SCP] \x01Комплекс уничтожен! Выживших не обнаружено...");
    }
    else
    {
        CS_TerminateRound(GetConVarFloat(FindConVar("mp_round_restart_delay")), CSRoundEnd_TerroristWin, false);
        PrintToChatAll(" \x07[SCP] \x01%s победили!", team);
    }
}

void SCP_NukeActivation()
{
    for(int client = 0; client < MAXPLAYERS; client++)
    {
        if(IsClientExist(client))
        {
            EmitSoundToClient(client, gamemode.config.NukeSound, SOUND_FROM_WORLD, SNDLEVEL_NORMAL);
        }
    }

    CreateTimer(gamemode.config.NukeTime, NukeExplosion);

    int ent;
    
    while((ent = FindEntityByClassname(ent, "func_door")) != -1)
    {
        AcceptEntityInput(ent, "Open");
    }
}

void FakePrecacheSound(const char[] szPath)
{
    AddToStringTable(FindStringTable( "soundprecache" ), szPath);
}

//////////////////////////////////////////////////////////////////////////////
//
//                                Commands
//
//////////////////////////////////////////////////////////////////////////////

public Action Command_AdminMenu(int client, int args)
{
    if(IsClientExist(client))
    {
        DisplayAdminMenu(client);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//                                 Timers
//
//////////////////////////////////////////////////////////////////////////////

public Action NukeExplosion(Handle hTimer)
{
    g_IsNuckExpl = true;
    PrintToChatAll("Boom!");
}

//////////////////////////////////////////////////////////////////////////////
//
//                                 Menu
//
//////////////////////////////////////////////////////////////////////////////

void DisplayCardMenu(int client)
{
    PrintToChat(client, " \x07[SCP] \x01Скоро тут будет меню (честно-честно!)");
}

void DisplayAdminMenu(int client)
{
    if(IsClientExist(client))
    {
        Menu hMenu = new Menu(MenuHandler_ScpAdminMenu);
        hMenu.SetTitle("Меню администратора");

        hMenu.AddItem("item1", "Просмотреть классы игроков", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item2", "Возродить игрока", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item3", "Телепортировать игрока", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item4", "Переместить в наблюдатели", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item5", "Провести беседу с игроком", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item6", "Игнорирование карт доступа", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item7", "Выдать предмет", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item8", "Перезапустить раунд", ITEMDRAW_DEFAULT);
        hMenu.AddItem("item9", "Взорвать комплекс", ITEMDRAW_DEFAULT);

        hMenu.Display(client, 30);
    }
}

public int MenuHandler_ScpAdminMenu(Menu hMenu, MenuAction action, int client, int item)
{
    if (action == MenuAction_Select)
    {
        switch(item)
        {
            case 5:
            {
                if(g_IgnoreDoorAccess[client] == false)
                {
                    PrintToChat(client, " \x07[SCP] \x01Игнорирование карт доступа \x06включено");
                    g_IgnoreDoorAccess[client] = true;
                }
                else
                {
                    PrintToChat(client, " \x07[SCP] \x01Игнорирование карт доступа \x06отключено");
                    g_IgnoreDoorAccess[client] = false;
                }
            }
            case 9:
            {
                SCP_NukeActivation();
            }
            default:
            {
                PrintToChat(client, "Да.");
            }
        }
    }
}