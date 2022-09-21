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

public Plugin myinfo = {
    name = "[SCP] GameMode Core",
    author = "Andrey::Dono, GeTtOo",
    description = "SCP gamemode for CS:GO",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

//////////////////////////////////////////////////////////////////////////////
//
//                                Main
//
//////////////////////////////////////////////////////////////////////////////

Handle OnLoadGM;
Handle OnUnloadGM;
Handle OnRoundStartForward;
Handle OnRoundEndForward;
Handle OnPlayerJoinForward;
Handle OnPlayerLeaveForward;
Handle PreClientSpawnForward;
Handle OnPlayerSpawnForward;
Handle PostClientSpawnForward;
Handle OnPlayerClearForward;
Handle OnPlayerSetupOverlay;
Handle OnPlayerPickupItemForward;
Handle OnPlayerPickupWeaponForward;
Handle OnPlayerSwitchWeaponForward;
Handle OnTakeDamageForward;
Handle OnPlayerDeathForward;
Handle OnPlayerEscapeForward;
Handle OnButtonPressedForward;
Handle OnInputForward;
Handle OnCallActionForward;
Handle RegMetaForward;
Handle Log_PlayerDeathForward;

#include "scp\events"
#include "scp\functions"
#include "scp\menu"
#include "scp\callbacks"
#include "scp\commands"
#include "scp\natives"
#include "scp\loader"
#include "scp\admin"

public void OnPluginStart()
{
    LoadTranslations("scpcore.phrases");
    LoadTranslations("scpcore.regions");
    LoadTranslations("scpcore.entities");
    LoadTranslations("scpcore.logs");
    
    HookEvent("round_start", OnRoundStart);
    HookEvent("round_prestart", OnRoundPreStart);
    HookEvent("player_death", Event_PlayerDeath, EventHookMode_Pre);
    
    HookEntityOutput("func_button", "OnPressed", Event_OnButtonPressed);

    RegAdminCmd("gm", Command_Base, ADMFLAG_ROOT);
    RegAdminCmd("ents", Command_Ents, ADMFLAG_ROOT);
    RegAdminCmd("player", Command_Player, ADMFLAG_ROOT);
    RegAdminCmd("scp_admin", Command_AdminMenu, ADMFLAG_GENERIC);
}

public APLRes AskPluginLoad2(Handle self, bool late, char[] error, int err_max)
{
    CreateNative("GameMode.collisiongroup.get", NativeGameMode_CollisionGroup);
    CreateNative("GameMode.GetTeamList", NativeGameMode_TeamList);
    CreateNative("GameMode.team", NativeGameMode_GetTeam);
    CreateNative("GameMode.config.get", NativeGameMode_Config);
    CreateNative("GameMode.meta.get", NativeGameMode_Meta);
    CreateNative("GameMode.mngr.get", NativeGameMode_Manager);
    CreateNative("GameMode.nuke.get", NativeGameMode_Nuke);
    CreateNative("GameMode.timer.get", NativeGameMode_Timers);
    CreateNative("GameMode.log.get", NativeGameMode_Logger);

    CreateNative("Timers.list.get", NativeTimers_GetList);
    CreateNative("Timers.HideCreate", NativeTimers_HideCreate);

    CreateNative("StatusEffectSingleton.list.get", NativeStatusEffect_GetList);
    CreateNative("StatusEffectSingleton.Create", NativeStatusEffects_Create);
    CreateNative("StatusEffectSingleton.Remove", NativeStatusEffects_Remove);
    CreateNative("StatusEffectSingleton.ClearAllOnPlayer", NativeStatusEffects_ClearAllOnPlayer);

    CreateNative("EntitySingleton.list.get", NativeEntities_GetList);
    CreateNative("EntitySingleton.Create", NativeEntities_Create);
    CreateNative("EntitySingleton.Push", NativeEntities_Push);
    CreateNative("EntitySingleton.Remove", NativeEntities_Remove);
    CreateNative("EntitySingleton.RemoveByID", NativeEntities_RemoveByID);
    CreateNative("EntitySingleton.Dissolve", NativeEntities_Dissolve);
    CreateNative("EntitySingleton.IndexUpdate", NativeEntities_IndexUpdate);
    CreateNative("EntitySingleton.Clear", NativeEntities_Clear);

    CreateNative("ClientSingleton.list.get", NativeEntities_GetList);
    CreateNative("ClientSingleton.Add", NativeClients_Add);
    CreateNative("ClientSingleton.Remove", NativeClients_Remove);

    CreateNative("WorldTextSingleton.list.get", NativeEntities_GetList);
    CreateNative("WorldTextSingleton.Create", NativeWT_Create);
    CreateNative("WorldTextSingleton.Remove", NativeWT_Remove);

    CreateNative("Player.Give", NativePlayer_GiveWeapon);
    CreateNative("Player.DropWeapons", NativePlayer_DropWeapons);
    CreateNative("Player.RestrictWeapons", NativePlayer_RestrictWeapons);

    CreateNative("Inventory.Give", NativePlayer_Inventory_GiveItem);
    CreateNative("Inventory.Drop", NativePlayer_Inventory_Drop);
    CreateNative("Inventory.DropByIdx", NativePlayer_Inventory_DropByIdx);
    CreateNative("Inventory.DropAll", NativePlayer_Inventory_DropAll);
    CreateNative("Inventory.Remove", NativePlayer_Inventory_Remove);
    CreateNative("Inventory.FullClear", NativePlayer_Inventory_FullClear);

    OnLoadGM = CreateGlobalForward("SCP_OnLoad", ET_Event);
    OnUnloadGM = CreateGlobalForward("SCP_OnUnload", ET_Event);
    OnRoundStartForward = CreateGlobalForward("SCP_OnRoundStart", ET_Event);
    OnRoundEndForward = CreateGlobalForward("SCP_OnRoundEnd", ET_Event);
    OnPlayerJoinForward = CreateGlobalForward("SCP_OnPlayerJoin", ET_Event, Param_CellByRef);
    OnPlayerLeaveForward = CreateGlobalForward("SCP_OnPlayerLeave", ET_Event, Param_CellByRef);
    PreClientSpawnForward = CreateGlobalForward("SCP_PrePlayerSpawn", ET_Event, Param_CellByRef);
    OnPlayerSpawnForward = CreateGlobalForward("SCP_OnPlayerSpawn", ET_Event, Param_CellByRef);
    PostClientSpawnForward = CreateGlobalForward("SCP_PostPlayerSpawn", ET_Event, Param_CellByRef);
    OnPlayerClearForward = CreateGlobalForward("SCP_OnPlayerClear", ET_Event, Param_CellByRef);
    OnPlayerSetupOverlay = CreateGlobalForward("SCP_OnPlayerSetupOverlay", ET_Event, Param_CellByRef);
    OnPlayerPickupItemForward = CreateGlobalForward("SCP_OnPlayerPickupItem", ET_Event, Param_CellByRef, Param_CellByRef);
    OnPlayerPickupWeaponForward = CreateGlobalForward("SCP_OnPlayerPickupWeapon", ET_Event, Param_CellByRef, Param_CellByRef);
    OnPlayerSwitchWeaponForward = CreateGlobalForward("SCP_OnPlayerSwitchWeapon", ET_Event, Param_CellByRef, Param_CellByRef);
    OnTakeDamageForward = CreateGlobalForward("SCP_OnTakeDamage", ET_Event, Param_CellByRef, Param_CellByRef, Param_FloatByRef, Param_CellByRef, Param_CellByRef);
    OnPlayerDeathForward = CreateGlobalForward("SCP_OnPlayerDeath", ET_Event, Param_CellByRef, Param_CellByRef);
    OnPlayerEscapeForward = CreateGlobalForward("SCP_OnPlayerEscape", ET_Event, Param_CellByRef, Param_CellByRef);
    OnButtonPressedForward = CreateGlobalForward("SCP_OnButtonPressed", ET_Event, Param_CellByRef, Param_Cell);
    OnInputForward = CreateGlobalForward("SCP_OnInput", ET_Event, Param_CellByRef, Param_Cell);
    OnCallActionForward = CreateGlobalForward("SCP_OnCallAction", ET_Event, Param_CellByRef);
    RegMetaForward = CreateGlobalForward("SCP_RegisterMetaData", ET_Event);
    Log_PlayerDeathForward = CreateGlobalForward("SCP_Log_PlayerDeath", ET_Event, Param_CellByRef, Param_CellByRef, Param_Cell, Param_Cell, Param_Cell);

    RegPluginLibrary("scp_core");
    return APLRes_Success;
}

bool modloaded = false;

public void OnMapStart()
{
    char mapName[128];
    GetCurrentMap(mapName, sizeof(mapName));

    if (StrContains(mapName, "scp") != -1)
        modloaded = true;
    else
    {
        LogMessage("This mod required scp_* map");
        return;
    }

    ents = new EntitySingleton();
    player = new ClientSingleton();
    worldtext = new WorldTextSingleton();
    timer = new Timers();
    statuseffect = new StatusEffectSingleton();
    AdminMenu = new AdminMenuSingleton();
    
    gamemode = new GameMode();
    
    gamemode.SetHandle("Manager", new Manager());
    gamemode.SetHandle("Nuke", new NuclearWarhead());
    gamemode.SetHandle("Logger", new Logger("SCP_OnLog", gamemode.config.logmode, gamemode.config.debug));
    
    gamemode.collisiongroup = FindSendPropInfo("CBaseEntity", "m_CollisionGroup");
    gamemode.mngr.CreateEscapeZoneList();

    AddCommandListener(Command_Kill, "kill");
    AddCommandListener(Command_GetMyPos, "getmypos");
    AddCommandListener(Command_GetEntsInBox, "getentsinbox");

    AddNormalSoundHook(SoundHandler);

    if (gamemode.config.debug) AddCommandListener(Command_Debug, "debug");

    LoadMetaData();
    
    if (gamemode.config.usablecards)
        InitKeyCards();

    Call_StartForward(RegMetaForward);
    Call_Finish();

    LoadAndPrecacheFileTable();
    
    Call_StartForward(OnLoadGM);
    Call_Finish();

    gamemode.log.Info("%t", "Log_Core_MapStart", mapName);
}

public void OnMapEnd()
{
    if (!modloaded) return;

    RemoveCommandListener(Command_Kill, "kill");
    RemoveCommandListener(Command_GetMyPos, "getmypos");
    RemoveCommandListener(Command_GetEntsInBox, "getentsinbox");

    if (gamemode.config.debug) RemoveCommandListener(Command_Debug, "debug");
    
    RemoveNormalSoundHook(SoundHandler);

    Call_StartForward(OnUnloadGM);
    Call_Finish();

    ents.Dispose();
    player.Dispose();
    AdminMenu.Dispose();
    worldtext.Dispose();
    timer.Dispose();
    statuseffect.Dispose();
    
    gamemode.config.Dispose();
    gamemode.meta.Dispose();
    gamemode.mngr.Dispose();
    gamemode.nuke.Dispose();
    gamemode.log.Dispose();
    gamemode.Dispose();
}

public void OnGameFrame()
{
    if (!modloaded) return;
    
    timer.Update();
    statuseffect.Update();
}

public void OnRebuildAdminCache(AdminCachePart part)
{
    AdminMenu.UpdateCache();
}

public void OnClientPostAdminCheck(int id)
{
    Player ply = player.Add(id);

    if(ply.IsAdmin()) AdminMenu.Add(ply);

    char clientname[32];
    ply.GetName(clientname, sizeof(clientname));
    gamemode.log.Info("%t", "Log_PlayerConnected", clientname);

    ply.SetHook(SDKHook_WeaponSwitch, OnWeaponSwitch);
    ply.SetHook(SDKHook_WeaponCanUse, OnWeaponTake);
    ply.SetHook(SDKHook_WeaponEquipPost, OnWeaponEquip);
    ply.SetHook(SDKHook_Spawn, OnPlayerSpawn);
    ply.SetHook(SDKHook_OnTakeDamage, OnTakeDamage);
    ply.SetHook(SDKHook_OnTakeDamageAlivePost, OnTakeDamageAlivePost);

    Call_StartForward(OnPlayerJoinForward);
    Call_PushCellRef(ply);
    Call_Finish();

    if (!IsFakeClient(ply.id))
    {
        SendConVarValue(ply.id, FindConVar("game_type"), "6");
        ply.SetPropFloat("m_fForceTeam", 0.0);
    }
}

public void OnClientDisconnect(int id)
{
    Player ply = player.GetByID(id);
    
    if (ply)
    {
        char timername[32];
        FormatEx(timername, sizeof(timername), "ent-%i", ply.id);
        timer.RemoveIsContains(timername);

        ply.se.ClearAll();

        char clientname[32];
        ply.GetName(clientname, sizeof(clientname));
        gamemode.log.Info("%t", "Log_PlayerDisconnected", clientname);

        ply.RemoveHook(SDKHook_WeaponSwitch, OnWeaponSwitch);
        ply.RemoveHook(SDKHook_WeaponCanUse, OnWeaponTake);
        ply.RemoveHook(SDKHook_WeaponEquipPost, OnWeaponEquip);
        ply.RemoveHook(SDKHook_Spawn, OnPlayerSpawn);
        ply.RemoveHook(SDKHook_OnTakeDamage, OnTakeDamage);
        ply.RemoveHook(SDKHook_OnTakeDamageAlivePost, OnTakeDamageAlivePost);

        Call_StartForward(OnPlayerLeaveForward);
        Call_StartForward(OnPlayerClearForward);
        Call_PushCellRef(ply);
        Call_Finish();

        ply.Team("None");
        ply.class = null;

        if (ply.GetHandle("spawnpos")) view_as<JSON_OBJECT>(ply.GetHandle("spawnpos")).SetBool("lock", false);

        if (ply.ragdoll)
        {
            delete ply.ragdoll.meta;
            ents.Remove(ply.ragdoll);
            ply.ragdoll = null;
        }

        player.Remove(id);
    }
}

public void PlayerSpawn(Player ply)
{
    if(ply && (ply.class || !ply.ready))
    {
        Call_StartForward(PreClientSpawnForward);
        Call_PushCellRef(ply);
        Call_Finish();

        if (!ply.class) return;
        
        if (ply.ragdoll) //Fix check if valid
        {
            delete ply.ragdoll.meta;
            ents.Remove(ply.ragdoll);
            ply.ragdoll = null;
        }

        ply.Spawn();
        ply.SetCollisionGroup(2);

        ply.SetupBaseStats();
        ply.SetupModel();
        ply.Setup();

        char team[32], class[32];

        ply.Team(team, sizeof(team));
        ply.class.GetString("name", class, sizeof(class));
        
        Call_StartForward(OnPlayerSpawnForward);
        Call_PushCellRef(ply);
        Call_Finish();

        if (ply.class.HasKey("overlay"))
        {
            char path[256];
            ply.class.overlay(path, sizeof(path));
            ply.ShowOverlay(path);
        
            ply.TimerSimple(gamemode.config.showoverlaytime * 1000, "PlyHideOverlay", ply);
        }
        
        ply.TimerSimple(1, "PostPlayerSpawn", ply);

        gamemode.log.Debug("Player %L spawned | Team/Class: (%s - %s)", ply.id, team, class);
    }

    ply.RemoveValue("rsptmr");
}

public void PostPlayerSpawn(Player ply)
{
    ply.SetProp("m_iHideHUD", 1<<12);

    Call_StartForward(PostClientSpawnForward);
    Call_PushCellRef(ply);
    Call_Finish();
}

public void OnEntityDestroyed(int entity)
{
    if (entity == -1) return;
    
    char entname[64];
    GetEntityClassname(entity, entname, sizeof(entname));
    if (StrEqual(entname, "dronegun")) ents.RemoveByID(entity);
    if (StrContains(entname, "weapon_") == -1) return;
    int defidx = GetEntProp(entity, Prop_Send, "m_iItemDefinitionIndex");
    if (
    (defidx == 69)    || //weapon_fists
    (defidx == 46)    || //weapon_molotov
    (defidx == 48)    || //weapon_incgrenade
    (defidx == 47)    || //weapon_decoy
    (defidx == 43)    || //weapon_flashbang
    (defidx == 44)    || //weapon_hegrenade
    (defidx == 45)    || //weapon_smokegrenade
    (defidx == 68)    || //weapon_tagrenade
    (defidx == 57)    || //weapon_healthshot
    (defidx == 84)    || //weapon_snowball
    (defidx == 70)       //weapon_breachcharge
    )
    {
        if (ents && ents.list)
        {
            Entity ent = ents.Get(entity);
            if (ent && !ent.GetBool("removing")) ents.Remove(ent);
        }
    }
}

public void OnPlayerRunCmdPost(int client, int buttons)
{
    Player ply = player.GetByID(client);
    
    if (ply && ply.class)
    {
        Call_StartForward(OnInputForward);
        Call_PushCellRef(ply);
        Call_PushCell(buttons);
        Call_Finish();
    }
}

public Action CS_OnTerminateRound(float &delay, CSRoundEndReason &reason)
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

public void SCP_OnInput(Player &ply, int buttons)
{
    if (buttons & IN_SCORE && !timer.IsAlive(view_as<Tmr>(ply.GetHandle("ActionCD"))))
    {
        ply.SetHandle("ActionCD", ply.TimerSimple(1000));

        char sound[256];
        gamemode.config.sound.GetString("menuselect", sound, sizeof(sound));
        ply.PlayNonCheckSound(sound);

        if (!ply.IsSCP) InventoryDisplay(ply);

        Call_StartForward(OnCallActionForward);
        Call_PushCellRef(ply);
        Call_Finish();
    }
}