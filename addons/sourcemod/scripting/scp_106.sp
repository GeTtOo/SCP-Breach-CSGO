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

public Plugin myinfo = {
    name = "[SCP] 106",
    author = "Andrey::Dono",
    description = "SCP-106 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnPlayerJoin(Player &ply) {

}

public void SCP_OnPlayerClear(Player &ply) {
    SDKUnhook(ply.id, SDKHook_StartTouch, CheckSurface);
}

public void SCP_OnPlayerSpawn(Player &ply) {
    if (ply != null && ply.class != null && ply.class.Is("106"))
    {
        SDKHook(ply.id, SDKHook_StartTouch, CheckSurface);
    }
}

public void Scp_OnRoundEnd()
{
    for (int i=0; i < player.Length; i++)
    {
        Player ply = player.Get(i);
        
        if (ply != null && ply.class != null && ply.class.Is("106"))
            SDKUnhook(ply.id, SDKHook_StartTouch, CheckSurface);
    }
}

public void SCP_OnButtonPressed(Player &ply, int doorId) {
    
}

public void CheckSurface(int client, int entity) {

    char className[32];
    GetEntityClassname(entity, className, sizeof(className));
    
    if (StrEqual(className, "prop_dynamic"))
    {
        int entid = GetEntPropEnt(entity, Prop_Data, "m_hMoveParent");
        Entity model = (entid != -1) ? new Entity(entid) : null;

        char dcn[32];

        model.GetClass(dcn, sizeof(dcn));

        if (StrEqual(dcn, "func_door") || StrEqual(dcn, "func_door_rotating") || StrEqual(dcn, "prop_door_rotating")) {
            if (gamemode.config.debug)
                PrintToChat(client, "Ded touched door. (ID: %i)", entity);

            int entidq = GetEntPropEnt(model.id, Prop_Data, "m_hMoveChild");
            Entity idpad = (entidq != -1) ? new Entity(entidq) : null;

            Player ply = player.Get(client);

            char t[32];
            FormatEx(t, sizeof(t), "Test_1234_%i", ply.id);
            
            ply.SetPos(ply.GetPos().GetFromPoint(idpad.GetPos()).Normalize().Scale(70.0));

            //ply.SetPos(ply.GetAng().Forward(ply.GetPos(), 70.0));
        }
    }
}

public void Test(Entity ply)
{
    ply.SetPos(ply.GetAng().Forward(ply.GetPos(), float(5)));
}