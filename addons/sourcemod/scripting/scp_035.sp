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

// Урон в секунду в конфиг

public Plugin myinfo = {
    name = "[SCP] 035",
    author = "Andrey::Dono, GeTtOo",
    description = "Added SCP-035",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntEvent(ON_PICKUP, "035_mask", "Logic");
}

public void SCP_OnPlayerClear(Player &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("035"))
    {
        timer.RemoveByName("Timer_SCP-035_Hit");
        Entity ent = view_as<Entity>(ply.GetHandle("035_ent"));
        if (ent)
        {
            //ent.model.SetRenderMode(RENDER_NONE);
            ent.Input("ClearParrent");
            ents.Remove(ent);
            ply.RemoveValue("035_ent");
        }
    }
}

public Action TransmitHandler(int entity, int client)
{
    Player ply = player.GetByID(client);

    Handle hndl = ply.GetHandle("035_ent");

    if (hndl && view_as<Entity>(hndl).id == entity)
        return Plugin_Handled;

    return Plugin_Continue;
}

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype)
{
    if (atk == null || atk.class == null) return Plugin_Continue;
   
    if(atk.class.Is("035"))
    {
        damage += 180;
        return Plugin_Changed;
    }

    return Plugin_Continue;
}

public void HandlerHitSCP(Player ply)
{
    if(ply.health > 10)
        ply.health -= 10;
    else
        ply.Kill();
}

public bool Logic(Player &ply, Entity &ent)
{
    if (!ent.GetBool("used"))
    {
        Vector sp = ply.GetPos();
        Angle sa = ply.GetAng();

        char model[256];
        ply.model.GetPath(model, sizeof(model));

        Base data = new Base();
        data.SetHandle("player", ply);
        data.SetHandle("entity", ent);
        data.SetInt("skin", ply.model.GetSkin());
        data.SetString("modelname", model);

        ply.Kill();

        ply.Team("SCP");
        ply.class = gamemode.team("SCP").class("035");
        
        ply.Spawn();
        ply.SetPos(sp, sa);
        
        ply.TimerSimple(1000, "ExecDelay", data);
        
        timer.Create("Timer_SCP-035_Hit", 2500, 0, "HandlerHitSCP", ply);
        
        ent.SetBool("used", true);
    }

    return false;
}

public void ExecDelay(Base data)
{
    char model[256];
    
    Player ply = view_as<Player>(data.GetHandle("player"));
    Entity ent = view_as<Entity>(data.GetHandle("entity"));
    int skinid = data.GetInt("skin");
    data.GetString("modelname", model, sizeof(model));

    delete data;

    //ents.IndexUpdate(ent.Create("prop_dynamic_override").Spawn());
    
    ply.SetHandle("035_ent", ent);
    ply.model.SetPath(model);
    ply.model.SetSkin(skinid + 1);
    ent.SetHook(SDKHook_SetTransmit, TransmitHandler);
    
    SetVariantString("!activator");
    ent.Input("SetParent", ply.id, ent.id);

    SetVariantString("facemask");
    ent.Input("SetParentAttachment", ent.id, ent.id);

    ent.SetPos(_, new Angle(0.0, 180.0, 90.0)); // ent.SetPos(_, ply.GetAng());
}