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
    name = "[SCP] Plugin example",
    author = "Anyone",
    description = "Plugin example for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/author/plugin"
};

public void SCP_OnPlayerSwitchWeapon(Player &ply, Entity &ent)
{
    if (!ply.IsSCP && !ply.GetHandle("fct"))
        if (ent.IsClass("weapon_fists"))
            ply.ShowOverlay("fists");
        else
            ply.HideOverlay();
}

public void SCP_OnPlayerSpawn(Player &ply)
{
    ply.SetHandle("fct", ply.TimerSimple(gamemode.config.tsto * 1010, "ShowCrosshair", ply));
}

public void ShowCrosshair(Player ply)
{
    if (ply.GetHandle("fct"))
    {
        ply.RemoveValue("fct");
        
        if (ply.class.Is("D Class") || ply.class.Is("Scientist") || ply.class.Is("Dr.Bright"))
            ply.ShowOverlay("fists");
    }
}