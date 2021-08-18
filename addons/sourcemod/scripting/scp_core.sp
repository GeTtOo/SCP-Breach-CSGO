#pragma semicolon 1
#pragma newdecls required

#include <sourcemod>
#include <cstrike>
#include <sdkhooks>
// ¯\_(ツ)_/¯
#include <scpcore>
#include "include/scp/scp_admin.sp"

#define NUKE_EXPLOSION_SOUND "weapons/c4/c4_exp_deb1.wav"

Handle OnClientJoinForward;
Handle OnClientLeaveForward;
Handle OnClientSpawnForward;
Handle OnClientResetForward;
Handle OnTakeDamageForward;
Handle OnPlayerDeathForward;
Handle OnButtonPressedForward;
Handle OnRoundStartForward;
Handle OnRoundEndForward;
Handle OnInputForward;
Handle OnPressFForward;

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

public APLRes AskPluginLoad2(Handle self, bool late, char[] error, int err_max)
{
    CreateNative("GameMode.team", NativeGameMode_GetTeam);
    CreateNative("GameMode.config.get", NativeGameMode_Config);
    CreateNative("GameMode.entities.get", NativeGameMode_Entities);
    CreateNative("GameMode.mngr.get", NativeGameMode_Manager);
    CreateNative("GameMode.timer.get", NativeGameMode_Timers);
    
    CreateNative("ClientSingleton.Get", NativeClients_Get);
    CreateNative("ClientSingleton.GetRandom", NativeClients_GetRandom);
    CreateNative("ClientSingleton.InGame", NativeClients_InGame);
    CreateNative("ClientSingleton.Alive", NativeClients_Alive);

    CreateNative("EntitySingleton.Create", NativeEntities_Create);
    CreateNative("EntitySingleton.Remove", NativeEntities_Remove);
    CreateNative("EntitySingleton.Get", NativeEntities_Get);
    CreateNative("EntitySingleton.TryGetOrAdd", NativeEntities_TryGetOrAdd);
    
    OnClientJoinForward = CreateGlobalForward("SCP_OnPlayerJoin", ET_Event, Param_CellByRef);
    OnClientLeaveForward = CreateGlobalForward("SCP_OnPlayerLeave", ET_Event, Param_CellByRef);
    OnClientSpawnForward = CreateGlobalForward("SCP_OnPlayerSpawn", ET_Event, Param_CellByRef);
    OnClientResetForward = CreateGlobalForward("SCP_OnPlayerReset", ET_Event, Param_CellByRef);
    OnTakeDamageForward = CreateGlobalForward("SCP_OnTakeDamage", ET_Event, Param_Cell, Param_Cell, Param_FloatByRef, Param_CellByRef);
    OnPlayerDeathForward = CreateGlobalForward("SCP_OnPlayerDeath", ET_Event, Param_CellByRef, Param_CellByRef);
    OnButtonPressedForward = CreateGlobalForward("SCP_OnButtonPressed", ET_Event, Param_CellByRef, Param_Cell);
    OnRoundStartForward = CreateGlobalForward("SCP_OnRoundStart", ET_Event);
    OnRoundEndForward = CreateGlobalForward("SCP_OnRoundEnd", ET_Event);
    OnInputForward = CreateGlobalForward("SCP_OnInput", ET_Event, Param_CellByRef, Param_Cell);
    OnPressFForward = CreateGlobalForward("SCP_OnPressF", ET_Event, Param_CellByRef);

    RegPluginLibrary("scp_core");
    return APLRes_Success;
}

public void OnPluginStart()
{
    Clients = new ClientSingleton();
    Ents = new EntitySingleton();
    AdminMenu = new AdminMenuSingleton();

    LoadTranslations("scpcore.phrases");
    
    RegServerCmd("ents", CmdEnts);                                                          // ¯\_(ツ)_/¯
    RegServerCmd("scp", CmdSCP);                                                            // ¯\_(ツ)_/¯

    RegAdminCmd("scp_admin", Command_AdminMenu, ADMFLAG_CUSTOM1);
    RegAdminCmd("scp_spawn", PlayerSpawn, ADMFLAG_CUSTOM1);

    AddCommandListener(OnLookAtWeaponPressed, "+lookatweapon");
    AddCommandListener(GetClientPos, "getmypos");                                           // ¯\_(ツ)_/¯
    AddCommandListener(TpTo914, "tp914");                                                   // ¯\_(ツ)_/¯
    
    HookEvent("player_spawn", Event_PlayerSpawn, EventHookMode_Post);
    HookEvent("round_start", OnRoundStart);
    HookEvent("round_prestart", OnRoundPreStart);
    HookEvent("player_death", Event_PlayerDeath, EventHookMode_Pre);
    
    HookEntityOutput("func_button", "OnPressed", Event_OnButtonPressed);
    HookEntityOutput("trigger_teleport", "OnStartTouch", Event_OnTriggerActivation);
}

