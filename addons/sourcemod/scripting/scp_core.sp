/**
 * =============================================================================
 * Copyright (C) 2021 Eternity team (Andrey::Dono, GeTtOo).
 * =============================================================================
 *
 * This file is part of the SCP Breach CS:GO.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 **/

#pragma semicolon 1
#pragma newdecls required

#include <sourcemod>
#include <cstrike>
#include <sdkhooks>
// ¯\_(ツ)_/¯
#include <scpcore>
#include "include/scp/scp_admin.sp"

Handle OnLoadGM;
Handle OnUnloadGM;
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
Handle OnCallActionMenuForward;
Handle RegMetaForward;

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
    CreateNative("GameMode.GetTeamList", NativeGameMode_TeamList);
    CreateNative("GameMode.team", NativeGameMode_GetTeam);
    CreateNative("GameMode.config.get", NativeGameMode_Config);
    CreateNative("GameMode.meta.get", NativeGameMode_Meta);
    CreateNative("GameMode.mngr.get", NativeGameMode_Manager);
    CreateNative("GameMode.nuke.get", NativeGameMode_Nuke);
    CreateNative("GameMode.timer.get", NativeGameMode_Timers);
    CreateNative("GameMode.log.get", NativeGameMode_Logger);

    CreateNative("ClientSingleton.Add", NativeClients_Add);
    CreateNative("ClientSingleton.Remove", NativeClients_Remove);
    CreateNative("ClientSingleton.Get", NativeClients_Get);
    CreateNative("ClientSingleton.GetAll", NativeClients_GetAll);
    CreateNative("ClientSingleton.GetRandom", NativeClients_GetRandom);
    CreateNative("ClientSingleton.InGame", NativeClients_InGame);
    CreateNative("ClientSingleton.Alive", NativeClients_Alive);

    CreateNative("EntitySingleton.Create", NativeEntities_Create);
    CreateNative("EntitySingleton.Remove", NativeEntities_Remove);
    CreateNative("EntitySingleton.IndexUpdate", NativeEntities_IndexUpdate);
    CreateNative("EntitySingleton.Clear", NativeEntities_Clear);
    CreateNative("EntitySingleton.Get", NativeEntities_Get);
    CreateNative("EntitySingleton.TryGet", NativeEntities_TryGet);
    CreateNative("EntitySingleton.GetAll", NativeEntities_GetAll);
    
    OnLoadGM = CreateGlobalForward("SCP_OnLoad", ET_Event);
    OnUnloadGM = CreateGlobalForward("SCP_OnUnload", ET_Event);
    OnClientJoinForward = CreateGlobalForward("SCP_OnPlayerJoin", ET_Event, Param_CellByRef);
    OnClientLeaveForward = CreateGlobalForward("SCP_OnPlayerLeave", ET_Event, Param_CellByRef);
    OnClientSpawnForward = CreateGlobalForward("SCP_OnPlayerSpawn", ET_Event, Param_CellByRef);
    OnClientResetForward = CreateGlobalForward("SCP_OnPlayerReset", ET_Event, Param_CellByRef);
    OnClientClearForward = CreateGlobalForward("SCP_OnPlayerClear", ET_Event, Param_CellByRef);
    OnTakeDamageForward = CreateGlobalForward("SCP_OnTakeDamage", ET_Event, Param_CellByRef, Param_CellByRef, Param_FloatByRef, Param_CellByRef);
    OnPlayerDeathForward = CreateGlobalForward("SCP_OnPlayerDeath", ET_Event, Param_CellByRef, Param_CellByRef);
    OnButtonPressedForward = CreateGlobalForward("SCP_OnButtonPressed", ET_Event, Param_CellByRef, Param_Cell);
    OnRoundStartForward = CreateGlobalForward("SCP_OnRoundStart", ET_Event);
    OnRoundEndForward = CreateGlobalForward("SCP_OnRoundEnd", ET_Event);
    OnInputForward = CreateGlobalForward("SCP_OnInput", ET_Event, Param_CellByRef, Param_Cell);
    OnCallActionMenuForward = CreateGlobalForward("SCP_OnCallActionMenu", ET_Event, Param_CellByRef);
    RegMetaForward = CreateGlobalForward("SCP_RegisterMetaData", ET_Event);

    RegPluginLibrary("scp_core");
    return APLRes_Success;
}

public void OnPluginStart()
{
    LoadTranslations("scpcore.phrases");
    LoadTranslations("scpcore.regions");
    LoadTranslations("scpcore.entities");

    RegServerCmd("scp", CmdSCP);  // ¯\_(ツ)_/¯

    RegAdminCmd("scp_admin", Command_AdminMenu, ADMFLAG_CUSTOM1);
    
    HookEvent("round_start", OnRoundStart);
    HookEvent("round_prestart", OnRoundPreStart);
    HookEvent("player_death", Event_PlayerDeath, EventHookMode_Pre);
    
    HookEntityOutput("func_button", "OnPressed", Event_OnButtonPressed);
}

//////////////////////////////////////////////////////////////////////////////
//
//                                Events
//
//////////////////////////////////////////////////////////////////////////////
bool fixCache = false;

