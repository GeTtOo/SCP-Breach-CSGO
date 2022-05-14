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
    name = "[SCP] 096",
    author = "Andrey::Dono",
    description = "SCP-096 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/Eternity-Development-Team/csgo_scp"
};

public void SCP_OnPlayerJoin(Player &ply) 
{
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void SCP_OnPlayerSpawn(Player &ply) {
    if (ply.class != null && ply.class.Is("096")) {

        ply.SetBool("096_IsRage", false);
        ply.SetBool("096_cooldown", false);

        SDKHook(ply.id, SDKHook_StartTouch, CheckSurface);
        
        char  timername[64];
        Format(timername, sizeof(timername), "SCP-096-%i", ply.id);
        gamemode.timer.Create(timername, 250, 0, "CheckVisualContact", ply);

        Format(timername, sizeof(timername), "SCP-096-S-%i", ply.id);
        gamemode.timer.Create(timername, 46000, 0, "Crying", ply);

        Crying(ply);
    }
}

public void SCP_OnPlayerClear(Player &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("096")) {
        
        ply.RemoveValue("096_IsRage");
        ply.RemoveValue("096_cooldown");
        ply.RemoveValue("096_candmg");
        
        char  timername[64];
        Format(timername, sizeof(timername), "SCP-096-%i", ply.id);
        gamemode.timer.RemoveByName(timername);

        Format(timername, sizeof(timername), "SCP-096-S-%i", ply.id);
        gamemode.timer.RemoveByName(timername);
    }
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	Player atk = player.GetByID(attacker);
	if (atk == null || atk.class == null) return Plugin_Continue;

	if(atk.class.Is("096"))
	{
        if (atk.GetBool("096_candmg"))
            damage += 10000.0;
        else
            damage = 0.0;

        return Plugin_Changed;
	}

	return Plugin_Continue;
}

public void CheckSurface(int client, int entity) {
    Player ply = player.GetByID(client);

    if (ply.GetBool("096_IsRage")) {
        int pid = GetEntPropEnt(entity, Prop_Data, "m_hMoveParent");
        if (pid != -1) {
            char classname[32];
            GetEntityClassname(pid, classname, sizeof(classname));
            
            if (StrEqual(classname, "func_door"))
            {
                char doorModel[256], configModel[256];
                
                Entity idpad = new Entity(entity);
                JSON_ARRAY doors = gamemode.plconfig.GetArray("doortodestruction");
                idpad.model.GetPath(doorModel, sizeof(doorModel));
                delete idpad;
                
                for(int i = 0; i < doors.Length; i++)
                {
                    doors.GetString(i, configModel, sizeof(configModel));
                    
                    if(StrContains(doorModel, configModel) != -1)
                    {
                        char sound[128];
                        JSON_ARRAY nbs = gamemode.plconfig.GetObject("sound").GetArray("doorbroke");
                        nbs.GetString(GetRandomInt(0, nbs.Length - 1), sound, sizeof(sound));

                        float vecarr[3];
                        GetEntPropVector(pid, Prop_Send, "m_vecOrigin", vecarr);
                        EmitAmbientSound(sound, vecarr, pid);

                        RemoveEntity(pid);
                    }
                }
                delete doors;
            }
        }
    }
}

public void CheckVisualContact(Player ply) 
{
    if (ply && ply.class && ply.class.Is("096") && ply.IsAlive())
    {
        bool visible = false;

        float scpPosArr[3];
        ply.EyePos().GetArrD(scpPosArr);

        char filter[1][32] = {"player"};
        ArrayList players = ents.FindInBox(ply.GetPos() - new Vector(2000.0, 2000.0, 400.0), ply.GetPos() + new Vector(2000.0, 2000.0, 400.0), filter, sizeof(filter));

        for (int i=0; i < players.Length; i++) {
            Player checkply = players.Get(i);
            
            if (ply == checkply || checkply.IsSCP || !checkply.IsAlive()) continue;

            float checkPlyPosArr[3];
            checkply.EyePos().GetArrD(checkPlyPosArr);

            ArrayList checklist = ents.FindInPVS(checkply, 2000);

            if (checklist.FindValue(ply) != -1)
            {
                Handle ray = TR_TraceRayFilterEx(checkPlyPosArr, scpPosArr, MASK_VISIBLE, RayType_EndPoint, RayFilter);
                if (!TR_DidHit(ray))
                {
                    visible = true;
                }

                delete ray;
            }

            delete checklist;
        }
        
        delete players;

        if (visible)
        {
            if (!ply.GetBool("096_IsRage") && !ply.GetBool("096_cooldown"))
            {
                ply.SetBool("096_IsRage", true);
                ply.speed = 0.1;

                char  timername[64];
                Format(timername, sizeof(timername), "SCP-096-S-%i", ply.id);
                gamemode.timer.RemoveByName(timername);
                
                ply.TimerSimple(5500, "Rage", ply);
                gamemode.mngr.PlayAmbientOnPlayer("*/eternity/scp/096/rage_start.mp3", ply);
                ply.SetBool("096_IsRage", true);
            }
        }
    }
}

public bool RayFilter(int ent, int mask, any plyidx) 
{
    if (ent >= 1 && ent <= MaxClients) return false;
    return true;
}

public void Rage(Player ply) {
    ply.SetBool("096_candmg", true);
    ply.speed = 260.0;
    ply.multipler = 2.5;

    gamemode.mngr.PlayAmbientOnPlayer("*/eternity/scp/096/rage.mp3", ply);

    ply.TimerSimple(10500, "Tranquility", ply);
}

public void Tranquility(Player ply) {
    if (ply != null && ply.class != null && ply.IsAlive()) {
        ply.speed = ply.class.speed;
        ply.multipler = ply.class.multipler;
        ply.SetBool("096_IsRage", false);
        ply.SetBool("096_cooldown", true);
        ply.SetBool("096_candmg", false);

        gamemode.mngr.PlayAmbientOnPlayer("*/eternity/scp/096/tranquility.mp3", ply);

        ply.TimerSimple(25000, "CooldownReset", ply);
        ply.TimerSimple(5900, "StartCrying", ply);
    }
}

public void StartCrying(Player ply) {
    char  timername[128];
    Format(timername, sizeof(timername), "SCP-096-S-%i", ply.id);
    gamemode.timer.Create(timername, 46000, 0, "Crying", ply);

    Crying(ply);
}

public void Crying(Player ply) {
    gamemode.mngr.PlayAmbientOnPlayer("*/eternity/scp/096/crying.mp3", ply);
}

public void CooldownReset(Player ply) {
    ply.SetBool("096_cooldown", false);
}

public void SCP_OnCallAction(Player &ply) {
    if (ply.GetBool("096_IsRage")){
        ply.TimerSimple(500, "DisableAbility", ply);
        ply.multipler *= 2;
    }
}

public void DisableAbility(Player ply) {
    if (ply.GetBool("096_IsRage")){
        ply.multipler = 2.5;
    } else {
        ply.speed = ply.class.speed;
        ply.multipler = ply.class.multipler;
    }
}