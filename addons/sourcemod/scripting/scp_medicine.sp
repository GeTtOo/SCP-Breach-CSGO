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
    name = "[SCP] HealthShot",
    author = "Andrey::Dono, GeTtOo",
    description = "Removes hp restriction when using healthshot",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_RegisterMetaData() {
    gamemode.meta.RegisterStatusEffect("Injure", 0.5);
    gamemode.meta.RegStatusEffectEvent(INIT, "Injure", "Injure_Init");
    gamemode.meta.RegStatusEffectEvent(UPDATE, "Injure", "Injure_Update");
    gamemode.meta.RegStatusEffectEvent(END, "Injure", "Injure_End");

    gamemode.meta.RegisterStatusEffect("Heal", 1.5);
    gamemode.meta.RegStatusEffectEvent(INIT, "Heal", "Heal_Init");
    gamemode.meta.RegStatusEffectEvent(UPDATE, "Heal", "Heal_Update");

    gamemode.meta.RegEntEvent(ON_PICKUP, "medkit", "MedkitUse");
    gamemode.meta.RegEntEvent(ON_USE, "medkit", "MedkitDeploy");
}

//////////////////////////////////////////////////////////////////////////////
//
//                              Heal status effect
//
//////////////////////////////////////////////////////////////////////////////

public void Heal_Init(Player ply) {
    ply.PrintWarning("Скорость твоего метаболизма возрасла в разы...");
}

public void Heal_Update(Player ply) {
    if (ply.health < ply.class.health)
        if (ply.health + (ply.class.health * 8 / 100) > ply.class.health)
            ply.health = ply.class.health;
        else
            ply.health += ply.class.health * 8 / 100;
}

//////////////////////////////////////////////////////////////////////////////
//
//                              Injure status effect
//
//////////////////////////////////////////////////////////////////////////////

public void Injure_Init(Player ply) {
    ply.speed = ply.class.speed / 1.4;

    ply.PrintWarning("Ваше тело начинает кровоточить из множества мелких ран...");

    char sound[128];
    JSON_ARRAY soundarr = gamemode.plconfig.GetObject("sound").GetArray("playerkill");
    soundarr.GetString(GetRandomInt(0, soundarr.Length - 1), sound, sizeof(sound));
    
    ply.PlaySound(sound);
}

public void Injure_Update(Player ply) {
    ply.health -= (ply.class.health * 2 / 100);
}

public void Injure_End(Player ply) {
    ply.PrintWarning("Вы чувствуете себя немного лучше...");
}

public void Injure_ForceEnd(Player ply) {
    if (ply.class) ply.speed = ply.class.speed;
}

//////////////////////////////////////////////////////////////////////////////
//
//                              Main
//
//////////////////////////////////////////////////////////////////////////////

public void SCP_OnLoad()
{
    HookEvent("weapon_fire", Event_WeaponFire, EventHookMode_Pre);
}

public Action Event_WeaponFire(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    char weapon[64];
    event.GetString("weapon", weapon, sizeof(weapon));

    Player ply = player.GetByID(client);

    if(StrEqual(weapon, "weapon_healthshot") && ply.GetBool("ha") && !ply.GetBool("hip"))
    {
        ply.SetInt("temphealth", ply.health);
        ply.SetBool("hip", true);
        ply.SetBool("hwb", false);
        ply.TimerSimple(800, "HealthShotTimer", ply);
    }

    return Plugin_Continue;
}

public void SCP_OnPlayerClear(Player &ply)
{
    if (ply && ply.class) {
        ply.RemoveValue("hip");
        ply.RemoveValue("temphealth");
    }
}

public void SCP_OnPlayerSwitchWeapon(Player &ply, Entity &ent)
{
    if (ent.IsClass("weapon_healthshot"))
        ply.SetBool("ha", true);
    else
    {
        ply.SetBool("hwb", true);
        ply.SetBool("ha", false);
    }
}

public void HealthShotTimer(Player ply)
{
    ply.health = ply.GetInt("temphealth");
    ply.RemoveValue("hip");
    ply.RemoveValue("temphealth");

    if (ply.GetBool("ha") && !ply.GetBool("hwb")) 
    {
        ply.se.Remove("Injure");
        
        if (ply.health < ply.class.health) {
            if (ply.health + (ply.class.health * 25 / 100) > ply.class.health)
                ply.health = ply.class.health;
            else
                ply.health += ply.class.health * 25 / 100;
        }

        ply.se.Create("Heal", 5);
    }
}

public bool MedkitUse(Player &ply, Entity &ent)
{
    if (ent.GetBool("active") && ent.GetInt("amount") > 0)
    {
        ply.Give("weapon_healthshot");
        int count = ent.GetInt("amount") - 1;
        ent.SetInt("amount", count);

        char countstr[12];
        WorldText info = view_as<WorldText>(ent.GetHandle("wtinfo"));
        FormatEx(countstr, sizeof(countstr), "count:%i", count);
        info.SetText(countstr);

        if (ent.GetInt("amount") == 0)
        {
            ents.Remove(info);
            ents.Remove(ent);
        }
        
        return false;
    }

    return true;
}

public void MedkitDeploy(Player &ply, InvItem &ent)
{
    ply.inv.Drop(ent);

    ent.TimerSimple(2000, "MedkitInit", ent);
}

public void MedkitInit(Entity ent)
{   
    Vector pos = ent.GetPos();
    
    ent.meta.spawnflags = 4364; // Add motion disabled flag
    ents.Remove(ent);
    
    ent = ents.Create("medkit");
    ent.meta.spawnflags = 4356; // Flags original value restore
    ent.SetPos(pos - new Vector(0.0,0.0,5.0), new Angle(90.0,0.0,0.0));
    ent.Spawn();
    
    ent.SetBool("active", true);
    ent.SetInt("amount", ent.meta.GetInt("amount"));

    char medkitname[32];
    FormatEx(medkitname, sizeof(medkitname), "medkit-%i", ent.id);
    ent.SetKV("targetname", medkitname);

    WorldText info = worldtext.Create(ent.GetPos() + new Vector(1.5,8.5,6.2), ent.GetAng() - new Angle(0.0,270.0,0.0));
    info.SetColor(new Colour(218,18,26));
    info.SetText(" Full");
    info.SetSize(3);
    
    SetVariantString(medkitname);
    info.Input("SetParent", ent.id);
    
    ent.SetHandle("wtinfo", info);
}