//////////////////////////////////////////////////////////////////////////////
//
//                                Events
//
//////////////////////////////////////////////////////////////////////////////
bool fixCache = false;

public void OnMapStart() 
{
    char mapName[128];

    GetCurrentMap(mapName, sizeof(mapName));
    gamemode = new GameMode(mapName);

    gamemode.mngr.CollisionGroup = FindSendPropInfo("CBaseEntity", "m_CollisionGroup");

    PrecacheSound(NUKE_EXPLOSION_SOUND);
    LoadFileToDownload();
    
    // ¯\_(ツ)_/¯
    if (!fixCache) {
        ForceChangeLevel(mapName, "Fix sound cached");
        fixCache = true;
    }
}

public void OnGameFrame()
{
    gamemode.timer.Update();
}

public void OnClientConnected(int id)
{
    Clients.Add(id);
}

public void OnClientPostAdminCheck(int id)
{
    Client ply = Clients.Get(id);

    if(GetAdminFlag(GetUserAdmin(id), Admin_Custom1))
    {
        AdminMenu.Add(ply);
    }

    if (gamemode.config.debug)
    {
        PrintToServer("Client joined - localId: (%i), steamId: (%i)", ply.id, GetSteamAccountID(ply.id));
    }

    SDKHook(ply.id, SDKHook_WeaponCanUse, OnWeaponTake);
    SDKHook(ply.id, SDKHook_SpawnPost, OnPlayerSpawnPost);
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);

    Call_StartForward(OnClientJoinForward);
    Call_PushCellRef(ply);
    Call_Finish();
}

public void OnClientDisconnect_Post(int id)
{
    Client ply = Clients.Get(id);

    if (gamemode.config.debug)
        PrintToServer("Client disconnected: %i", ply.id);

    EndRoundCount(ply);

    Call_StartForward(OnClientLeaveForward);
    Call_PushCellRef(ply);
    Call_Finish();

    Clients.Remove(id);
}

public Action Event_PlayerSpawn(Event event, const char[] name, bool dontBroadcast)
{
    Client ply = Clients.Get(GetClientOfUserId(GetEventInt(event, "userid")));

    if (IsClientExist(ply.id) && GetClientTeam(ply.id) > 1 && !ply.active) {
        ply.active = true;
        gamemode.timer.Simple(250, "Timer_PlayerSpawn", ply);
    }

    return Plugin_Continue;
}

public void Timer_PlayerSpawn(Client ply)
{
    if(IsClientExist(ply.id))
    {
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
        
        gamemode.mngr.SetCollisionGroup(ply.id, 2);

        if (ply != null && ply.class != null)
        {
            Call_StartForward(OnClientSpawnForward);
            Call_PushCellRef(ply);
            Call_Finish();

            if (ply.class.fists)         // (╯°□°）╯︵ ┻━┻  ©️ Гет
                EquipPlayerWeapon(ply.id, GivePlayerItem(ply.id, "weapon_fists"));

            ply.Setup();

            if (!IsFakeClient(ply.id))
                SendConVarValue(ply.id, FindConVar("game_type"), "6");

            if (gamemode.config.debug)
            {
                char teamName[32], className[32];
                ply.Team(teamName, sizeof(teamName));
                ply.class.Name(className, sizeof(className));
                PrintToChat(ply.id, " \x07[SCP] \x01%t", "Show class when player spawn", teamName, className);
            }
        }
    }
}