public void OnMapStart()
{
    Ents = new EntitySingleton();
    Clients = new ClientSingleton();
    WT = new WorldTextSingleton();
    AdminMenu = new AdminMenuSingleton();

    char mapName[128];

    GetCurrentMap(mapName, sizeof(mapName));
    gamemode = new GameMode(mapName);

    gamemode.SetValue("clients", Clients);
    gamemode.SetValue("ents", Ents);
    
    gamemode.SetValue("Manager", new Manager());
    gamemode.SetValue("Nuke", new NuclearWarhead());
    gamemode.SetValue("Logger", new Logger("SCP_OnLog", gamemode.config.logmode, gamemode.config.loglevel, gamemode.config.debug));
    
    gamemode.mngr.CollisionGroup = FindSendPropInfo("CBaseEntity", "m_CollisionGroup");

    if (gamemode.config.debug)
    {
        AddCommandListener(Command_Kill, "kill");
        AddCommandListener(Command_Ents, "ents");
        AddCommandListener(Command_GetMyPos, "getmypos");
        AddCommandListener(Command_GetEntsInBox, "getentsinbox");
        AddCommandListener(Command_Debug, "debug");
    }

    LoadMetaData(mapName);
    
    if (gamemode.config.usablecards)
        InitKeyCards();

    Call_StartForward(RegMetaForward);
    Call_Finish();

    PrecacheSound(NUKE_EXPLOSION_SOUND);
    LoadFileToDownload();
    
    Call_StartForward(OnLoadGM);
    Call_Finish();

    // ¯\_(ツ)_/¯
    if (!fixCache) {
        ForceChangeLevel(mapName, "Fix sound cached");
        fixCache = true;
    }

    gamemode.log.Info("%t", "Log_Core_MapStart", mapName);
}

public void OnMapEnd()
{
    Call_StartForward(OnUnloadGM);
    Call_Finish();
    
    delete Ents;
    delete Clients;
    delete WT;
    delete AdminMenu;
    delete gamemode;
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

    char clientauth[32];
    ply.GetAuth2(clientauth, sizeof(clientauth));
    gamemode.log.Info("%t", "Log_PlayerConnected", clientauth);

    SDKHook(ply.id, SDKHook_WeaponCanUse, OnWeaponTake);
    SDKHook(ply.id, SDKHook_Spawn, OnPlayerSpawn);
    SDKHook(ply.id, SDKHook_SpawnPost, OnPlayerSpawnPost);
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
    SDKHook(ply.id, SDKHook_OnTakeDamageAlivePost, OnTakeDamageAlivePost);

    Call_StartForward(OnClientJoinForward);
    Call_PushCellRef(ply);
    Call_Finish();

    if (!IsFakeClient(ply.id))
    {
        SendConVarValue(ply.id, FindConVar("game_type"), "6");
        ply.SetPropFloat("m_fForceTeam", 0.0);
    }
}

public void OnClientDisconnect_Post(int id)
{
    Client ply = Clients.Get(id);

    char clientauth[32];
    ply.GetAuth2(clientauth, sizeof(clientauth));
    gamemode.log.Info("%t", "Log_PlayerDisconnected", clientauth); 

    gamemode.mngr.GameCheck();

    char timername[32];
    FormatEx(timername, sizeof(timername), "entid-%i", ply.id);
    gamemode.timer.RemoveIsContains(timername);

    SDKUnhook(ply.id, SDKHook_WeaponCanUse, OnWeaponTake);
    SDKUnhook(ply.id, SDKHook_Spawn, OnPlayerSpawn);
    SDKUnhook(ply.id, SDKHook_SpawnPost, OnPlayerSpawnPost);
    SDKUnhook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
    SDKUnhook(ply.id, SDKHook_OnTakeDamageAlivePost, OnTakeDamageAlivePost);

    Base pos = ply.GetBase("spawnpos");
    if (pos != null)
        pos.SetBool("lock", false);

    Call_StartForward(OnClientClearForward);
    Call_StartForward(OnClientLeaveForward);
    Call_PushCellRef(ply);
    Call_Finish();

    Entity ragdoll = ply.ragdoll;
    if (ragdoll)
        ragdoll.Remove();

    Clients.Remove(id);
}

public Action OnPlayerSpawn(int client)
{
    if (!gamemode.mngr.IsWarmup)
    {
        Client ply = Clients.Get(client);

        if (IsClientExist(client) && GetClientTeam(client) > 1) {
            gamemode.timer.Simple(100, "PlayerSpawn", ply);
            if (ply.FirstSpawn)
                ply.FirstSpawn = false;
        }

        if (!ply.spawned) return Plugin_Handled;
    }

    return Plugin_Continue;
}

public void PlayerSpawn(Client ply)
{
    if(IsClientExist(ply.id) && ply != null && ply.class != null)
    {
        if (ply.ragdoll)
        {
            ply.ragdoll.Remove();
            ply.ragdoll = null;
        }

        ply.Spawn();

        ply.RestrictWeapons();

        if (ply.class.fists)
            EquipPlayerWeapon(ply.id, GivePlayerItem(ply.id, "weapon_fists"));

        if (ply.class.HasKey("overlay"))
        {
            char name[32];
            ply.class.overlay(name, sizeof(name));
            ply.ShowOverlay(name);
        
            ply.TimerSimple(gamemode.config.tsto * 1000, "PlyHideOverlay", ply);
        }
        
        ply.Setup();

        char team[32], class[32];

        ply.Team(team, sizeof(team));
        ply.class.GetString("name", class, sizeof(class));

        char notifytag[32];
        FormatEx(notifytag, sizeof(notifytag), "%s-%s-Notify", team, class);

        if (TranslationPhraseExists(notifytag))
        {
            char notify[2048];

            FormatEx(notify, sizeof(notify), "%t", "ControlNotify", team, class);
            Format(notify, sizeof(notify), "%s\n%t", notify, notifytag);
            
            ply.PrintNotify(notify);
        }

        Call_StartForward(OnClientSpawnForward);
        Call_PushCellRef(ply);
        Call_Finish();
        
        gamemode.log.Debug("Player %L spawned | Team/Class: (%s - %s)", ply.id, team, class);
    }
}

