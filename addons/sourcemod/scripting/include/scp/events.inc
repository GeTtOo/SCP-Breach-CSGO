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

//////////////////////////////////////////////////////////////////////////////
//
//                                Events
//
//////////////////////////////////////////////////////////////////////////////

public Action OnPlayerSpawn(int client)
{
    Player ply = player.GetByID(client);

    if (IsClientExist(client) && ply && !ply.GetHandle("rsptmr") && GetClientTeam(client) > 1)
    {
        if (!gamemode.mngr.IsWarmup)
        {
            ply.SetHandle("rsptmr", ply.TimerSimple(100, "PlayerSpawn", ply));
            if (ply.FirstSpawn) ply.FirstSpawn = false;

            //if (!ply.spawned) return Plugin_Stop;
        }
        else
        {
            ply.TimerSimple(100, "WarmupGiveWeapon", ply);
        }
    }

    return Plugin_Continue;
}

public Action OnPlayerSwitchTeam(Event event, const char[] name, bool dbroadcast)
{
    Player ply = player.GetByID(GetClientOfUserId(GetEventInt(event, "userid")));
    int team = GetEventInt(event, "team");

    if (team == 1)
    {
        ply.Kill();
        ply.ready = false;

        SetHudTextParams(0.025, 0.915, 9999.0, 102, 102, 255, 190);
        ShowHudText(ply.id, 1, "%t", "Spec msg");
    }

    return Plugin_Continue;
}

