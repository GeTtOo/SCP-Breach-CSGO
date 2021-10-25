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
#include <json>

#pragma semicolon 1
#pragma newdecls required

// Урон в секунду в конфиг

public Plugin myinfo = {
    name = "[SCP] 035",
    author = "GeTtOo",
    description = "Added SCP-035",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntEvent(ON_PICKUP, "035_mask", "Logic", true);
}

public void SCP_OnPlayerJoin(Client &ply)
{
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void SCP_OnPlayerClear(Client &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("035"))
    {
        gamemode.timer.Remove("Timer_SCP-035_Hit");
    }
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
    Client atk = Clients.Get(attacker);

    if (atk == null || atk.class == null) return Plugin_Continue;

    if(atk.class.Is("035"))
    {
        damage += 180;
        return Plugin_Changed;
    }

    return Plugin_Continue;
}

public Action HandlerHitSCP(Client ply)
{
    if(ply.health > 10)
        ply.health -= 10;
    else
        ply.Kill();
}

public void Logic(Client &ply)
{
    Vector sp = ply.GetPos();
    Angle sa = ply.GetAng();

    ply.Kill();

    ply.Team("SCP");
    ply.class = gamemode.team("SCP").class("035");
    
    ply.Spawn();

    ply.SetPos(sp, sa);
    
    gamemode.timer.Create("Timer_SCP-035_Hit", 2500, 0, "HandlerHitSCP", ply);
}