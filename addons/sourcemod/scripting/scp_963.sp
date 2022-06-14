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
    name = "[SCP] 963",
    author = "Andrey::Dono",
    description = "SCP-963-(1,2) for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_RegisterMetaData() {
    //gamemode.meta.RegEntEvent(ON_USE, "ent_id", "Function name");
    //gamemode.meta.RegEntEvent(ON_PICKUP, "ent_id", "Function name", true); // true disable pick up to inventory (def false).
    gamemode.meta.RegEntEvent(ON_PICKUP, "963_amulet", "OnPickup");
    gamemode.meta.RegEntEvent(ON_PICKUP, "963_amulet_bright", "OnPickup");
    gamemode.meta.RegEntEvent(ON_TOUCH, "963_amulet", "OnTouch");
    gamemode.meta.RegEntEvent(ON_TOUCH, "963_amulet_bright", "OnTouch");
    gamemode.meta.RegEntEvent(ON_DROP, "963_amulet", "OnDrop");
    gamemode.meta.RegEntEvent(ON_DROP, "963_amulet_bright", "OnDrop");
}

public bool OnPickup(Player &ply, InvItem &item)
{
    Player soul = view_as<Player>(item.GetHandle("soul"));

    if (soul && !soul.IsAlive() && Reincarnation(soul, ply) && soul.inv.Pickup(item))
    {
        item.WorldRemove();
        ents.IndexUpdate(item);
        
        return false;
    }

    return true;
}

public void OnTouch(Entity &ent1, Entity &ent2)
{
    if (ent1.IsClass("player") && (ent2.IsClass("963_amulet") || ent2.IsClass("963_amulet_bright")))
    {
        Player soul = view_as<Player>(ent2.GetHandle("soul"));
        Player consume = view_as<Player>(ent1);

        if (soul && !soul.IsAlive() && Reincarnation(soul, consume) && soul.inv.Pickup(ent2))
        {
            ent2.WorldRemove();
            ents.IndexUpdate(ent2);
        }
    }
}

public bool Reincarnation(Player &target, Player &consumed)
{
    if (!consumed.IsSCP && target && consumed != target)
    {
        char team[32];
        consumed.Team(team, sizeof(team));
        
        target.Team(team);
        target.class = view_as<Class>(CloneHandle(consumed.class));
        //soul.class.Remove("items");

        target.Spawn();

        target.SetBool("reincarnation", true);
        target.SetVector("soulpos", consumed.GetPos());
        target.SetAngle("soulang", consumed.GetAng());

        consumed.SetBool("bodyconsumed", true);
        consumed.RestrictWeapons();
        consumed.inv.FullClear();
        consumed.Kill();

        return true;
    }

    return false;
}

public void OnDrop(Player &ply, Entity &ent)
{
    if (ent.IsClass("963_amulet") || ent.IsClass("963_amulet_bright"))
    {
        if (!ply.IsAlive() && !ent.GetHandle("soul"))
        {
            ent.SetHandle("soul", ply);
        }
    }
}

public void SCP_OnPlayerSpawn(Player &ply)
{
    if (ply.GetBool("reincarnation"))
    {
        ply.SetPos(view_as<Vector>(ply.GetBase("soulpos")), view_as<Angle>(ply.GetBase("soulang")));

        ply.RemoveValue("soulpos");
        ply.RemoveValue("soulang");
    }
}

public void SCP_OnPlayerDeath(Player &vic, Player &atk) {
    if (vic.GetBool("bodyconsumed") && vic.ragdoll)
    {
        vic.RemoveValue("bodyconsumed");
        vic.ragdoll.Remove();
        vic.ragdoll = null;
    }
}

public void SCP_OnPlayerClear(Player &ply) {
    if (ply.GetBool("reincarnation"))
    {
        ply.RemoveValue("reincarnation");
        ply.RemoveValue("soulpos");
        ply.RemoveValue("soulang");
    }
}