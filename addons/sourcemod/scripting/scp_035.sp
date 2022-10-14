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

#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

// Урон в секунду в конфиг

public Plugin myinfo = {
    name = "[SCP] 035",
    author = "Andrey::Dono, GeTtOo",
    description = "Added SCP-035",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntEvent(ON_PICKUP, "035_mask", "Setup");
}

public void SCP_OnPlayerClear(Player &ply)
{
    if (ply.class && ply.class.Is("035"))
    {
        char timername[64];
        Format(timername, sizeof(timername), "hit-035-%i", ply.id);
        timer.RemoveByName(timername);

        Entity ent = view_as<Entity>(ply.GetHandle("035_ent"));
        if (ent)
        {
            //char entclass[32];
            //ent.GetClass(entclass, sizeof(entclass));

            ent.Input("ClearParent");

            ents.Remove(ent);
            //ents.Create(entclass).SetPos(ply.ragdoll.GetPos()).Spawn().Input("Wake");

            ply.RemoveValue("035_ent");
            ply.RemoveValue("035_decayed");
        }

        SetEntityRenderColor(ply.id, 255, 255, 255);
        SetEntityRenderColor(ply.ragdoll.id, 0, 0, 0);
    }
}

public Action TransmitHandler(int entity, int client)
{
    Entity ent = view_as<Entity>(player.GetByID(client).GetBase("035_ent"));

    if (ent && ent.id == entity)
        return Plugin_Handled;

    return Plugin_Continue;
}

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype)
{
    if (!atk || !atk.class) return Plugin_Continue;
   
    if(atk.class.Is("035"))
    {
        damage += 180;
        return Plugin_Changed;
    }

    return Plugin_Continue;
}

public bool Setup(Player &ply, Entity &ent)
{
    ply.Team("SCP");
    ply.class = gamemode.team("SCP").class("035");
    
    ply.Spawn(false,false,_,true);
    
    ply.SetHandle("035_ent", ent);
    
    ent.SetProp("m_fEffects", 1);
    ent.SetHook(SDKHook_SetTransmit, TransmitHandler);

    SetVariantString("!activator");
    ent.Input("SetParent", ply.id, ent.id);

    SetVariantString("facemask");
    ent.Input("SetParentAttachment", ent.id, ent.id);
    
    char timername[64];
    Format(timername, sizeof(timername), "hit-035-%i", ply.id);
    timer.Create(timername, 15000, 0, "HandlerHitSCP", ply);

    return false;
}

public void HandlerHitSCP(Player ply)
{
    if (!ply.class) return;

    ply.TakeDamage(_,250.0);

    if (ply.health < ply.class.health / 100 * 75 && !ply.HasKey("035_decayed"))
    {
        ply.model.SetSkin(ply.model.GetSkin() + 1);
        ply.SetBool("035_decayed", true);
    }

    int color = RoundToCeil(float(ply.class.health / 100 * 255) / ply.class.health * (float(ply.health) / float(ply.class.health) * float(100)));

    SetEntityRenderColor(ply.id, color, color, color);
}