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
//                                Commands
//
//////////////////////////////////////////////////////////////////////////////

//-----------------------------Server-----------------------------//

//-----------------------------Player-----------------------------//

public Action Command_AdminMenu(int client, int args)
{
    if(IsClientExist(client))
    {
        DisplayAdminMenu(client);
    }

    return Plugin_Handled;
}

public Action Command_Kill(int client, const char[] command, int argc)
{
    Player ply = player.GetByID(client);

    if (ply && ply.IsSCP)
    {
        PrintToConsole(client, "Самоуйбиство за класс SCP запрещено!");
        return Plugin_Handled;
    }

    ply.TakeDamage(_, 10000.0, DMG_GENERIC);

    return Plugin_Handled;
}

public Action Command_Spec(int client, int argc)
{
    Player ply = player.GetByID(client);
    
    if (GetClientTeam(ply.id) > 1)
    {
        ChangeClientTeam(ply.id, 1);
    }
    else if (GetClientTeam(ply.id) == 1)
    {
        ply.ready = true;
        CS_SwitchTeam(ply.id, GetRandomInt(2, 3));

        SetHudTextParams(0.025, 0.915, 1.0, 255, 255, 255, 255);
        ShowHudText(ply.id, 1, "");
    }

    return Plugin_Handled;
}

public Action Command_Base(int client, int argc)
{
    Player ply = player.GetByID(client);

    char arg1[32], arg2[32], arg3[32];

    GetCmdArg(1, arg1, sizeof(arg1));
    GetCmdArg(2, arg2, sizeof(arg2));
    GetCmdArg(3, arg3, sizeof(arg3));

    if (StrEqual(arg1, "timers", false))
    {
        PrintToConsole(ply.id, "-----------------------------------------Timers----------------------------------------");
        PrintToConsole(ply.id, "|                    Name                    |   delay   |  repeations  |  time left  |");
        PrintToConsole(ply.id, "---------------------------------------------------------------------------------------");

        for (int i=0; i < timer.list.Length; i++)
        {
            char tmrname[45], tmrdelay[12], tmrrepeat[15], tmrtl[14];
            Tmr tmr = timer.list.Get(i);
            tmr.name(tmrname, sizeof(tmrname));
            FormatEx(tmrdelay, sizeof(tmrdelay), "%.3f", tmr.delay);
            FormatEx(tmrrepeat, sizeof(tmrrepeat), (tmr.repeations == 0) ? "loop" : "%i", tmr.repeations);
            IntToString(tmr.GetTimeLeft(), tmrtl, sizeof(tmrtl));
            while (strlen(tmrname) <= 43) Format(tmrname, sizeof(tmrname), "%s ", tmrname);
            while (strlen(tmrdelay) <= 10) Format(tmrdelay, sizeof(tmrdelay), "%s ", tmrdelay);
            while (strlen(tmrrepeat) <= 13) Format(tmrrepeat, sizeof(tmrrepeat), "%s ", tmrrepeat);
            while (strlen(tmrtl) <= 12) Format(tmrtl, sizeof(tmrtl), "%s ", tmrtl);
            PrintToConsole(ply.id, "|%s|%s|%s|%s|", tmrname, tmrdelay, tmrrepeat, tmrtl);
        }

        PrintToConsole(ply.id, "---------------------------------------------------------------------------------------");
    }
    if (StrEqual(arg1, "se", false))
    {
        ArrayList sel = statuseffect.GetArrayList("list");

        PrintToConsole(ply.id, "---------------Status effects---------------");

        for (int i=0; i < sel.Length; i++)
        {
            char sename[64];
            StatusEffect se = sel.Get(i);
            se.name(sename, sizeof(sename));
            PrintToConsole(ply.id, "Player: %i | name: %s | time: %i | ticks left: %i", se.GetBase("player").GetInt("id"), sename, se.time, se.count);
        }
    }
    if (StrEqual(arg1, "changelevel", false))
        ServerCommand("changelevel %s", arg2);

    if (StrEqual(arg1, "round", false))
    {
        if (StrEqual(arg2, "end", false))
            if (gamemode.mngr.IsWarmup) { ServerCommand("mp_warmup_end"); PrintToConsole(ply.id, "Warmup has been forced to end..."); }
            else { gamemode.mngr.EndGame("restart"); PrintToConsole(ply.id, "Game round has been forced to end"); }
        if (StrEqual(arg2, "lock", false))
            { gamemode.mngr.RoundLock = true; PrintToConsole(ply.id, "Game round has locked"); }
        if (StrEqual(arg2, "unlock", false))
            { gamemode.mngr.RoundLock = false; PrintToConsole(ply.id, "Game round unlocked"); }
        if (StrEqual(arg2, "status", false))
        {
            ArrayList GlobalTeams = new ArrayList(32);
            ArrayList players = player.GetAll();
            int tpc[64];

            for (int i=0; i < players.Length; i++)
            {
                Player plycmd = players.Get(i);

                char plyTeamName[32];
                plycmd.Team(plyTeamName, sizeof(plyTeamName));

                int idt = GlobalTeams.FindString(plyTeamName);

                if (idt == -1) {
                    idt = GlobalTeams.PushString(plyTeamName);
                    tpc[idt] = 1;
                }
                else
                {
                    tpc[idt]++;
                }
            }

            PrintToConsole(ply.id, "------------------------------");

            for (int i = 0; i < GlobalTeams.Length; i++) {
                char buf[32];
                GlobalTeams.GetString(i, buf, sizeof(buf));
                PrintToConsole(ply.id, "Team: %s. (Count: %i)", buf, tpc[i]);
            }

            PrintToConsole(ply.id, "------------------------------");

            delete GlobalTeams;
            delete players;
        }
    }
    if (StrEqual(arg1, "entscount", false))
    {
        int entscount = 0;
        for (int i=0; i < 2048; i++) if (IsValidEntity(i)) entscount++;
        PrintToConsole(ply.id, "%i/2048", entscount);
    }

    return Plugin_Stop;
}