public Action OnPlayerSpawnPost(int client)
{
    SetEntProp(client, Prop_Send, "m_iHideHUD", 1<<12);
}

public void OnRoundStart(Event event, const char[] name, bool dbroadcast)
{
    if(!gamemode.mngr.IsWarmup)
    {
        ArrayList players = Clients.GetAll();
        players.Sort(Sort_Random, Sort_Integer);
        
        int teamCount, classCount, extra = 0;
        
        ArrayList teams = gamemode.GetTeamList();

        for (int i = 0; i < teams.Length; i++)
        {
            char teamname[32];
            teams.GetString(i, teamname, sizeof(teamname));
            
            GTeam team = gamemode.team(teamname);

            teamCount = Clients.InGame() * team.percent / 100;
            teamCount = (teamCount != 0 || !team.priority) ? teamCount : 1;

            gamemode.log.Debug("[Team] %s trying setup on %i players", teamname, teamCount);

            ArrayList classes = team.GetClassList();

            if (team.randompick)
            {
                for (int scc = 1; scc <= teamCount; scc++)
                {
                    if (extra > Clients.InGame()) break;
                    int id = players.Length - 1;
                    if (id < 0) break;
                    
                    int classid = GetRandomInt(0, classes.Length - 1);

                    char classname[32];
                    classes.GetString(classid, classname, sizeof(classname));
                    classes.Erase(classid);

                    Class class = team.class(classname);

                    Client player = players.Get(id);
                    players.Erase(id);
                    player.Team(teamname);
                    player.class = class;

                    gamemode.log.Debug("[Class] %s random setup on player: %L", classname, player.id);

                    extra++;
                }

                delete classes;
                continue;
            }
            
            for (int v = 0; v < classes.Length; v++)
            {
                char classname[32];
                classes.GetString(v, classname, sizeof(classname));

                Class class = team.class(classname);

                classCount = teamCount * class.percent / 100;
                classCount = (classCount != 0 || !class.priority) ? classCount : 1;

                /*if (gamemode.config.debug)
                    gamemode.log.Info("[Class] %s trying setup on %i players", classname, classCount);*/

                for (int scc = 1; scc <= classCount; scc++)
                {
                    if (extra > Clients.InGame()) break;
                    int id = players.Length - 1;
                    if (id < 0) break;
                    Client player = players.Get(id);
                    players.Erase(id);
                    player.Team(teamname);
                    player.class = class;

                    gamemode.log.Debug("[Class] %s setup on player: %L", classname, player.id);

                    extra++;
                }
            }

            delete classes;
        }

        for (int i = 1; i <= Clients.InGame() - extra; i++)
        {
            int id = players.Length - 1;
            if (id < 0) break;
            Client player = players.Get(id);
            players.Erase(id);
            char team[32], class[32];
            gamemode.config.DefaultGlobalClass(team, sizeof(team));
            gamemode.config.DefaultClass(class, sizeof(class));

            player.Team(team);
            player.class = gamemode.team(team).class(class);

            gamemode.log.Debug("[Extra] Team: %s, Class: %s setup on player: %L", team, class, player.id);
        }

        delete teams;
        
        delete players;

        SetupMapRegions();
        SpawnItemsOnMap();

        gamemode.nuke.SpawnDisplay();
        
        gamemode.timer.Create("SCP_Combat_Reinforcement", gamemode.config.reinforce.GetInt("time") * 1000, 0, "CombatReinforcement");

        //gamemode.timer.Create("Entities_Limit_Checker", gamemode.config.GetInt("elc", 30) * 1000, 0, "EntitiesLimitChecker");

        CheckNewPlayers(gamemode.config.psars);

        Call_StartForward(OnRoundStartForward);
        Call_Finish();
    }
}

public void OnRoundPreStart(Event event, const char[] name, bool dbroadcast)
{
    if (!gamemode.mngr.IsWarmup)
    {
        ArrayList players = Clients.GetAll();

        for (int i=0; i < players.Length; i++)
        {
            Client ply = players.Get(i);

            char timername[32];
            FormatEx(timername, sizeof(timername), "entid-%i", ply.id);
            gamemode.timer.RemoveIsContains(timername);

            Call_StartForward(OnClientClearForward);
            Call_StartForward(OnClientResetForward);
            Call_PushCellRef(ply);
            Call_Finish();

            ply.spawned = false;
            ply.Team("None");
            ply.class = null;
            ply.inv.Clear();
            
            Base pos = ply.GetBase("spawnpos");
            if (pos != null)
                pos.SetBool("lock", false);
            
            if (ply.ragdoll)
            {
                ply.ragdoll.Dispose();
                ply.ragdoll = null;
            }
        }

        Ents.Clear();
        WT.Clear();
        gamemode.mngr.RoundComplete = false;
        gamemode.nuke.Reset();
        gamemode.timer.ClearAll();

        Call_StartForward(OnRoundEndForward);
        Call_Finish();

        gamemode.log.Info("%t", "Log_Core_RoundStart");
    }
}

