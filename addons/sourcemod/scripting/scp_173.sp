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
    name = "[SCP] 173",
    author = "Andrey::Dono",
    description = "SCP 173 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnPlayerSpawn(Player &ply) 
{
    if (ply.class != null && ply.class.Is("173")) {
        char  timername[64];
        
        Format(timername, sizeof(timername), "SCP-173-VisChecker-%i", ply.id);
        gamemode.timer.Create(timername, 250, 0, "CheckVisualContact", ply);

        Format(timername, sizeof(timername), "SCP-173-Holo-%i", ply.id);
        gamemode.timer.Create(timername, 100, 0, "RenderHologram", ply);
    }
}

public void SCP_OnPlayerClear(Player &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("173")) {
        char  timername[64];

        Format(timername, sizeof(timername), "SCP-173-VisChecker-%i", ply.id);
        gamemode.timer.Remove(timername);

        Format(timername, sizeof(timername), "SCP-173-Holo-%i", ply.id);
        gamemode.timer.Remove(timername);

        Entity ent = view_as<Entity>(ply.GetHandle("173_holo"));
        if (ent)
        {
            ents.Remove(ent);
            delete ent;
        }

        ply.RemoveValue("173_isvis");
        ply.RemoveValue("173_abtmr");
        ply.RemoveValue("173_abready");
        ply.RemoveValue("173_holo");
    }
}

public void SCP_OnPlayerDeath(Player &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("173")) {
        char sound[128];
        JSON_ARRAY ds = gamemode.plconfig.GetObject("sound").GetArray("death");
        ds.GetString(GetRandomInt(0, ds.Length - 1), sound, sizeof(sound));
        gamemode.mngr.PlayAmbientOnPlayer(sound, ply);
    }
}

public void CheckVisualContact(Player ply) 
{
    if (ply != null && ply.class != null && ply.IsAlive() && ply.class.Is("173")) 
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

                    if (!ply.GetHandle("173_abtmr") && !ply.GetBool("173_abready"))
                        gamemode.mngr.Fade(checkply.id, 25, 250, new Colour(0,0,0,255));
                }

                delete ray;
            }

            delete checklist;
        }
        
        delete players;

        if (visible)
        {
            if (!ply.GetBool("173_isvis"))
            {
                ply.SetMoveType(MOVETYPE_NONE);
                ply.SetBool("173_isvis", true);

                if (!ply.GetHandle("173_abtmr") && !ply.GetBool("173_abready"))
                    ply.SetHandle("173_abtmr", ply.TimerSimple(5000, "AbilityCooldown", ply));

            }
        }
        else
        {
            if (ply.GetBool("173_isvis"))
            {
                ply.SetMoveType(MOVETYPE_WALK);
                ply.SetBool("173_isvis", false);
            }
        }
    }
}

public bool RayFilter(int ent, int mask, any plyidx) 
{
    if (ent >= 1 && ent <= MaxClients) return false;
    return true;
}

public void KillInPVS(Player ply, int radius)
{
    ArrayList entArr = ents.FindInPVS(ply, radius);

    for(int i=0; i < entArr.Length; i++) {
        Player vic = entArr.Get(i);
        
        if (ply.id != vic.id && !vic.IsSCP && vic.IsAlive())
        {
            char sound[128];
            JSON_ARRAY nbs = gamemode.plconfig.GetObject("sound").GetArray("neckbroke");
            nbs.GetString(GetRandomInt(0, nbs.Length - 1), sound, sizeof(sound));
            gamemode.mngr.PlayAmbientOnPlayer(sound, vic);
            
            vic.Kill();
            
            break;
        }

        entArr.Erase(i);
        delete vic;
    }

    delete entArr;
}

public void SCP_OnInput(Player &ply, int buttons)
{
    if (ply.class.Is("173") && !ply.GetBool("173_isvis") && buttons & IN_ATTACK)  // 2^0 +attack
    {
        KillInPVS(ply, 130);
    }
}

public void AbilityCooldown(Player ply)
{
    ply.RemoveValue("173_abtmr");
    ply.SetBool("173_abready", true);
}

public void SCP_OnCallActionMenu(Player &ply)
{
    if (ply.GetBool("173_abready")) //ply.GetBool("173_abready")
    {
        Entity ent = view_as<Entity>(ply.GetHandle("173_holo"));

        ply.SetPos(ent.GetPos());
        
        KillInPVS(ply, 175);

        delete ent;

        ply.SetBool("173_isvis", false);
        ply.SetBool("173_abready", false);

        ply.SetMoveType(MOVETYPE_WALK);
    }
}

public void RenderHologram(Player ply)
{   
    if (ply.GetBool("173_isvis"))
    {
        if (!ply.GetHandle("173_holo"))
        {
            char model[256];
            ply.GetModel(model, sizeof(model));

            ply.SetHandle("173_holo", ents.Create("prop_dynamic")
            .SetModel(model)
            .Spawn()
            .SetHook(SDKHook_SetTransmit, TransmitHandler)
            .SetRenderMode(RENDER_TRANSCOLOR)
            .SetRenderColor(new Colour(255,255,255,75)));
        }
        else
        {
            Entity ent = view_as<Entity>(ply.GetHandle("173_holo"));
            Vector pos = ply.EyePos();
            Angle ang = ply.GetAng();
            
            float posarr[3], angarr[3], endposarr[3];
            pos.GetArrD(posarr);
            ang.GetArrD(angarr);

            Handle trace = TR_TraceRayFilterEx(posarr, angarr, MASK_SHOT, RayType_Infinite, TRFilter);

            if (TR_DidHit(trace))
            {
                TR_GetEndPosition(endposarr, trace);
                //float dist = (GetVectorDistance(posarr, endposarr) - 35.0);

                //endposarr[0] = (posarr[0] + (dist * Sine(DegToRad(angarr[1]))));
                //endposarr[1] = (posarr[1] + (dist * Cosine(DegToRad(angarr[1]))));
                //endposarr[2] += 90.0;
                //endposarr[2] += 16;
            }

            delete trace;

            ent.SetPos(new Vector(endposarr[0], endposarr[1], endposarr[2]), new Angle(0.0, angarr[1], 0.0)); // posarr[2] - 64.0

            ent.SetRenderMode(RENDER_TRANSCOLOR);

            delete ent;
        }
    }
    else
    {
        Entity ent = view_as<Entity>(ply.GetHandle("173_holo"));
        if (ent)
        {
            ent.SetRenderMode(RENDER_NONE);
            delete ent;
        }
    }
}

public bool TRFilter(int ent, int mask)
{
    return ent > MaxClients || !ent;
}

public Action TransmitHandler(int entity, int client)
{
    Player ply = player.GetByID(client);

    Handle hndl = ply.GetHandle("173_holo");

    delete ply;

    if (!hndl)
    {
        delete hndl;
        return Plugin_Handled;
    }

    return Plugin_Continue;
}