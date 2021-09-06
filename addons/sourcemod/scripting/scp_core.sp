#pragma semicolon 1
#pragma newdecls required

#include <sourcemod>
#include <cstrike>
#include <sdkhooks>
// ¯\_(ツ)_/¯
#include <scpcore>
#include "include/scp/scp_admin.sp"

Handle OnClientJoinForward;
Handle OnClientLeaveForward;
Handle OnClientSpawnForward;
Handle OnClientResetForward;
Handle OnClientClearForward;
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
    CreateNative("GameMode.nuke.get", NativeGameMode_Nuke);
    CreateNative("GameMode.timer.get", NativeGameMode_Timers);
    CreateNative("GameMode.log.get", NativeGameMode_Logger);
    
    CreateNative("ClientSingleton.GetAll", NativeClients_GetAll);
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
    OnClientClearForward = CreateGlobalForward("SCP_OnPlayerClear", ET_Event, Param_CellByRef);
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
    WT = new WorldTextSingleton();
    AdminMenu = new AdminMenuSingleton();

    LoadTranslations("scpcore.phrases");
    LoadTranslations("scpcore.regions.phrases");
    
    RegServerCmd("ents", CmdEnts);                                                          // ¯\_(ツ)_/¯
    RegServerCmd("scp", CmdSCP);                                                            // ¯\_(ツ)_/¯

    RegAdminCmd("scp_admin", Command_AdminMenu, ADMFLAG_CUSTOM1);
    RegAdminCmd("scp_spawn", PlayerSpawn, ADMFLAG_CUSTOM1);

    AddCommandListener(OnLookAtWeaponPressed, "+lookatweapon");
    AddCommandListener(GetClientPos, "getmypos");                                           // ¯\_(ツ)_/¯
    AddCommandListener(TpTo, "tp");                                                   // ¯\_(ツ)_/¯
    
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
    
    gamemode.SetValue("Manager", new Manager());
    gamemode.SetValue("Nuke", new NuclearWarhead());
    gamemode.SetValue("Logger", new Logger());
    
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
        gamemode.log.Info("Client joined | localId: (%i), steamId: (%i)", ply.id, GetSteamAccountID(ply.id));
    }

    SDKHook(ply.id, SDKHook_WeaponCanUse, OnWeaponTake);
    SDKHook(ply.id, SDKHook_SpawnPost, OnPlayerSpawnPost);
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);

    Call_StartForward(OnClientJoinForward);
    Call_PushCellRef(ply);
    Call_Finish();

    if (!ply.active && !IsFakeClient(ply.id))
        ply.SetPropFloat("m_fForceTeam", 0.0);
}

public void OnClientDisconnect_Post(int id)
{
    Client ply = Clients.Get(id);

    if (gamemode.config.debug)
        gamemode.log.Info("Client disconnected: %i", ply.id);

    EndRoundCount(ply);

    Call_StartForward(OnClientClearForward);
    Call_PushCellRef(ply);
    Call_Finish();

    Call_StartForward(OnClientLeaveForward);
    Call_PushCellRef(ply);
    Call_Finish();

    Clients.Remove(id);
}

public Action Event_PlayerSpawn(Event event, const char[] name, bool dontBroadcast)
{
    Client ply = Clients.Get(GetClientOfUserId(GetEventInt(event, "userid")));

    if (IsClientExist(ply.id) && GetClientTeam(ply.id) > 1 && !ply.active) {
        if (ply.FirstSpawn)
            ply.FirstSpawn = false;
        ply.active = true;
        gamemode.timer.Simple(1, "Timer_PlayerSpawn", ply);
    }

    return Plugin_Continue;
}

public void Timer_PlayerSpawn(Client ply)
{
    if(ply.spawned && IsClientExist(ply.id) && ply != null && ply.class != null)
    {
        gamemode.mngr.SetCollisionGroup(ply.id, 2);
        ply.RestrictWeapons();

        Call_StartForward(OnClientSpawnForward);
        Call_PushCellRef(ply);
        Call_Finish();

        if (ply.class.fists)
            EquipPlayerWeapon(ply.id, GivePlayerItem(ply.id, "weapon_fists"));

        ply.Setup();

        if (!IsFakeClient(ply.id))
            SendConVarValue(ply.id, FindConVar("game_type"), "6");
    }

    ply.spawned = true;
}

public void OnRoundStart(Event event, const char[] name, bool dbroadcast)
{
    if(!IsWarmup())
    {
        gamemode.mngr.RoundComplete = false;
        
        ArrayList sortedPlayers = new ArrayList();
        
        Client buf[MAXPLAYERS+1];
        Clients.GetArray("Clients", buf, sizeof(buf));

        for (int i=1; i <= Clients.InGame(); i++)
            sortedPlayers.Push(buf[i]);

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
                    Client player = sortedPlayers.Get(id);
                    sortedPlayers.Erase(id);
                    player.Team(teamKey);
                    player.class = class;
                    player.haveclass = true;

                    extra++;
                }
            }
        }

        for (int i = 1; i <= Clients.InGame() - extra; i++)
        {
            int id = sortedPlayers.Length - 1;
            if (id < 0) break;
            Client player = sortedPlayers.Get(id);
            sortedPlayers.Erase(id);
            char team[32], class[32];
            gamemode.config.DefaultGlobalClass(team, sizeof(team));
            gamemode.config.DefaultClass(class, sizeof(class));
            player.Team(team);
            player.class = gamemode.team(team).class(class);
            player.haveclass = true;
        }
        
        delete sortedPlayers;

        SetupMapRegions();
        SpawnItemsOnMap();

        gamemode.nuke.SpawnDisplay();

        CheckNewPlayers(gamemode.config.psars);

        Call_StartForward(OnRoundStartForward);
        Call_Finish();
    }
}