public void OnRoundStart(Event event, const char[] name, bool dbroadcast)
{
    if(!IsWarmup())
    {
        gamemode.mngr.RoundComplete = false;
        gamemode.mngr.IsNuked = false;
        gamemode.mngr.Reset();
        
        ArrayList sortedPlayers = new ArrayList();
        
        Client bufarr[MAXPLAYERS+1];
        Clients.GetArray("Clients", bufarr, sizeof(bufarr));

        for (int i=1; i <= Clients.InGame(); i++)
            sortedPlayers.Push(bufarr[i]);

        sortedPlayers.Sort(Sort_Random, Sort_Integer);
        
        StringMapSnapshot teamNameS = gamemode.GetTeamNames();
        int teamCount, classCount, extra = 0;
        int keyLen;

        for (int i = 0; i < teamNameS.Length; i++)
        {
            keyLen = teamNameS.KeyBufferSize(i);
            char[] teamKey = new char[keyLen];
            teamNameS.GetKey(i, teamKey, keyLen);
            if (json_is_meta_key(teamKey)) continue;

            GTeam team = gamemode.team(teamKey);

            teamCount = Clients.InGame() * team.percent / 100;
            teamCount = (teamCount != 0 || !team.priority) ? teamCount : 1;
            
            StringMapSnapshot classNameS = team.GetClassNames();
            int classKeyLen;

            for (int v = 0; v < classNameS.Length; v++)
            {
                classKeyLen = classNameS.KeyBufferSize(v);
                char[] classKey = new char[classKeyLen];
                classNameS.GetKey(v, classKey, classKeyLen);
                if (json_is_meta_key(classKey)) continue;

                Class class = team.class(classKey);

                classCount = teamCount * class.percent / 100;
                classCount = (classCount != 0 || !class.priority) ? classCount : 1;

                for (int scc = 1; scc <= classCount; scc++)
                {
                    if (extra > Clients.InGame()) break;
                    int id = sortedPlayers.Length - 1;
                    if (id < 0) break;
                    Client player = view_as<Client>(sortedPlayers.Get(id));
                    sortedPlayers.Erase(id);
                    player.Team(teamKey);
                    player.class = class;
                    player.haveclass = true;
                    gamemode.mngr.team(teamKey).count++;

                    extra++;
                }
            }
        }

        for (int i = 1; i <= Clients.InGame() - extra; i++)
        {
            int id = sortedPlayers.Length - 1;
            if (id < 0) break;
            Client player = view_as<Client>(sortedPlayers.Get(id));
            sortedPlayers.Erase(id);
            char team[32], class[32];
            gamemode.config.DefaultGlobalClass(team, sizeof(team));
            gamemode.config.DefaultClass(class, sizeof(class));
            player.Team(team);
            player.class = gamemode.team(team).class(class);
            player.haveclass = true;
            gamemode.mngr.team(team).count++;
        }
        
        delete sortedPlayers;

        SetMapRegions();
        SpawnItemsOnMap();

        Call_StartForward(OnRoundStartForward);
        Call_Finish();
    }
}

public void OnRoundPreStart(Event event, const char[] name, bool dbroadcast)
{
    for (int cig=1; cig <= Clients.InGame(); cig++)
    {
        Client client = Clients.Get(cig);

        Call_StartForward(OnClientResetForward);
        Call_PushCellRef(client);
        Call_Finish();

        client.class = null;
        client.haveclass = false;
        client.inv.Clear();
        client.active = false;
    }

    Ents.Clear();

    Call_StartForward(OnRoundEndForward);
    Call_Finish();
}

public Action CS_OnTerminateRound(float& delay, CSRoundEndReason& reason)
{
    if(IsWarmup())
	{
		return Plugin_Continue;
	}
    else if(gamemode.mngr.RoundComplete)
    {
        return Plugin_Continue;
    }
    else
    {
        return Plugin_Handled;
    }
}

public void OnPlayerRunCmdPost(int client, int buttons)
{
    Client ply = Clients.Get(client);
    
    if (ply != null && ply.class != null)
    {
        Call_StartForward(OnInputForward);
        Call_PushCellRef(ply);
        Call_PushCell(buttons);
        Call_Finish();
    }
}

