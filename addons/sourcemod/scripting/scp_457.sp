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
	name = "[SCP] 457",
	author = "GeTtOo, Andrey::Dono",
	description = "SCP-457",
	version = "1.0",
	url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnLoad()
{
    LoadTranslations("scpcore.phrases");
}

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype)
{
    if (!atk || !atk.class) return Plugin_Continue;
    
    if(atk.class.Is("457") && atk.id != vic.id)
    {
        IgniteEntity(vic.id, float(gamemode.plconfig.GetInt("ignitetime", 20)));
    }

    if(vic.class.Is("457") && damagetype == DMG_BURN)
    {
        return Plugin_Handled;
    }

    return Plugin_Continue;
}

public void SCP_OnPlayerClear(Player &ply)
{
    if (ply && ply.class && ply.class.Is("457") && ply.InGame())
    {
        ply.RemoveValue("457_abilitycd");

        if (ply.ragdoll)
        {
            ents.Remove(ply.ragdoll);
            ply.ragdoll = null;
        }
    }
}

public void SCP_OnCallAction(Player &ply)
{
    if (ply.class.Is("457") && !ply.GetBool("457_abilitycd"))
    {
        float abradius = float(gamemode.plconfig.GetInt("abradius", 250));
        
        char filter[1][32] = {"player"};
        ArrayList players = ents.FindInBox(ply.GetPos() - new Vector(abradius, abradius, 400.0), ply.GetPos() + new Vector(abradius, abradius, 400.0), filter, sizeof(filter));

        for (int i=0; i < players.Length; i++) {
            Player target = players.Get(i);
            
            if (ply == target || target.IsSCP || !target.IsAlive()) continue;

            IgniteEntity(target.id, float(gamemode.plconfig.GetInt("ignitetime", 20)));
        }

        delete players;

        ply.SetBool("457_abilitycd", true);
        ply.TimerSimple(gamemode.plconfig.GetInt("abcd", 15) * 1000, "AbilityUnlock", ply);
    }
    else if (ply.class.Is("457") && ply.GetBool("457_abilitycd"))
        ply.PrintWarning("%t", "Ability cooldown");
}

public void AbilityUnlock(Player ply)
{
    ply.SetBool("457_abilitycd", false);
    ply.progress.Stop(false);
}