public Action Command_Ents(int client, int argc)
{
    Player ply = player.GetByID(client);

    char arg[32];

    GetCmdArg(1, arg, sizeof(arg));

    if (StrEqual(arg, "getall", false))
    {
        ArrayList entities = ents.GetAll();
        int idx = 0;

        if (entities.Length >= 150) idx = entities.Length - 50;

        while (idx < entities.Length)
        {
            Entity ent = entities.Get(idx);
            char name[32];

            ent.GetClass(name, sizeof(name));

            if (ent.id != 5000)
                PrintToConsole(ply.id, "%s id: %i", name, ent.id);
            else
                PrintToConsole(ply.id, "%s (picked)", name);

            idx++;
        }

        PrintToConsole(ply.id, "------------------------");

        PrintToConsole(ply.id, "Count: %i", entities.Length);

        delete entities;
    }
    
    return Plugin_Stop;
}

public Action Command_Player(int client, int argc)
{
    Player ply = player.GetByID(client);

    char arg1[32],arg2[32],arg3[32],arg4[32];

    GetCmdArg(1, arg1, sizeof(arg1));
    GetCmdArg(2, arg2, sizeof(arg2));
    GetCmdArg(3, arg3, sizeof(arg3));
    GetCmdArg(4, arg4, sizeof(arg4));

    if (StrEqual(arg1, "getall", false))
    {
        ArrayList players = player.GetAll();

        for (int i=0; i < players.Length; i++) 
        {
            Player user = players.Get(i);
            char name[32], team[32], class[32];
            user.GetName(name, sizeof(name));
            user.Team(team, sizeof(team));
            user.class.Name(class, sizeof(class));
            
            PrintToConsole(ply.id, "Id: %i | Name: %s | Team: %s | Class: %s", user.id, name, team, class);
        }

        delete players;
    }
    else if (StringToInt(arg1) <= player.InGame())
    {
        Player user = player.GetByID(StringToInt(arg1));

        if (StrEqual(arg2, "inv", false))
        {
            if (StrEqual(arg3, "getall", false))
            {
                ArrayList items = user.inv.list;
                if (items.Length == 0)
                    PrintToConsole(ply.id, "Инвентарь игрока пуст");
                else
                    for (int i=0; i < items.Length; i++)
                    {
                        char itemname[32];
                        InvItem item = items.Get(i);
                        item.GetClass(itemname, sizeof(itemname));

                        PrintToConsole(ply.id, "slot: %i | item: %s", i, itemname);
                    }
            }
            else if (StrEqual(arg3, "drop", false))
            {
                user.inv.DropByIdx(StringToInt(arg4));
            }
        }
    }
    
    return Plugin_Stop;
}

public Action Command_GetMyPos(int client, const char[] command, int argc)
{
    Player ply = player.GetByID(client);
    Vector plyPos = ply.GetPos();
    Angle plyAng = ply.GetAng();

    PrintToConsole(ply.id, "{\"vec\":[%i,%i,%i],\"ang\":[%i,%i,%i]}", RoundFloat(plyPos.x), RoundFloat(plyPos.y), RoundFloat(plyPos.z), RoundFloat(plyAng.x), RoundFloat(plyAng.y), RoundFloat(plyAng.z));

    delete plyPos;
    delete plyAng;

    return Plugin_Stop;
}

public Action Command_GetEntsInBox(int client, const char[] command, int argc)
{
    Player ply = player.GetByID(client);

    char filter[4][32] = { "prop_physics", "weapon_", "func_door", "prop_dynamic" };

    ArrayList entArr = ents.FindInBox(ply.GetPos() - new Vector(200.0, 200.0, 200.0), ply.GetPos() + new Vector(200.0, 200.0, 200.0), filter, sizeof(filter));

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
    Player ply = player.GetByID(client);

    char arg1[32], arg2[32], arg3[32], arg4[32];

    GetCmdArg(1, arg1, sizeof(arg1));
    GetCmdArg(2, arg2, sizeof(arg2));
    GetCmdArg(3, arg3, sizeof(arg3));
    GetCmdArg(4, arg4, sizeof(arg4));
    
    if (StrEqual(arg1, "set", false))
    {
        if (StrEqual(arg2, "body", false))
            ply.SetProp("m_nBody", StringToInt(arg3));
        if (StrEqual(arg2, "skin", false))
            ply.SetProp("m_nSkin", StringToInt(arg3));
    }
    if (StrEqual(arg1, "flashlight", false))
        ply.SetProp("m_fEffects", ply.GetProp("m_fEffects") ^ 4);
    if (StrEqual(arg1, "nvgs", false))
        ply.SetProp("m_bNightVisionOn", (ply.GetProp("m_bNightVisionOn") == 0) ? 1 : 0);
    if (StrEqual(arg1, "voice", false))
    {
        if (StrEqual(arg2, "mute", false))
            SetListenOverride(StringToInt(arg3), StringToInt(arg4), Listen_No);
        if (StrEqual(arg2, "unmute", false))
            SetListenOverride(StringToInt(arg3), StringToInt(arg4), Listen_Yes);
    }
    if (StrEqual(arg1, "getground"))
        PrintToChat(ply.id, "%i", GetEntPropEnt(ply.id, Prop_Send, "m_hGroundEntity"));

    return Plugin_Stop;
}