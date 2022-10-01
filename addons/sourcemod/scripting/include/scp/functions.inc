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
//                                Functions
//
//////////////////////////////////////////////////////////////////////////////

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

public void WarmupGiveWeapon(Player ply)
{
    char melees[4][32] = {"weapon_knife", "weapon_axe", "weapon_spanner", "weapon_hammer"};

    if (ply.IsAlive()) ply.Give(melees[GetRandomInt(0, 3)]);
}

public void UpdateSpectatorInfo()
{
    ArrayList players = player.GetAll();
    
    for (int i=0; i < players.Length; i++)
    {
        Player ply = players.Get(i);

        if (!ply.IsAlive()) gamemode.mngr.GetSpecInfo(ply, player.GetByID(ply.GetPropEntId("m_hObserverTarget")));
    }
    
    delete players;
}

public void GameController()
{
    gamemode.mngr.GameCheck();
    gamemode.mngr.CheckLimitEntities();
}

public void OpenCameraDoors(JSON_ARRAY doors)
{
    for (int i=0; i < doors.Length; i++)
        AcceptEntityInput(doors.GetInt(i), "Open");
}

public void PlyHideOverlay(Player ply)
{
    ply.HideOverlay();
    
    Call_StartForward(OnPlayerSetupOverlay);
    Call_PushCellRef(ply);
    Call_Finish();
}

public void SetupIdPads()
{
    int entId = 0;
    while ((entId = FindEntityByClassname(entId, "prop_dynamic")) != -1) {
        if (!IsValidEntity(entId)) continue;

        char ModelName[256];
        GetEntPropString(entId, Prop_Data, "m_ModelName", ModelName, sizeof(ModelName));

        if (StrEqual(ModelName, "models/eternity/map/keypad.mdl"))
        {
            SetEntProp(entId, Prop_Send, "m_nSkin", (gamemode.mngr.serverlang == 22) ? 0 : 3);
        }
    }
}

public void ResetIdPad(int entid)
{
    char langcode[3];
    gamemode.mngr.GetServerLangInfo(langcode, sizeof(langcode));
    SetEntProp(entid, Prop_Send, "m_nSkin", (gamemode.mngr.serverlang == 22) ? 0 : 3);
}

public void SetPlyDoorAccess(Player &ply, Entity &item)
{
    char filter[1][32] = {"func_button"};
    ArrayList list = ents.FindInPVS(ply, 55, 90, filter);

    if (list.Length != 0)
    {
        Entity button = list.Get(0);
        char buttonid[8];
        IntToString(button.id, buttonid, sizeof(buttonid));
        if (gamemode.config.buttons.HasKey(buttonid))
        {
            ply.SetArrayList("dooraccess", item.meta.GetArrayList("access"));
            view_as<Entity>(list.Get(0)).Input("Use", ply.id);
        }
        else
        {
            ply.PrintWarning("%t", "ID pad not found");
        }

        delete button;
    }
    else
    {
        ply.PrintWarning("%t", "ID pad not found");
    }

    if (gamemode.config.usablecards)
        ply.RemoveValue("dooraccess");

    delete list;
}

public void EscapeController(Player ply, int doorID)
{
    if (gamemode.mngr.IsEscapeZone(doorID) && ply.class.escape)
    {
        EscapeInfo data = view_as<EscapeInfo>(new JSON_OBJECT());

        char className[32], teamName[32];
        ply.class.escape.team(teamName, sizeof(teamName));
        if (ply.class.escape.GetType("class") == String)
            ply.class.escape.class(className, sizeof(className));
        else
            ply.class.escape.GetArr("class").GetString(GetRandomInt(0, ply.class.escape.GetArr("class").Length - 1), className, sizeof(className));

        data.trigger = ply.class.escape.trigger;
        data.team(teamName);
        data.class(className);
        data.savepos = ply.class.escape.savepos;

        Call_StartForward(OnPlayerEscapeForward);
        Call_PushCellRef(ply);
        Call_PushCellRef(data);
        Call_Finish();

        if (doorID == data.trigger)
        {
            data.team(teamName, sizeof(teamName));
            data.class(className, sizeof(className));

            Vector opp;
            Angle opa;

            if (data.savepos)
            {
                opp = ply.GetPos();
                opa = ply.GetAng();
            }
            
            char timername[32];
            FormatEx(timername, sizeof(timername), "ent-%i", ply.id);
            timer.RemoveIsContains(timername);

            ply.se.ClearAll();

            if (ply.GetBase("spawnpos")) ply.GetBase("spawnpos").SetBool("lock", false);

            ply.Team(teamName);
            ply.class = gamemode.team(teamName).class(className);
            
            ply.inv.FullClear();

            Call_StartForward(PreClientSpawnForward);
            Call_PushCellRef(ply);
            Call_Finish();

            ply.UpdateClass();

            if (data.savepos)
                ply.SetPos(opp, opa);

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
        }

        delete data;
    }
}

public void PSARS()
{
    ArrayList players = player.GetAll();

    for (int i=0; i < players.Length; i++)
    {
        Player ply = players.Get(i);

        if (ply.FirstSpawn && GetClientTeam(ply.id) > 1)
        {
            char team[32], class[32];
            gamemode.config.DefaultGlobalClass(team, sizeof(team));
            gamemode.config.DefaultClass(class, sizeof(class));
            ply.Team(team);
            ply.class = gamemode.team(team).class(class);
            ply.Spawn();
        }
    }

    delete players;
}

public void CombatReinforcement()
{
    if (player.Alive() < RoundToNearest(float(player.InGame()) / 100.0 * float(gamemode.config.reinforce.GetInt("ratiodeadplayers")))) {
        ArrayList teams = gamemode.GetTeamList(false);
        ArrayList reinforcedteams = new ArrayList(32);

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

        delete teams;
        delete reinforcedteams;
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