public Action Event_OnButtonPressed(const char[] output, int caller, int activator, float delay)
{
    if(IsClientExist(activator) && IsValidEntity(caller) && IsPlayerAlive(activator) && !IsClientInSpec(activator))
    {
        Client ply = Clients.Get(activator);
        int doorId = GetEntProp(caller, Prop_Data, "m_iHammerID");

        if (gamemode.config.debug)
            PrintToChat(ply.id, " \x07[SCP Admin] \x01Door/Button id: (%i)", doorId);

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
                int entid = GetEntPropEnt(caller, Prop_Data, "m_hMoveChild");
                Entity idpad = (entid != -1) ? new Entity(entid) : new Entity(caller);
                
                if (gamemode.mngr.IsWarmup)
                {
                    return Plugin_Continue;
                }
                else if(ply.fullaccess)
                {
                    return Plugin_Continue;
                }
                else if ((gamemode.config.usablecards && ply.Check("dooraccess", door.access)) || (!gamemode.config.usablecards && ply.inv.Check("access", door.access)) || (ply.IsSCP && door.scp)) // old check = ply.inv.Check("access", door.access)
                {
                    if (idpad)
                    {
                        if (idpad.HasProp("m_nSkin"))
                        {
                            idpad.SetProp("m_nSkin", (ply.lang == 22) ? 1 : 4); // 22 = ru lang code
                            gamemode.timer.Simple(RoundToCeil(GetEntPropFloat(caller, Prop_Data, "m_flWait")) * 1000, "ResetIdPad", idpad.id);
                        }
                    }
                    idpad.Dispose();
                }
                else
                {
                    if (idpad)
                    {
                        if (idpad.HasProp("m_nSkin"))
                        {
                            idpad.SetProp("m_nSkin", (ply.lang == 22) ? 2 : 5);
                            gamemode.timer.Simple(RoundToCeil(GetEntPropFloat(caller, Prop_Data, "m_flWait")) * 1000, "ResetIdPad", idpad.id);
                        }

                        if (!ply.IsSCP)
                        {
                            char dn[15], aln[128];
                            FormatEx(dn, sizeof(dn), "AccessLevel_%i", door.access);
                            FormatEx(aln, sizeof(aln), "%t", dn);
                            ply.PrintWarning("%t", "Door access denied", aln);
                        }
                    }
                    idpad.Dispose();
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
            
            Base pos = ply.GetBase("spawnpos");
            if (pos != null)
                pos.SetBool("lock", false);

            ply.Team(teamName);
            ply.class = gamemode.team(teamName).class(className);
            
            ply.UpdateClass();

            if (savepos)
                ply.SetPos(opp, opa);
        }

        gamemode.nuke.Controller(doorId);

        Call_StartForward(OnButtonPressedForward);
        Call_PushCellRef(ply);
        Call_PushCell(doorId);
        Call_Finish();
    }
    
    return Plugin_Continue;
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
            if (vic != null && vic.class != null)
            {
                if(vic.IsSCP && atk.IsSCP)
                {
                    return Plugin_Stop;
                }
                else if(!gamemode.config.ff)
                {
                    char vicTeam[32], atkTeam[32];
                    
                    vic.Team(vicTeam, sizeof(vicTeam));
                    atk.Team(atkTeam, sizeof(atkTeam));
                    
                    if(StrEqual(vicTeam, atkTeam)) return Plugin_Stop;
                }
            }
        }
        else
        {
            atk = null;
        }

        if (atk == null || atk.class == null) return Plugin_Continue;
        if (vic == null || vic.class == null) return Plugin_Continue;
        
        Call_StartForward(OnTakeDamageForward);
        Call_PushCellRef(vic);
        Call_PushCellRef(atk);
        Call_PushFloatRef(damage);
        Call_PushCellRef(damagetype);
        Call_Finish(result);

        return result;
    }

    return Plugin_Continue;
}

public void OnTakeDamageAlivePost(int victim, int attacker, int inflictor, float damage, int damagetype)
{   
    if(IsClientExist(victim))
    {
        Client vic = Clients.Get(victim);

        if (vic.health <= 0)
            vic.DropWeapons();
    }
}

public Action Event_PlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    if(!gamemode.mngr.IsWarmup)
    {
        Client vic = Clients.Get(GetClientOfUserId(GetEventInt(event, "userid")));
        Client atk = Clients.Get(GetClientOfUserId(GetEventInt(event, "attacker")));
        
        vic.ragdoll = vic.CreateRagdoll();

        ArrayList inv = vic.inv.items;

        while (inv.Length != 0)
        {   
            InvItem item = vic.inv.Drop();

            if (item.meta.ondrop)
            {
                char funcname[32];
                item.meta.ondrop.name(funcname, sizeof(funcname));

                Call_StartFunction(item.meta.ondrop.hndl, GetFunctionByName(item.meta.ondrop.hndl, funcname));
                Call_PushCellRef(vic);
                Call_PushCellRef(item);
                Call_Finish();
            }

            item
            .Create()
            .SetHook(SDKHook_Use, view_as<SDKHookCB>(CB_EntUse))
            .SetHook(SDKHook_TouchPost, CB_EntTouch)
            .SetPos(vic.GetPos() + new Vector(GetRandomFloat(-30.0,30.0), GetRandomFloat(-30.0,30.0), 0.0), vic.GetAng())
            .Spawn();

            if (Ents.IndexUpdate(item))
                continue;
        }

        if (vic.progress.active)
            vic.progress.Stop();

        vic.spawned = false;

        char timername[32];
        FormatEx(timername, sizeof(timername), "entid-%i", vic.id);
        gamemode.timer.RemoveIsContains(timername);

        Call_StartForward(OnClientClearForward);
        Call_PushCellRef(vic);
        Call_Finish();
        
        Call_StartForward(OnPlayerDeathForward);
        Call_PushCellRef(vic);
        Call_PushCellRef(atk);
        Call_Finish();

        vic.Team("Dead");
        vic.class = null;

        Base pos = vic.GetBase("spawnpos");
        if (pos != null)
            pos.SetBool("lock", false);

        gamemode.mngr.GameCheck();

        char vicname[32], vicauth[32];
        vic.GetName(vicname, sizeof(vicname));
        vic.GetAuth2(vicauth, sizeof(vicauth));
        
        if(atk) {
            char atkname[32], atkauth[32];

            atk.GetName(atkname, sizeof(atkname));
            atk.GetAuth2(atkauth, sizeof(atkauth));

            gamemode.log.Info("%t", "Log_Core_PlayerDead", vicname, vicauth, atkname, atkauth);
        }
        else {
            gamemode.log.Info("%t", "Log_Core_Suicide",  vicname, vicauth);
        }
    }

    return Plugin_Handled;
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

        if(ply.IsSCP && weaponAllow)
        {
            EquipPlayerWeapon(client, iWeapon);
            return Plugin_Continue;
        }
    }

    if(ply.IsSCP)
    {
        return Plugin_Handled;
    }

    if (StrEqual(classname, "weapon_melee") || StrEqual(classname, "weapon_knife"))
    {
        EquipPlayerWeapon(client, iWeapon);
    }

    return Plugin_Continue;
}