public void OnRoundPreStart(Event event, const char[] name, bool dbroadcast)
{
    for (int cig=1; cig <= Clients.InGame(); cig++)
    {
        Client client = Clients.Get(cig);

        Call_StartForward(OnClientClearForward);
        Call_PushCellRef(client);
        Call_Finish();

        Call_StartForward(OnClientResetForward);
        Call_PushCellRef(client);
        Call_Finish();

        client.class = null;
        client.haveclass = false;
        client.inv.Clear();
        client.active = false;
        client.spawned = true;
    }

    Ents.Clear();
    WT.Clear();
    gamemode.nuke.Reset();
    gamemode.timer.PluginClear();

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

        gamemode.nuke.Controller(doorId);

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
                else if (ply.access >= door.access || ply.inv.Check("access", door.access))
                {
                    return Plugin_Continue;
                }
                else
                {
                    return Plugin_Stop;
                }
            }
        }

        if (ply.class.escape != null && doorId == ply.class.escape.trigger)
        {
            char className[32], teamName[32];
            ply.class.escape.class(className, sizeof(className));
            if (!ply.class.escape.team(teamName, sizeof(teamName)))
                ply.Team(teamName, sizeof(teamName));

            bool savepos = ply.class.escape.savepos;

            Vector opp;
            Angle opa;

            if (savepos)
            {
                opp = ply.GetPos();
                opa = ply.GetAng();
            }

            ply.Team(teamName);
            ply.class = gamemode.team(teamName).class(className);
            
            ply.UpdateClass();

            if (savepos)
                ply.SetPos(opp, opa);
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

        Call_StartForward(OnClientClearForward);
        Call_PushCellRef(vic);
        Call_Finish();

        vic.Team("Dead");
        vic.class = null;

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

    char classname[64];
    GetEntityClassname(iWeapon, classname, sizeof(classname));

    bool weaponAllow = false;

    if (ply != null && ply.class != null && ply.class.weapons != null)
    {
        
        ArrayList meleeFix = new ArrayList(32);
        meleeFix.PushString("weapon_axe");
        meleeFix.PushString("weapon_spanner");
        meleeFix.PushString("weapon_hammer");

        char buf[32];
                
        for (int i=0; i < ply.class.weapons.Length; i++)
        {
            if (view_as<int>(ply.class.weapons.GetKeyType(i)) != 4)
            {
                ply.class.weapons.GetString(i, buf, sizeof(buf));
            }
            else
            {
                view_as<JSON_ARRAY>(ply.class.weapons.GetObject(i)).GetString(0, buf, sizeof(buf));
            }

            if (meleeFix.FindString(buf) != -1)
                buf = "weapon_melee";
            
            if (StrEqual(classname, buf))
                weaponAllow = true;
        }

        delete meleeFix;
    }

    if(ply.IsSCP && !weaponAllow)
    {
        return Plugin_Stop;
    }

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

public void SetupMapRegions() 
{
    JSON_ARRAY regions = gamemode.config.regions;

    for (int i=0; i < regions.Length; i++) 
    {
        JSON_OBJECT region = view_as<JSON_OBJECT>(regions.GetObject(i));
        Vector pos = region.GetVector("pos");
        char radius[5], name[128];
        IntToString(region.GetInt("radius"),radius,sizeof(radius));
        region.GetString("ltag",name,sizeof(name));
        
        Format(name, sizeof(name), "%T", name, LANG_SERVER);

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

public void CheckNewPlayers(int seconds)
{
    gamemode.timer.Create("PSARS", 1000, seconds, "PSARS");
}

public void PSARS()
{
    ArrayList players = Clients.GetAll();

    for (int i=0; i < players.Length; i++)
    {
        Client ply = players.Get(i);

        if (ply.FirstSpawn && IsClientInGame(ply.id) && GetClientTeam(ply.id) > 1)
        {
            char team[32], class[32];
            gamemode.config.DefaultGlobalClass(team, sizeof(team));
            gamemode.config.DefaultClass(class, sizeof(class));
            ply.Team(team);
            ply.class = gamemode.team(team).class(class);
            ply.haveclass = true;
            ply.Spawn();
        }
    }

    delete players;
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
        gamemode.mngr.PrintTeamStatus();
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

public Action TpTo(int client, const char[] command, int argc)
{
    Client ply = Clients.Get(client);

    char arg[32];

    GetCmdArg(1, arg, sizeof(arg));

    if (StrEqual(arg, "914", false))
    {
        ply.SetPos(new Vector(3100.0, -2231.0, 0.0), new Angle(0.0, 0.0, 0.0));
    }
    if (StrEqual(arg, "d", false))
    {
        ply.SetPos(new Vector(-2413.0, -5632.0, 0.0), new Angle(0.0, 0.0, 0.0));
    }
    if (StrEqual(arg, "mog", false))
    {
        ply.SetPos(new Vector(-10739.0, -5920.0, 1712.0), new Angle(0.0, 0.0, 0.0));
    }
    if (StrEqual(arg, "nuke", false))
    {
        ply.SetPos(new Vector(-7821.0, -5978.0, 200.0), new Angle(0.0, 0.0, 0.0));
    }
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