public Action OnWeaponTake(int client, int iWeapon)
{
    Player ply = player.GetByID(client);

    char classname[64];
    GetEntityClassname(iWeapon, classname, sizeof(classname));

    bool weaponAllow = false;

    if (ply && ply.class && ply.class.weapons)
    {
        ArrayList meleeFix = new ArrayList(32);
        meleeFix.PushString("weapon_axe");
        meleeFix.PushString("weapon_spanner");
        meleeFix.PushString("weapon_hammer");

        char buf[32];
                
        for (int i=0; i < ply.class.weapons.Length; i++)
        {
            if (ply.class.weapons.GetType(i) != Object)
            {
                ply.class.weapons.GetString(i, buf, sizeof(buf));
            }
            else
            {
                view_as<JSON_ARRAY>(ply.class.weapons.Get(i)).GetString(0, buf, sizeof(buf));
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

    if(ply && ply.class && ply.IsSCP)
    {
        return Plugin_Handled;
    }

    if (StrEqual(classname, "weapon_melee") || StrEqual(classname, "weapon_knife"))
    {
        EquipPlayerWeapon(client, iWeapon);
    }

    return Plugin_Continue;
}

public void OnWeaponEquip(int client, int iWeapon)
{
    Player ply = player.GetByID(client);

    char classname[64];
    GetEntityClassname(iWeapon, classname, sizeof(classname));
    
    Base data = new Base();
    data.SetHandle("player", ply);
    data.SetInt("weapon", iWeapon);
    data.SetString("wepname", classname);
    
    ply.TimerSimple(1000, "WeaponIdUpdate", data);
}

public Action OnWeaponSwitch(int client, int iWeapon)
{
    Player ply = player.GetByID(client);
    Entity ent = ents.Get(iWeapon);

    if (ply && ply.class && ent)
    {
        Call_StartForward(OnPlayerSwitchWeaponForward);
        Call_PushCellRef(ply);
        Call_PushCellRef(ent);
        Call_Finish();
    }

    return Plugin_Continue;
}

public void WeaponIdUpdate(Base data)
{
    Player ply = view_as<Player>(data.GetHandle("player"));
    int weaponid = data.GetInt("weapon");
    char weaponclass[64];
    data.GetString("wepname", weaponclass, sizeof(weaponclass));
    delete data;

    int arrsize = GetEntPropArraySize(ply.id, Prop_Send, "m_hMyWeapons");
    int item;

    for(int index = 0; index < arrsize; index++)
    { 
        item = GetEntPropEnt(ply.id, Prop_Send, "m_hMyWeapons", index);

        if(item != -1)
        {
            char classname[64];
            GetEntityClassname(item, classname, sizeof(classname));
            
            if (StrEqual(classname, weaponclass))
            {
                Entity ent = ents.Get(weaponid);
                if (ent)
                {
                    ent.id = item;
                    ents.IndexUpdate(ent);

                    Call_StartForward(OnPlayerPickupWeaponForward);
                    Call_PushCellRef(ply);
                    Call_PushCellRef(ent);
                    Call_Finish();
                }
            }
        }
    }
}

public Action OnButtonPressed(const char[] output, int btnid, int activator, float delay)
{
    if(IsClientExist(activator) && IsValidEntity(btnid) && IsPlayerAlive(activator) && !IsClientInSpec(activator))
    {
        Player ply = player.GetByID(activator);

        char langcode[3];
        ply.GetLangInfo(langcode, sizeof(langcode));

        if (gamemode.config.debug)
            PrintToChat(ply.id, " \x07[SCP Admin] \x01Door/Button id: (%i)", btnid);

        char doorKey[10];
        IntToString(btnid, doorKey, sizeof(doorKey));
        
        if (gamemode.config.buttons.HasKey(doorKey))
        {
            Button button = gamemode.config.buttons.Get(doorKey);

            int entid = GetEntPropEnt(btnid, Prop_Data, "m_hMoveChild");
            Entity idpad = (entid != -1) ? new Entity(entid) : new Entity(btnid);
            
            if (gamemode.mngr.IsWarmup)
            {
                return Plugin_Continue;
            }
            else if(ply.fullaccess)
            {
                return Plugin_Continue;
            }
            else if ((button.checkwhenuse && ply.inv.Check("access", button.access)) || (gamemode.config.usablecards && ply.Check("dooraccess", button.access)) || (!gamemode.config.usablecards && ply.inv.Check("access", button.access)) || (ply.IsSCP && button.scp)) // old check = ply.inv.Check("access", door.access)
            {
                if (idpad)
                {
                    if (idpad.HasProp("m_nSkin"))
                    {
                        idpad.SetProp("m_nSkin", (StrEqual(langcode, "ru")) ? 1 : 4); // 22 = ru lang code
                        char sound[256];
                        gamemode.config.sound.GetString("idpadag", sound, sizeof(sound));
                        gamemode.mngr.PlayTranslatedAmbient(sound, langcode, idpad);
                        timer.Simple(RoundToCeil(GetEntPropFloat(btnid, Prop_Data, "m_flWait")) * 1000, "ResetIdPad", idpad.id);
                    }
                }
            }
            else
            {
                if (idpad)
                {
                    if (idpad.HasProp("m_nSkin"))
                    {
                        idpad.SetProp("m_nSkin", (StrEqual(langcode, "ru")) ? 2 : 5);
                        char sound[256];
                        gamemode.config.sound.GetString("idpadad", sound, sizeof(sound));
                        gamemode.mngr.PlayTranslatedAmbient(sound, langcode, idpad);
                        timer.Simple(RoundToCeil(GetEntPropFloat(btnid, Prop_Data, "m_flWait")) * 1000, "ResetIdPad", idpad.id);
                    }

                    if (!ply.IsSCP)
                    {
                        char dn[15], aln[128];
                        FormatEx(dn, sizeof(dn), "AccessLevel_%i", button.access);
                        FormatEx(aln, sizeof(aln), "%t", dn);
                        ply.PrintWarning("%t", "Door access denied", aln);
                    }
                }
                
                idpad.Dispose();
                return Plugin_Stop;
            }

            idpad.Dispose();
        }

        EscapeController(ply, btnid);
        gamemode.nuke.Controller(btnid);

        Call_StartForward(OnButtonPressedForward);
        Call_PushCellRef(ply);
        Call_PushCell(btnid);
        Call_Finish();
    }
    
    return Plugin_Continue;
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{   
    if(IsClientExist(victim))
    {
        Player atk, vic = player.GetByID(victim);
        Action result;

        if(IsClientExist(attacker))
        {
            atk = player.GetByID(attacker);
            if (vic != null && vic.class != null)
            {
                if(vic.IsSCP && atk.IsSCP)
                {
                    return Plugin_Stop;
                }
                else if(!gamemode.config.ff && !gamemode.mngr.CanAttack(atk, vic))
                {
                    return Plugin_Stop;
                }
            }
        }
        else
        {
            atk = null;
        }

        //if (atk == null || atk.class == null) return Plugin_Continue;
        if (!vic || !vic.class) return Plugin_Continue;
        
        Call_StartForward(OnTakeDamageForward);
        Call_PushCellRef(vic);
        Call_PushCellRef(atk);
        Call_PushFloatRef(damage);
        Call_PushCellRef(damagetype);
        Call_PushCellRef(inflictor);
        Call_Finish(result);

        return result;
    }

    return Plugin_Continue;
}

public void OnTakeDamageAlivePost(int victim, int attacker, int inflictor, float damage, int damagetype)
{   
    if(IsClientExist(victim))
    {
        Player vic = player.GetByID(victim);
        Player atk = null;
        
        if(IsClientExist(attacker))
            atk = player.GetByID(attacker);

        if (vic.health <= 0)
        {
            vic.DropWeapons();

            bool logchange = false;
                
            Call_StartForward(Log_PlayerDeathForward);
            Call_PushCellRef(vic);
            Call_PushCellRef(atk);
            Call_PushCell(damage);
            Call_PushCell(damagetype);
            Call_PushCell(inflictor);
            Call_Finish(logchange);

            if (logchange) return;

            char vicname[32], vicauth[32], victeam[32];
            vic.GetName(vicname, sizeof(vicname));
            vic.GetAuth(vicauth, sizeof(vicauth));
            vic.Team(victeam, sizeof(victeam));
            
            if(atk)
            {
                char atkname[32], atkauth[32], atkteam[32];

                atk.GetName(atkname, sizeof(atkname));
                atk.GetAuth(atkauth, sizeof(atkauth));
                atk.Team(atkteam, sizeof(atkteam));

                gamemode.log.Info("%t", "Log_Core_PlayerDead", vicname, vicauth, victeam, atkname, atkauth, atkteam);
            }
            else
            {
                switch (damagetype)
                {
                    case DMG_BLAST:
                        gamemode.log.Info("%t", "Log_Core_Death_By_Alpha_Warhead",  vicname, vicauth);
                    case DMG_RADIATION:
                        gamemode.log.Info("%t", "Log_Core_Death_By_Radiation",  vicname, vicauth);
                    default:
                        gamemode.log.Info("%t", "Log_Core_Suicide",  vicname, vicauth);
                }
            }
        }
    }
}

public void OnPlayerDeath(Player vic, Player atk)
{
    if(!gamemode.mngr.IsWarmup)
    {
        if (!vic) return;

        vic.ragdoll = vic.CreateRagdoll();
        ents.Push(vic.ragdoll);

        Call_StartForward(PrePlayerClearForward);
        Call_PushCellRef(vic);
        Call_Finish();

        vic.inv.DropAll();
        vic.se.ClearAll();

        vic.HideOverlay();

        if (vic.progress.active)
            vic.progress.Stop();

        vic.spawned = false;

        char timername[32];
        FormatEx(timername, sizeof(timername), "ent-%i", vic.id);
        timer.RemoveIsContains(timername);

        Call_StartForward(OnPlayerDeathForward);
        Call_PushCellRef(vic);
        Call_PushCellRef(atk);
        Call_Finish();

        Call_StartForward(OnPlayerClearForward);
        Call_PushCellRef(vic);
        Call_Finish();

        vic.Team("Dead");
        vic.class = null;

        if (vic.GetHandle("spawnpos")) view_as<JSON_OBJECT>(vic.GetHandle("spawnpos")).SetBool("lock", false);
    }
}

public Action OnKillNotify(Event event, const char[] name, bool dontBroadcast)
{
    OnPlayerDeath(player.GetByID(GetClientOfUserId(GetEventInt(event, "userid"))), player.GetByID(GetClientOfUserId(GetEventInt(event, "attacker"))));

    return Plugin_Handled;
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

public void OnRoundStart(Event event, const char[] name, bool dbroadcast)
{
    if(!gamemode.mngr.IsWarmup)
    {
        ArrayList players = player.GetAll();
        players.Sort(Sort_Random, Sort_Integer);
        
        for (int i=0; i < players.Length; i++)
        {
            Player ply = players.Get(i);
            if (ply.ready) continue;
            players.Erase(i);
            i--;
        }
        
        int teamCount, classCount, extra = 0, playerscount = players.Length;
        
        ArrayList teams = gamemode.GetTeamList();

        for (int i = 0; i < teams.Length; i++)
        {
            char teamname[32];
            teams.GetString(i, teamname, sizeof(teamname));
            
            GTeam team = gamemode.team(teamname);

            teamCount = playerscount * team.percent / 100;
            teamCount = (teamCount != 0 || !team.priority) ? teamCount : 1;

            gamemode.log.Debug("[Team] %s trying setup on %i players", teamname, teamCount);

            ArrayList classes = team.GetClassList();

            if (team.randompick)
            {
                for (int scc = 1; scc <= teamCount; scc++)
                {
                    if (extra > playerscount) break;
                    int id = players.Length - 1;
                    if (id < 0) break;
                    
                    int classid = GetRandomInt(0, classes.Length - 1);

                    char classname[32];
                    classes.GetString(classid, classname, sizeof(classname));
                    classes.Erase(classid);

                    Class class = team.class(classname);

                    Player ply = players.Get(id);
                    players.Erase(id);
                    ply.Team(teamname);
                    ply.class = class;

                    gamemode.log.Debug("[Class] %s random setup on player: %L", classname, ply.id);

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
                    if (extra > playerscount) break;
                    int id = players.Length - 1;
                    if (id < 0) break;
                    Player ply = players.Get(id);
                    players.Erase(id);
                    ply.Team(teamname);
                    ply.class = class;

                    gamemode.log.Debug("[Class] %s setup on player: %L", classname, ply.id);

                    extra++;
                }
            }

            delete classes;
        }

        for (int i = 1; i <= playerscount - extra; i++)
        {
            int id = players.Length - 1;
            if (id < 0) break;
            Player ply = players.Get(id);
            players.Erase(id);
            char team[32], class[32];
            gamemode.config.DefaultGlobalClass(team, sizeof(team));
            gamemode.config.DefaultClass(class, sizeof(class));

            ply.Team(team);
            ply.class = gamemode.team(team).class(class);

            gamemode.log.Debug("[Extra] Team: %s, Class: %s setup on player: %L", team, class, ply.id);
        }

        delete teams;
        
        delete players;

        SetupMapRegions();
        SetupIdPads();
        SpawnItemsOnMap();

        gamemode.mngr.rst = GetTime();

        gamemode.nuke.SpawnDisplay();
        if (gamemode.config.nuke.autostart) gamemode.nuke.AutoStart(gamemode.config.nuke.ast);
        
        timer.Create("CombatReinforcement", gamemode.config.reinforce.GetInt("time", 300) * 1000, 0, "CombatReinforcement");
        timer.Create("UpdateSpectatorInfo", 1000, 0, "UpdateSpectatorInfo");
        timer.Create("GameController", gamemode.config.GetInt("updategc", 5) * 1000, 0, "GameController");
        timer.Create("PlayerSpawnAfterRoundStart", 1000, gamemode.config.psars, "PSARS");

        Call_StartForward(OnRoundStartForward);
        Call_Finish();
    }
}

public void OnRoundPreStart(Event event, const char[] name, bool dbroadcast)
{
    if (!gamemode.mngr.IsWarmup)
    {
        ArrayList players = player.GetAll();

        for (int i=0; i < players.Length; i++)
        {
            Player ply = players.Get(i);

            char timername[32];
            FormatEx(timername, sizeof(timername), "ent-%i", ply.id);
            timer.RemoveIsContains(timername);

            Call_StartForward(OnPlayerClearForward);
            Call_PushCellRef(ply);
            Call_Finish();

            ply.spawned = false;
            ply.Team("None");
            ply.class = null;
            ply.inv.Clear();
            ply.progress.Stop(false);
            
            ply.RestrictWeapons();

            if (ply.GetHandle("spawnpos")) view_as<JSON_OBJECT>(ply.GetHandle("spawnpos")).SetBool("lock", false);
            
            if (ply.ragdoll)
            {
                delete ply.ragdoll.meta;
                ents.Remove(ply.ragdoll);
                ply.ragdoll = null;
            }
        }

        delete players;

        ents.Clear();
        gamemode.mngr.RoundComplete = false;
        gamemode.nuke.Reset();
        statuseffect.ClearAll();
        timer.ClearAll();

        Call_StartForward(OnRoundEndForward);
        Call_Finish();

        gamemode.log.Info("%t", "Log_Core_RoundStart");
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

public Action OnPlayerPing(int client, const char[] command, int args)
{
	return Plugin_Stop;
}