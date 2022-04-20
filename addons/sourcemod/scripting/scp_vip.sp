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

#include <sdkhooks>
#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] Vip",
    author = "Andrey::Dono",
    description = "VIP plugin for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/author/plugin"
};

public void SCP_OnPlayerSpawn(Player &ply) {
    if (GetAdminFlag(GetUserAdmin(ply.id), Admin_Custom5))
    {
        if (ply.class.Is("D Class"))
        {
            char team[32];

            ply.Team(team, sizeof(team));

            ply.class = gamemode.team(team).class("Medic");
        }
        else if (ply.class.Is("Scientist"))
        {
            char team[32];

            ply.Team(team, sizeof(team));

            ply.class = gamemode.team(team).class("Dr.Bright");
        }
        else if (ply.class.Is("Cadet"))
        {
            char team[32];

            ply.Team(team, sizeof(team));

            ply.class = gamemode.team(team).class("Sapper");
        }
        else if (ply.class.Is("Lieutenant"))
        {
            char team[32];

            ply.Team(team, sizeof(team));

            ply.class = gamemode.team(team).class("Lieutenant-Medic");
        }
    }
}

public void SCP914_OnModify(Player &ply, int &modifychance, int &ruinechance) {
    if (GetAdminFlag(GetUserAdmin(ply.id), Admin_Custom5))
    {
        if ((modifychance + 20) <= 100)
            modifychance += 20;

        if ((ruinechance != -1) && (ruinechance - 20) >= 0)
            ruinechance -= 20;
    }
}