public Action Event_OnButtonPressed(const char[] output, int caller, int activator, float delay)
{
    if(IsClientExist(activator) && IsValidEntity(caller) && IsPlayerAlive(activator) && !IsClientInSpec(activator))
    {
        Client ply = Clients.Get(activator);
        int doorId = GetEntProp(caller, Prop_Data, "m_iHammerID");

        if (gamemode.config.debug)
            PrintToChatAll(" \x07[SCP Admin] \x01Door/Button id: (%i)", doorId);

        if(gamemode.config.NukeButtonID == doorId)
            SCP_NukeActivation();

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
                else if(ply.fullaccess)
                {
                    return Plugin_Continue;
                }
                else if (ply.IsSCP && door.scp)
                {
                    return Plugin_Continue;
                }
                else if (ply.access >= door.access || ply.inv.Check("access") >= door.access)
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
        Call_PushCellRef(ply);
        Call_PushCell(doorId);
        Call_Finish();
    }
    
    return Plugin_Continue;
}

public Action OnPlayerSpawnPost(int client)
{
    SetEntProp(client, Prop_Send, "m_iHideHUD", 1<<12);
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{   
    if(IsClientExist(victim))
    {
        Client atk, vic = Clients.Get(victim);
        Action result;

        if(IsClientExist(attacker))
        {
            atk = Clients.Get(attacker);
            if(vic.IsSCP && atk.IsSCP)
            {
                return Plugin_Stop;
            }
            else if(!gamemode.config.ff)
            {
                char vicTeam[32], atkTeam[32];
                
                vic.Team(vicTeam, sizeof(vicTeam));
                atk.Team(atkTeam, sizeof(atkTeam));
                
                if(StrEqual(vicTeam, atkTeam))
                {
                    return Plugin_Stop;
                }
            }
        }
        else
        {
            atk = null;
        }
        
        Call_StartForward(OnTakeDamageForward);
        Call_PushCell(vic);
        Call_PushCell(atk);
        Call_PushFloatRef(damage);
        Call_PushCellRef(damagetype);
        Call_Finish(result);

        return result;
    }

    return Plugin_Continue;
}

public Action Event_PlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    if(!IsWarmup())
    {
        Client vic = Clients.Get(GetClientOfUserId(GetEventInt(event, "userid")));
        Client atk = Clients.Get(GetClientOfUserId(GetEventInt(event, "attacker")));

        ArrayList inv = vic.inv.items;

        while (inv.Length != 0)
        {
            char entclass[32];
            
            Item itm = vic.inv.Drop();
            itm.GetEntClass(entclass, sizeof(entclass));

            delete itm;

            Ents.Create(entclass)
            .SetPos(vic.GetPos() + new Vector(GetRandomFloat(-30.0,30.0), GetRandomFloat(-30.0,30.0), 0.0))
            .UseCB(view_as<SDKHookCB>(Callback_EntUse))
            .Spawn();
        }
        
        vic.inv.Clear();

        vic.active = false;

        EndRoundCount(vic);

        Call_StartForward(OnPlayerDeathForward);
        Call_PushCellRef(vic);
        Call_PushCellRef(atk);
        Call_Finish();
    }

    return Plugin_Handled;
}

public void Event_OnTriggerActivation(const char[] output, int caller, int activator, float delay)
{
    if(IsClientExist(activator) && IsValidEntity(caller) && IsPlayerAlive(activator) && !IsClientInSpec(activator))
    {
        int iTrigger = GetEntProp(caller, Prop_Data, "m_iHammerID");

        if(gamemode.config.debug)
        {
            PrintToChatAll(" \x07[SCP Admin] \x01T_ID: %i", iTrigger);
        }
    }
}

