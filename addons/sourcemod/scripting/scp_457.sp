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

#include <sourcemod>
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

public void SCP_OnPlayerSpawn(Client &ply)
{
    if(ply.class.Is("457"))
    {
        ply.SetRenderMode(RENDER_NONE);
        
        Entity effect = (new Entity()).Create("info_particle_system");

        if(IsValidEdict(effect.id))
        {   
            effect.SetPos(ply.GetPos())
            .SetKV("targetname", "tf2particle")
            .SetKV("effect_name", "env_fire_large")
            .Spawn();
            
            SetVariantString("!activator");
            effect.Input("SetParent", ply)
            .Input("Start")
            .Activate();

            ply.SetBase("457_effect", effect);
        }
    }
}

public Action SCP_OnTakeDamage(Client &vic, Client &atk, float &damage, int &damagetype)
{
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

public void SCP_OnPlayerClear(Client &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("457") && ply.InGame())
    {
        ply.SetRenderMode(RENDER_NORMAL);

        Entity ragdoll = ply.ragdoll;
        
        if (ragdoll)
            ragdoll.Remove();

        Entity effect = view_as<Entity>(ply.GetBase("457_effect"));
        
        if (effect)
            effect.Remove();
    }
}