public Action CS_OnTerminateRound(float& delay, CSRoundEndReason& reason)
{
    if(gamemode.mngr.IsWarmup)
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

public void LoadMetaData(char[] mapName)
{
    LoadModels();
    LoadEntities(mapName);
}

public void LoadEntities(char[] mapName)
{
    JSON_OBJECT ents = ReadConfig(mapName, "entities");
    StringMapSnapshot sents = ents.Snapshot();

    int keylen;
    for (int i = 0; i < sents.Length; i++)
    {
        keylen = sents.KeyBufferSize(i);
        char[] entclass = new char[keylen];
        sents.GetKey(i, entclass, keylen);
        if (json_is_meta_key(entclass)) continue;

        JSON_OBJECT ent = ents.GetObject(entclass);
        StringMapSnapshot sent = ent.Snapshot();
        
        EntityMeta entdata = new EntityMeta();
        
        for (int k=0; k < sent.Length; k++)
        {
            int kl = sent.KeyBufferSize(k);
            char[] keyname = new char[kl];
            sent.GetKey(k, keyname, kl);
            if (json_is_meta_key(keyname)) continue;

            switch(ent.GetKeyType(keyname))
            {
                case 0: {
                    char str[128];
                    ent.GetString(keyname, str, sizeof(str));
                    entdata.SetString(keyname, str);
                }
                case 1: { entdata.SetInt(keyname, ent.GetInt(keyname)); }
                case 2: { entdata.SetFloat(keyname, ent.GetFloat(keyname)); }
                case 3: { entdata.SetBool(keyname, ent.GetBool(keyname)); }
                case 4: {
                    JSON_ARRAY arr = ent.GetArray(keyname);
                    ArrayList list = new ArrayList();

                    for (int v=0; v < arr.Length; v++)
                        list.Push(arr.GetInt(v));

                    entdata.SetArrayList(keyname, list);
                }
            }
        }
        
        gamemode.meta.RegisterEntity(entclass, entdata);
    }
}

public void LoadModels()
{
    JSON_ARRAY modelsdata = gamemode.config.GetObject("meta").GetArray("models");

    for (int i=0; i < modelsdata.Length; i++)
    {
        JSON_OBJECT mdlmeta = view_as<JSON_OBJECT>(modelsdata.GetObject(i));

        char id[32], path[128];
        mdlmeta.GetString("path", path, sizeof(path));
        mdlmeta.GetString("id", id, sizeof(id));
        
        JSON_ARRAY mdlbg = mdlmeta.GetArray("bginf");
        
        ModelMeta modeldata = new ModelMeta();
        ArrayList groups = new ArrayList();
        
        for (int k=0; k < mdlbg.Length; k++)
            groups.Push(mdlbg.GetInt(k));
        
        modeldata.Path(path);
        modeldata.SetArrayList("bg", groups);

        gamemode.meta.RegisterModel(id, modeldata);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//                                Callbacks
//
//////////////////////////////////////////////////////////////////////////////

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

public SDKHookCB CB_EntUse(int entity, int client)
{
    Client ply = Clients.Get(client);
    Entity ent = Ents.Get(entity);

    if (ply.IsSCP) return;

    if (ent.meta)
    {
        if (ent.meta.onpickup)
        {
            char funcname[32];
            ent.meta.onpickup.name(funcname, sizeof(funcname));

            Call_StartFunction(ent.meta.onpickup.hndl, GetFunctionByName(ent.meta.onpickup.hndl, funcname));
            Call_PushCellRef(ply);
            Call_PushCellRef(ent);
            Call_Finish();
        }

        if (!ent.meta.onpickup || !ent.meta.onpickup.invblock)
            if (ply.inv.Pickup(ent))
            {
                ent.WorldRemove();
                Ents.IndexUpdate(ent);
            }
            else
            {
                ply.PrintNotify("%t", "Inventory full");
            }
        else
        {
            ent.WorldRemove();
            Ents.IndexUpdate(ent);
        }
    }
}

public void CB_EntTouch(int firstentity, int secondentity)
{
    Entity ent1 = Ents.Get(firstentity), ent2 = Ents.Get(secondentity);

    if (ent1.meta.ontouch && ent2)
    {
        char funcname[32];
        ent1.meta.ontouch.name(funcname, sizeof(funcname));

        Call_StartFunction(ent1.meta.ontouch.hndl, GetFunctionByName(ent1.meta.ontouch.hndl, funcname));
        Call_PushCellRef(ent2);
        Call_PushCellRef(ent1);
        Call_Finish();
    }
}

public int InventoryHandler(Menu hMenu, MenuAction action, int client, int idx)
{
    if (action == MenuAction_End)
    {
        delete hMenu;
    }
    else if (action == MenuAction_Select) 
    {
        Client ply = Clients.Get(client);
        InvItem item = ply.inv.Get(idx);

        char class[32];
        item.GetClass(class, sizeof(class));

        Menu InvItmMenu = new Menu(InventoryItemHandler, MenuAction_DrawItem | MenuAction_DisplayItem | MenuAction_Select | MenuAction_End);

        char bstr[128], itemid[3];

        FormatEx(bstr, sizeof(bstr), "%T", class, ply.id);
        IntToString(idx, itemid, sizeof(itemid));
        
        InvItmMenu.SetTitle(bstr);
        InvItmMenu.AddItem(itemid, "use");
        InvItmMenu.AddItem(itemid, "drop");

        InvItmMenu.Display(ply.id, 30);
    }
}

public int InventoryItemHandler(Menu hMenu, MenuAction action, int client, int idx)
{
    switch (action)
    {
        case MenuAction_DrawItem:
        {
            switch (idx)
            {
                case 0:
                {
                    Client ply = Clients.Get(client);

                    char itemid[3];
                    hMenu.GetItem(idx, itemid, sizeof(itemid));

                    InvItem item = ply.inv.Get(StringToInt(itemid));
                    
                    return (item.meta.onuse && !item.disabled) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
                }
            }
        }
        case MenuAction_DisplayItem:
        {
            Client ply = Clients.Get(client);

            char itemid[3];
            hMenu.GetItem(idx, itemid, sizeof(itemid));

            InvItem item = ply.inv.Get(StringToInt(itemid));

            switch (idx)
            {
                case 0:
                {
                    char bstr[64], fullstr[128];
                    FormatEx(bstr, sizeof(bstr), "%T", "Use", ply.id);
                    float timeremain = item.cdr - GetGameTime();
                    if (timeremain <= 0.0) item.cdr = 0.0;
                    if (item.cdr <= 0.0) return RedrawMenuItem(bstr);
                    FormatEx(fullstr, sizeof(fullstr), "%s (%i:%i)", bstr, RoundToNearest(timeremain) / 60, RoundFloat(timeremain) % 60);
                    return RedrawMenuItem(fullstr);
                }
                case 1:
                {
                    char bstr[64];
                    FormatEx(bstr, sizeof(bstr), "%T", "Drop", ply.id);
                    return RedrawMenuItem(bstr);
                }
            }
        }
        case MenuAction_Select:
        {
            Client ply = Clients.Get(client);
            
            char itemid[3];
            hMenu.GetItem(idx, itemid, sizeof(itemid));

            switch (idx)
            {
                case 0:
                {
                    InvItem item = ply.inv.Get(StringToInt(itemid));
                    
                    char funcname[32];
                    item.meta.onuse.name(funcname, sizeof(funcname));

                    Call_StartFunction(item.meta.onuse.hndl, GetFunctionByName(item.meta.onuse.hndl, funcname));
                    Call_PushCellRef(ply);
                    Call_PushCellRef(item);
                    Call_Finish();
                }
                case 1:
                {
                    InvItem item = ply.inv.Drop(StringToInt(itemid));

                    if (item.meta.ondrop)
                    {
                        char funcname[32];
                        item.meta.ondrop.name(funcname, sizeof(funcname));

                        Call_StartFunction(item.meta.ondrop.hndl, GetFunctionByName(item.meta.ondrop.hndl, funcname));
                        Call_PushCellRef(ply);
                        Call_PushCellRef(item);
                        Call_Finish();
                    }

                    if (ply.progress.active)
                        ply.progress.Stop();

                    item
                    .Create()
                    .SetHook(SDKHook_Use, view_as<SDKHookCB>(CB_EntUse))
                    .SetHook(SDKHook_TouchPost, CB_EntTouch)
                    .SetPos(ply.GetAng().Forward(ply.EyePos(), 5.0) - new Vector(0.0, 0.0, 15.0), ply.GetAng())
                    .Spawn()
                    .ReversePush(ply.EyePos() - new Vector(0.0, 0.0, 15.0), 250.0);

                    Ents.IndexUpdate(item);
                }
            }
        }
        case MenuAction_End:
        {
            delete hMenu;
        }
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////
//
//                                Functions
//
//////////////////////////////////////////////////////////////////////////////

public void EntitiesLimitChecker()
{
    gamemode.mngr.CheckLimitEntities();
}

public void PlyHideOverlay(Client ply)
{
    ply.HideOverlay();
}

public void InitKeyCards()
{
    gamemode.meta.RegEntEvent(ON_USE, "card_o5", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "card_facility_manager", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "card_containment_engineer", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "card_mog_commander", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "card_mog_lieutenant", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "card_guard", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "card_senior_guard", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "card_zone_manager", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "card_major_scientist", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "card_scientist", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "card_janitor", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "card_chaos", "SetPlyDoorAccess");
    gamemode.meta.RegEntEvent(ON_USE, "005_picklock", "SetPlyDoorAccess");
}

public void ResetIdPad(int entid)
{
    SetEntProp(entid, Prop_Send, "m_nSkin", (gamemode.mngr.serverlang == 22) ? 0 : 3);
}

public void SetPlyDoorAccess(Client &ply, Entity &item)
{
    char filter[1][32] = {"func_button"};
    ArrayList list = Ents.FindInPVS(ply, 55, 90, filter);

    if (list.Length != 0)
    {
        Entity door = list.Get(0);
        char doorid[8];
        IntToString(door.GetProp("m_iHammerID", Prop_Data), doorid, sizeof(doorid));
        if (gamemode.config.doors.HasKey(doorid))
        {
            ply.SetArrayList("dooraccess", item.meta.GetArrayList("access"));
            view_as<Entity>(list.Get(0)).Input("Use", ply);
        }
        else
        {
            ply.PrintWarning("%t", "ID pad not found");
        }
    }
    else
    {
        ply.PrintWarning("%t", "ID pad not found");
    }

    if (gamemode.config.usablecards)
        ply.RemoveValue("dooraccess");

    delete list;
}

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
            StringMapSnapshot sdata = data.Snapshot();

            int random = GetRandomInt(1,100);
            int count = 0;
            for (int k=0; k < sdata.Length; k++)
            {
                int chancekeylen = sdata.KeyBufferSize(k);
                char[] strchance = new char[chancekeylen];
                sdata.GetKey(k, strchance, chancekeylen);
                if (json_is_meta_key(strchance)) continue;

                int chance = StringToInt(strchance);

                if (chance != 0)
                {
                    count += chance;
                    if (count >= random) {
                        data = data.GetObject(strchance);

                        Vector pos = data.GetVector("vec");
                        Angle ang = data.GetAngle("ang");

                        Ents.Create(item)
                        .SetPos(pos, ang)
                        .Spawn();
                        
                        delete sdata;
                        break;
                    }
                }
            }

            delete sdata;

            if (count != 0)
                break;

            Vector pos = data.GetVector("vec");
            Angle ang = data.GetAngle("ang");

            if (GetRandomInt(1, 100) <= data.GetInt("chance"))
                Ents.Create(item)
                .SetPos(pos, ang)
                .Spawn();
        }
    }

    delete snapshot;
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
            if (ply.IsAlive())
                ply.SilenceKill();
            ply.Team(team);
            ply.class = gamemode.team(team).class(class);
            ply.Spawn();
        }
    }

    delete players;
}