public Action OnLookAtWeaponPressed(int client, const char[] command, int argc)
{
    if(IsClientExist(client) && !IsClientInSpec(client))
        DisplayFMenu(Clients.Get(client));
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

public void LoadFileToDownload()
{
    Handle hFile = OpenFile("addons/sourcemod/configs/scp/downloads.txt", "r");
    
    if(hFile)
    {
        int size = 0;
        char buffer[PLATFORM_MAX_PATH];
        
        while(!IsEndOfFile(hFile) && ReadFileLine(hFile, buffer, sizeof(buffer)))
        {
            if(TrimString(buffer) > 2 && IsCharAlpha(buffer[0]))
            {
                AddFileToDownloadsTable(buffer);
                size = strlen(buffer);

                if(StrContains(buffer, ".mdl", false) == (size - 4))
                {
                    PrecacheModel(buffer);
                }
                else if(StrContains(buffer, ".wav", false) == (size - 4) || StrContains(buffer, ".mp3", false) == (size - 4))
                {
                    strcopy(buffer, sizeof(buffer), buffer[6]);
                    Format(buffer, sizeof(buffer), "%s%s", "*/", buffer);
                    FakePrecacheSound(buffer);
                }
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
//                                Callbacks
//
//////////////////////////////////////////////////////////////////////////////

public SDKHookCB Callback_EntUse(int entity, int client) 
{
    Client ply = Clients.Get(client);
    Entity ent = Ents.Get(entity);

    if (ply.IsSCP) return;

    char entClassName[32];
    ent.GetClass(entClassName, sizeof(entClassName));

    if (gamemode.entities.HasKey(entClassName))
    {
        if (ply.inv.Add(entClassName))
            Ents.Remove(ent.id);
        else
            PrintToChat(ply.id, " \x07[SCP] \x01%t", "Inventory full");
    }
}

public int InventoryHandler(Menu hMenu, MenuAction action, int client, int item) 
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if (action == MenuAction_Select) 
    {
        Client ply = Clients.Get(client);
        Item itm = ply.inv.Drop(item);

        char entclass[32];
        itm.GetEntClass(entclass, sizeof(entclass));
        delete itm;

        Ents.Create(entclass)
        .SetPos(ply.GetPos())
        .UseCB(view_as<SDKHookCB>(Callback_EntUse))
        .Spawn();
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//                                Functions
//
//////////////////////////////////////////////////////////////////////////////

public void SetMapRegions() 
{
    JSON_ARRAY regions = gamemode.config.regions;

    for (int i=0; i < regions.Length; i++) 
    {
        JSON_OBJECT region = view_as<JSON_OBJECT>(regions.GetObject(i));
        Vector pos = region.GetVector("pos");
        char radius[5], name[128];
        IntToString(region.GetInt("radius"),radius,sizeof(radius));
        region.GetString("name",name,sizeof(name));

        Entity ent = Ents.Create("info_map_region", false).SetPos(pos);
        DispatchKeyValue(ent.id,"radius",radius);
        DispatchKeyValue(ent.id,"token",name);
        ent.Spawn();
        delete ent;
    }
}

public void SpawnItemsOnMap() 
{
    JSON_OBJECT spawnmap = gamemode.config.spawnmap;
    StringMapSnapshot snapshot = spawnmap.Snapshot();

    for (int i=0; i < snapshot.Length; i++) 
    {
        int itemlen = snapshot.KeyBufferSize(i);
        char[] item = new char[itemlen];
        snapshot.GetKey(i, item, itemlen);

        if (json_is_meta_key(item)) continue;
        
        JSON_ARRAY rawDataArr = view_as<JSON_ARRAY>(spawnmap.GetObject(item));

        for (int v=0; v < rawDataArr.Length; v++) 
        {
            JSON_OBJECT data = view_as<JSON_OBJECT>(rawDataArr.GetObject(v));
            Vector pos = data.GetVector("pos");

            if (GetRandomInt(1, 100) <= data.GetInt("chance"))
                Ents.Create(item)
                .SetPos(pos)
                .UseCB(view_as<SDKHookCB>(Callback_EntUse))
                .Spawn();
        }
    }
}

public void SCP_EndRound(const char[] team)
{
    gamemode.mngr.RoundComplete = true;
    
    if(StrEqual("nuke_explosion", team))
    {
        CS_TerminateRound(GetConVarFloat(FindConVar("mp_round_restart_delay")), CSRoundEnd_TargetBombed, false);
        PrintToChatAll(" \x07[SCP] \x01%t", "Site destroy");
    }
    else
    {
        CS_TerminateRound(GetConVarFloat(FindConVar("mp_round_restart_delay")), CSRoundEnd_TerroristWin, false);
        PrintToChatAll(" \x07[SCP] \x01%t", "Team Win", team);
    }
}

public void SCP_NukeActivation()
{
    char sound[128];
    gamemode.config.NukeSound(sound, sizeof(sound));

    EmitSoundToAll(sound);

    CreateTimer(gamemode.config.NukeTime - 10.0, NukeExplosionDoorClose);
    CreateTimer(gamemode.config.NukeTime, NukeExplosion);

    int ent;
    
    while((ent = FindEntityByClassname(ent, "func_door")) != -1)
    {
        AcceptEntityInput(ent, "Open");
    }
}

public void EndRoundCount(Client ply)
{
    if(Clients.Alive() == 0 && Clients.InGame() != 0)
    {
        SCP_EndRound("nuke_explosion");
    }
    else
    {
        if (ply != null && ply.class != null)
        {
            char team[32];
            ply.Team(team, sizeof(team));
            
            gamemode.mngr.team(team).count--;
            gamemode.mngr.DeadPlayers++;
            
            char winTeam[32];
            if (gamemode.mngr.CheckTeamStatus(winTeam, sizeof(winTeam)))
                SCP_EndRound(winTeam);
        }
    }
}

stock void FakePrecacheSound(const char[] szPath)
{
    AddToStringTable(FindStringTable( "soundprecache" ), szPath);
}

stock void Shake(int client)
{
    Handle message = StartMessageOne("Shake", client, USERMSG_RELIABLE);

    PbSetInt(message, "command", 0);
    PbSetFloat(message, "local_amplitude", 50.0);
    PbSetFloat(message, "frequency", 10.0);
    PbSetFloat(message, "duration", 5.0);

    EndMessage();
}

stock bool IsClientExist(int client)
{
    if((0 < client < MaxClients) && IsClientInGame(client) && !IsClientSourceTV(client))
    {
        return true;
    }

    return false;
}

stock bool IsClientInSpec(int client)
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

stock bool IsCleintInSpec(int client)
{
    if(GetClientTeam(client) != 1)
    {
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////
//
//                                 Timers
//
//////////////////////////////////////////////////////////////////////////////

public Action NukeExplosion(Handle hTimer)
{
    float pos[3];
    gamemode.mngr.IsNuked = true;

    for(int client = 0; client < MAXPLAYERS; client++)
    {
        if(IsClientExist(client))
        {
            GetClientAbsOrigin(client, pos);

            if(pos[2] <= gamemode.config.NukeKillPos)
            {
                ForcePlayerSuicide(client);
            }
            else
            {
                EmitSoundToClient(client, NUKE_EXPLOSION_SOUND);
                Shake(client);
            }
        }
    }

    return Plugin_Stop;
}

public Action NukeExplosionDoorClose(Handle hTimer)
{
    int ent;
    while((ent = FindEntityByClassname(ent, "func_door")) != -1)
    {
        char name[16];
        GetEntPropString(ent, Prop_Data, "m_iName", name, sizeof(name));
        
        if(StrContains(name, "DoorGate", false) != -1)
        {
            AcceptEntityInput(ent, "Close");
            AcceptEntityInput(ent, "Lock");
        }
    }

    //!- Список кнопок которые будут блокироваться после взрыва
    return Plugin_Stop;
}

//////////////////////////////////////////////////////////////////////////////
//
//                                Commands
//
//////////////////////////////////////////////////////////////////////////////

//-----------------------------Server-----------------------------//

public Action CmdEnts(int args) 
{
    char command[32];
    GetCmdArgString(command, sizeof(command));

    if (StrEqual(command, "getall", false)) 
    {
        ArrayList ents = Ents.GetAll();

        for (int i=0; i < ents.Length; i++) 
        {
            Entity ent = ents.Get(i);
            char name[32];

            ent.GetClass(name, sizeof(name));
            
            PrintToServer("%s id: %i", name, ent.id);
        }

        PrintToServer("Count: %i", ents.Length);
    }
}

public Action CmdSCP(int args)
{
    char command[32];
    GetCmdArgString(command, sizeof(command));

    if (StrEqual(command, "status", false))
    {
        StringMapSnapshot snapshot = gamemode.mngr.teams.Snapshot();

        PrintToServer("Class: Dead, count: %i", gamemode.mngr.DeadPlayers);
        
        for (int i=0; i < snapshot.Length; i++)
        {
            int teamlen = snapshot.KeyBufferSize(i);
            char[] teamname = new char[teamlen];
            snapshot.GetKey(i, teamname, teamlen);
            if (json_is_meta_key(teamname)) continue;

            PrintToServer("Class: %s, count: %i", teamname, gamemode.mngr.team(teamname).count);
        }
    }
}

//-----------------------------Client-----------------------------//

public Action Command_AdminMenu(int client, int args)
{
    if(IsClientExist(client))
    {
        DisplayAdminMenu(client);
    }
}

public Action GetClientPos(int client, const char[] command, int argc)
{
    Client ply = Clients.Get(client);

    Vector plyPos = ply.GetPos();
    PrintToChat(ply.id, "Your pos is: %f, %f, %f", plyPos.x, plyPos.y, plyPos.z);

    delete plyPos;

    PrintEntInCone(client, command, argc);
    PrintEntInBox(client, command, argc);
}

public Action PrintEntInBox(int client, const char[] command, int argc)
{
    Client ply = Clients.Get(client);

    char filter[4][32] = { "prop_physics", "weapon_", "func_door", "prop_dynamic" };

    ArrayList entArr = Ents.FindInBox(ply.GetPos() - new Vector(200.0, 200.0, 200.0), ply.GetPos() + new Vector(200.0, 200.0, 200.0), filter, sizeof(filter));

    for(int i=0; i < entArr.Length; i++) 
    {
        Entity ent = entArr.Get(i, 0);

        char entclass[32];
        ent.GetClass(entclass, sizeof(entclass));
        
        PrintToChat(ply.id, "class: %s, id: %i", entclass, ent.id);
    }

    delete entArr;
}

public Action PrintEntInCone(int client, const char[] command, int argc)
{
    Client ply = Clients.Get(client);

    char filter[1][32] = {"player"};

    ArrayList entArr = Ents.FindInCone(ply.EyePos(), ply.EyeAngles().Forward(ply.EyePos(), 1000.0), 90, filter, sizeof(filter));

    for(int i=0; i < entArr.Length; i++) 
    {
        Client ent = entArr.Get(i, 0);
        
        PrintToChat(ply.id, "Player: %i", ent.id);
    }

    delete entArr;
}

public Action TpTo914(int client, const char[] command, int argc)
{
    Client ply = Clients.Get(client);
    ply.SetPos(new Vector(3100.0, -2231.0, 0.0), new Angle(0.0, 0.0, 0.0));
}

public Action PlayerSpawn(int client,int args)
{
    if (IsPlayerAlive(client))
    {
        PrintToConsole(client, "Сменить класс возможно только мёртвым игрокам");
        return Plugin_Stop;
    }

    char teamName[32], className[32];
    GetCmdArg(1, teamName, sizeof(teamName));
    GetCmdArg(2, className, sizeof(className));
    
    Client ply = Clients.Get(client);

    GTeam gteam = gamemode.team(teamName);

    if (gteam != null && gteam.classes.HasKey(className))
    {
        char curTeam[32];

        ply.Team(curTeam, sizeof(curTeam));
        ply.Team(teamName);
        ply.class = gteam.class(className);
        
        //ply.Setup();
        ply.Spawn();

        //gamemode.mngr.team(curTeam).count--;
        gamemode.mngr.team(teamName).count++;
        gamemode.mngr.DeadPlayers--;
    }
    else
    {
        PrintToConsole(ply.id, "Ошибка в идентификаторе команды/класса");
    }
    
    return Plugin_Stop;
}

//////////////////////////////////////////////////////////////////////////////
//
//                                 Menu
//
//////////////////////////////////////////////////////////////////////////////

public void InventoryDisplay(Client ply)
{
    Menu InvMenu = new Menu(InventoryHandler);

    InvMenu.SetTitle("Инвентарь");
    
    ArrayList inv;
    ply.inv.GetValue("inventory", inv);

    if (inv.Length)
    {
        for (int i=0; i < inv.Length; i++)
        {
            char itemid[8], itemName[32];

            IntToString(i, itemid, sizeof(itemid));
            view_as<Item>(inv.Get(i, 0)).name(itemName, sizeof(itemName));
            
            InvMenu.AddItem(itemid, itemName, ITEMDRAW_DEFAULT);
        }
    }
    else
    {
        PrintCenterText(ply.id, "%t", "Inventory empty");
    }

    InvMenu.Display(ply.id, 30);
}

public void DisplayFMenu(Client ply)
{
    // PrintToChat(client, " \x07[SCP] \x01Скоро тут будет меню (честно-честно!)"); ©️ Гет
    // 3 месяца прошло, ничего себе скоро :pepeGigles: ©️ Гет (да говорю сам с собой, что вы мне сделаете?!)
    // Она тут была оказывается ┬─┬ ノ( ゜-゜ノ) ©️ Гет

    if (!ply.IsSCP)
        InventoryDisplay(ply);
    else
    {
        Call_StartForward(OnPressFForward);
        Call_PushCellRef(ply);
        Call_Finish();
    }
}
