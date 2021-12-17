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
    gamemode.meta.RegEntEvent(ON_PICKUP, "035_mask", "Logic");
}

public void SCP_OnPlayerClear(Client &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("035"))
    {
        gamemode.timer.Remove("Timer_SCP-035_Hit");
        Entity ent = view_as<Entity>(ply.GetHandle("035_ent"));
        if (ent)
            Ents.Remove(ent.id);
    }
}

public Action TransmitHandler(int entity, int client)
{
    Client ply = Clients.Get(client);

    Handle hndl = ply.GetHandle("035_ent");

    if (hndl && view_as<Entity>(hndl).id == entity)
        return Plugin_Handled;

    return Plugin_Continue;
}

public Action SCP_OnTakeDamage(Client &vic, Client &atk, float &damage, int &damagetype)
{
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

public void Logic(Client &ply, Entity &ent)
{
    Vector sp = ply.GetPos();
    Angle sa = ply.GetAng();

    ply.Kill();

    ply.Team("SCP");
    ply.class = gamemode.team("SCP").class("035");
    
    ply.Spawn();
    ply.SetPos(sp, sa);

    ArrayList data = new ArrayList();
    data.Push(ply);
    data.Push(ent);
    
    ply.TimerSimple(1000, "ExecDelay", data);
    
    gamemode.timer.Create("Timer_SCP-035_Hit", 2500, 0, "HandlerHitSCP", ply);
}

public void ExecDelay(ArrayList data)
{
    Client ply = data.Get(0);
    Entity ent = data.Get(1);

    delete data;

    Ents.IndexUpdate(ent.Create("prop_dynamic_override").Spawn());
    
    ply.SetHandle("035_ent", ent);
    ent.SetHook(SDKHook_SetTransmit, TransmitHandler);
    
    SetVariantString("!activator");
    ent.Input("SetParent", ply, ent);

    SetVariantString("facemask");
    ent.Input("SetParentAttachment", ent, ent);

    ent.SetPos(_, new Angle(0.0, 180.0, 90.0));
}