public void CombatReinforcement()
{
    if (Clients.Alive() < RoundToNearest(float(Clients.InGame()) / 100.0 * float(gamemode.config.reinforce.GetInt("ratiodeadplayers")))) {
        ArrayList teams = gamemode.GetTeamList();
        ArrayList reinforcedteams = new ArrayList();

        for (int i = 0; i < teams.Length; i++)
        {
            char teamname[32];
            teams.GetString(i, teamname, sizeof(teamname));

            if (gamemode.team(teamname).reinforce)
                reinforcedteams.PushString(teamname);
        }

        char teamname[32];
        reinforcedteams.GetString(GetRandomInt(0, reinforcedteams.Length - 1), teamname, sizeof(teamname));

        gamemode.mngr.CombatReinforcement(teamname);
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

//////////////////////////////////////////////////////////////////////////////
//
//                                Commands
//
//////////////////////////////////////////////////////////////////////////////

//-----------------------------Server-----------------------------//

public Action CmdSCP(int args)
{

    char arg1[32], arg2[32];

    GetCmdArg(1, arg1, sizeof(arg1));
    GetCmdArg(2, arg2, sizeof(arg2));

    if (StrEqual(arg1, "status", false))
    {
        gamemode.mngr.PrintTeamStatus();
    }
    if (StrEqual(arg1, "timers", false))
    {
        ArrayList timers = gamemode.timer.GetArrayList("timers");

        PrintToServer("------------Timers------------");

        for (int i=0; i < timers.Length; i++)
        {
            char timername[64];
            Tmr timer = timers.Get(i);
            timer.name(timername, sizeof(timername));
            PrintToServer("Name: %s", timername);
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

public Action Command_Kill(int client, const char[] command, int argc)
{
    Client ply = Clients.Get(client);
    
    if (ply.IsAlive())
        ply.Kill();

    return Plugin_Stop;
}

public Action Command_Ents(int client, const char[] command, int argc)
{
    Client ply = Clients.Get(client);

    char arg[32], buf[256];

    GetCmdArg(1, arg, sizeof(arg));

    int counter = 0;

    if (StrEqual(arg, "getall", false))
    {
        ArrayList ents = Ents.GetAll();

        for (int i=0; i < ents.Length; i++) 
        {
            Entity ent = ents.Get(i);
            char name[32];

            ent.GetClass(name, sizeof(name));

            if (counter != 5)
            {
                counter++;

                if (ent.id != 5000)
                    Format(buf, sizeof(buf), (counter != 5) ? "%s%s id: %i\n" : "%s%s id: %i", buf, name, ent.id);
                else
                    Format(buf, sizeof(buf), (counter != 5) ? "%s%s (picked)\n" : "%s%s (picked)", buf, name);
            }
            else
            {
                PrintToConsole(ply.id, buf);
                Format(buf, sizeof(buf), "");
                counter = 0;
            }
        }

        PrintToConsole(ply.id, "------------------------");

        PrintToConsole(ply.id, "Count: %i", ents.Length);
    }
    
    return Plugin_Stop;
}

public Action Command_GetMyPos(int client, const char[] command, int argc)
{
    Client ply = Clients.Get(client);
    Vector plyPos = ply.GetPos();
    Angle plyAng = ply.GetAng();

    PrintToConsole(ply.id, "{\"vec\":[%i,%i,%i],\"ang\":[%i,%i,%i]}", RoundFloat(plyPos.x), RoundFloat(plyPos.y), RoundFloat(plyPos.z), RoundFloat(plyAng.x), RoundFloat(plyAng.y), RoundFloat(plyAng.z));

    delete plyPos;
    delete plyAng;

    return Plugin_Stop;
}

public Action Command_GetEntsInBox(int client, const char[] command, int argc)
{
    Client ply = Clients.Get(client);

    char filter[4][32] = { "prop_physics", "weapon_", "func_door", "prop_dynamic" };

    ArrayList entArr = Ents.FindInBox(ply.GetPos() - new Vector(200.0, 200.0, 200.0), ply.GetPos() + new Vector(200.0, 200.0, 200.0), filter, sizeof(filter));

    for(int i=0; i < entArr.Length; i++) 
    {
        Entity ent = entArr.Get(i, 0);

        char entclass[32];
        ent.GetClass(entclass, sizeof(entclass));

        Vector entPos = ent.GetPos();
        Angle entAng = ent.GetAng();
        
        PrintToChat(ply.id, "class: %s, id: %i, pos: {\"vec\":[%i,%i,%i],\"ang\":[%i,%i,%i]}", entclass, ent.id, RoundFloat(entPos.x), RoundFloat(entPos.y), RoundFloat(entPos.z), RoundFloat(entAng.x), RoundFloat(entAng.y), RoundFloat(entAng.z));

        delete entPos;
        delete entAng;
    }

    delete entArr;

    return Plugin_Stop;
}

public Action Command_Debug(int client, const char[] command, int argc)
{
    Client ply = Clients.Get(client);

    char arg1[32], arg2[32], arg3[32], arg4[32];

    GetCmdArg(1, arg1, sizeof(arg1));
    GetCmdArg(2, arg2, sizeof(arg2));
    GetCmdArg(3, arg3, sizeof(arg3));
    GetCmdArg(4, arg4, sizeof(arg4));
    
    if (StrEqual(arg1, "set", false))
    {
        if (StrEqual(arg2, "body", false))
        {
            ply.SetProp("m_nBody", StringToInt(arg3));
        }
        if (StrEqual(arg2, "skin", false))
        {
            ply.SetProp("m_nSkin", StringToInt(arg3));
        }
    }
    if (StrEqual(arg1, "flashlight", false))
        ply.SetProp("m_fEffects", ply.GetProp("m_fEffects") ^ 4);
    if (StrEqual(arg1, "nvgs", false))
        ply.SetProp("m_bNightVisionOn", (ply.GetProp("m_bNightVisionOn") == 0) ? 1 : 0);
    if (StrEqual(arg1, "round", false))
    {
        if (StrEqual(arg2, "lock", false))
            gamemode.mngr.RoundLock = true;
        if (StrEqual(arg2, "unlock", false))
            gamemode.mngr.RoundLock = false;
    }

    if (StrEqual(arg1, "voice", false))
    {
        if (StrEqual(arg2, "mute", false))
            SetListenOverride(StringToInt(arg3), StringToInt(arg4), Listen_No);
        if (StrEqual(arg2, "unmute", false))
            SetListenOverride(StringToInt(arg3), StringToInt(arg4), Listen_Yes);
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

    char bstr[128];

    FormatEx(bstr, sizeof(bstr), "%T", "Inventory", ply.id);
    InvMenu.SetTitle(bstr);
    
    ArrayList inv = ply.inv.GetArrayList("inventory");

    if (inv.Length)
    {
        for (int i=0; i < inv.Length; i++)
        {
            char itemid[8], itemClass[32];

            IntToString(i, itemid, sizeof(itemid));
            view_as<InvItem>(inv.Get(i, 0)).GetClass(itemClass, sizeof(itemClass));

            FormatEx(bstr, sizeof(bstr), "%T", itemClass, ply.id);
            InvMenu.AddItem(itemid, bstr, ITEMDRAW_DEFAULT);
        }
    }
    else
    {
        ply.PrintNotify("%t", "Inventory empty");
    }

    InvMenu.Display(ply.id, 30);
}

public void SCP_OnInput(Client &ply, int buttons)
{
    if (buttons & IN_SCORE)
    {
        if (ply.GetBool("ActionMenuAvailable", true))
        {
            ply.SetBool("ActionMenuAvailable", false);
            ply.TimerSimple(1000, "ActionMenuUnlock", ply);

            if (!ply.IsSCP)
                InventoryDisplay(ply);
            else
            {
                Call_StartForward(OnCallActionMenuForward);
                Call_PushCellRef(ply);
                Call_Finish();
            }
        }
    }
}

public void ActionMenuUnlock(Client ply)
{
    ply.SetBool("ActionMenuAvailable", true);
}

//////////////////////////////////////////////////////////////////////////////
//
//                                Natives
//
//////////////////////////////////////////////////////////////////////////////

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
        entity.SetHook(SDKHook_Use, view_as<SDKHookCB>(CB_EntUse));
        entity.SetHook(SDKHook_TouchPost, CB_EntTouch);
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
    ArrayList ents = Ents.list;
    int idx = ents.FindValue(GetNativeCell(2), 0);
    Entity ent = ents.Get(idx, 1);
    if (ent.meta)
    {
        if (ent.meta.onuse)
            ent.RemoveHook(SDKHook_Use, view_as<SDKHookCB>(CB_EntUse));
        if (ent.meta.ontouch)
            ent.RemoveHook(SDKHook_TouchPost, CB_EntTouch);
    }
    ent.Remove();
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