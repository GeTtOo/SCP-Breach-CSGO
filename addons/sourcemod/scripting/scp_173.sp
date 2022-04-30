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
    name = "[SCP] 173",
    author = "Andrey::Dono",
    description = "SCP 173 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnLoad()
{
    LoadTranslations("scpcore.phrases");
}

public void SCP_OnPlayerSpawn(Player &ply) 
{
    if (ply.class != null && ply.class.Is("173")) {
        char  timername[64];
        
        Format(timername, sizeof(timername), "SCP-173-VisChecker-%i", ply.id);
        gamemode.timer.Create(timername, 100, 0, "CheckVisualContact", ply);

        Format(timername, sizeof(timername), "SCP-173-Holo-%i", ply.id);
        gamemode.timer.Create(timername, 20, 0, "RenderHologram", ply);

        char model[256];
        ply.class.Model(model, sizeof(model));

        Entity hologramm = ents.Create("prop_dynamic");
        
        hologramm.SetPropEnt("m_hOwnerEntity", ply);
        hologramm.model.SetPath(model).SetRenderMode(RENDER_TRANSCOLOR).SetRenderColor(new Colour(255,255,255,75));
        hologramm.Spawn().SetHook(SDKHook_SetTransmit, TransmitHandler);

        ply.SetHandle("173_holo", hologramm);
    }
}

public void SCP_OnPlayerClear(Player &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("173")) {
        char  timername[64];
        Entity ent = view_as<Entity>(ply.GetHandle("173_holo"));
        
        if (ent) ents.Remove(ent);

        Format(timername, sizeof(timername), "SCP-173-VisChecker-%i", ply.id);
        gamemode.timer.RemoveByName(timername);

        Format(timername, sizeof(timername), "SCP-173-Holo-%i", ply.id);
        gamemode.timer.RemoveByName(timername);

        ply.RemoveValue("173_isvis");
        ply.RemoveValue("173_abtmr");
        ply.RemoveValue("173_abready");
        ply.RemoveValue("173_holo");
        ply.RemoveValue("173_renderholo");
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

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype)
{   
    if (vic.class.Is("173"))
    {
        //damage = damage / 50.0;
        if (vic.health >= gamemode.plconfig.GetInt("minhpscale", 4000))
            vic.multipler = float(vic.class.health) / (float(vic.health) / vic.class.multipler);
        //return Plugin_Changed;
    }

    return Plugin_Continue;
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
                ply.speed = 0.1;
                ply.SetBool("173_isvis", true);

                if (!ply.GetHandle("173_abtmr") && !ply.GetBool("173_abready"))
                {
                    ply.SetHandle("173_abtmr", ply.progress.Start(gamemode.plconfig.GetInt("abilitycd", 5) * 1000, "AbilityCooldown"));
                    ply.PrintNotify("Ability cooldown");
                }

            }
        }
        else
        {
            if (ply.GetBool("173_isvis"))
            {
                ply.speed = ply.class.speed;
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
            float scpPosArr[3];
            ply.EyePos().GetArrD(scpPosArr);

            float vicPlyPosArr[3];
            vic.EyePos().GetArrD(vicPlyPosArr);
            
            Handle ray = TR_TraceRayFilterEx(vicPlyPosArr, scpPosArr, MASK_VISIBLE, RayType_EndPoint, RayFilter);
            if (!TR_DidHit(ray))
            {
                char sound[128];
                JSON_ARRAY nbs = gamemode.plconfig.GetObject("sound").GetArray("neckbroke");
                nbs.GetString(GetRandomInt(0, nbs.Length - 1), sound, sizeof(sound));
                gamemode.mngr.PlayAmbientOnPlayer(sound, vic);

                vic.Kill();
            }

            delete ray;
            
            break;
        }
    }

    delete entArr;
}

public void SCP_OnInput(Player &ply, int buttons)
{
    if (ply.class.Is("173") && !ply.GetBool("173_isvis") && buttons & IN_ATTACK)  // 2^0 +attack
    {
        KillInPVS(ply, 90);
    }
}

public void AbilityCooldown(Player ply)
{
    ply.RemoveValue("173_abtmr");
    ply.SetBool("173_abready", true);
    ply.progress.Stop();
}

public void SCP_OnCallAction(Player &ply)
{
    if (ply.GetBool("173_isvis") && ply.GetBool("173_abready")) //ply.GetBool("173_abready")
    {
        Entity ent = view_as<Entity>(ply.GetHandle("173_holo"));

        ply.SetPos(ent.GetPos());
        
        KillInPVS(ply, 175);

        delete ent;

        ply.SetBool("173_isvis", false);
        ply.SetBool("173_abready", false);

        ply.speed = ply.class.speed;
    }
    else if (ply.GetBool("173_isvis") && !ply.GetBool("173_abready"))
        ply.PrintWarning("%t", "Ability cooldown");
}

public void RenderHologram(Player ply)
{
    Entity ent = view_as<Entity>(ply.GetHandle("173_holo"));

    if (ply.GetBool("173_isvis"))
    {
        if (ent)
        {
            Vector pos = ply.EyePos();
            Angle ang = ply.GetAng();
            
            float posarr[3], angarr[3], endposarr[3], backvecarr[3];
            pos.GetArrD(posarr);
            ang.GetArr(angarr);

            Handle trace = TR_TraceRayFilterEx(posarr, angarr, MASK_SHOT, RayType_Infinite, TRFilter);
            
            Vector back = ang.GetVectors(Forward).Normalize().Scale(10.0);
            back.GetArrD(backvecarr);

            bool correct;
            int loop = 100;
            float blinkrange = float(gamemode.plconfig.GetInt("blinkrange", 400));

            if (TR_DidHit(trace))
            {
                TR_GetEndPosition(endposarr, trace);
            }

            float blinkdistance = (ply.health >= gamemode.plconfig.GetInt("minhpscale", 4000)) ? float(ply.class.health) / (float(ply.health) / blinkrange) : float(ply.class.health) / (float(gamemode.plconfig.GetInt("minhpscale", 4000)) / blinkrange);

            if (GetVectorDistance(posarr, endposarr) >= blinkdistance)
            {
                Vector endpos = ply.GetAng().Forward(ply.EyePos(), blinkdistance);
                endpos.GetArrD(endposarr);
            }

            while (IsStuck(endposarr, ply.id) && !correct)
            {
                SubtractVectors(endposarr, backvecarr, endposarr);

                if (GetVectorDistance(endposarr, posarr) < 10 || loop-- < 1)
                {
                    correct = true;
                    endposarr = posarr;
                }
            }

            delete trace;
            
            if (GetVectorDistance(posarr, endposarr) >= 80)
            {
                ent.SetPos(new Vector(endposarr[0], endposarr[1], endposarr[2]), new Angle(0.0, angarr[1], 0.0)); // posarr[2] - 64.0
                if (!ply.GetBool("173_renderholo"))
                {
                    ply.SetBool("173_renderholo", true);
                    ent.model.SetRenderColor(new Colour(255,255,255,75));
                }
            }
        }
    }
    else
    {
        if (ent && ply.GetBool("173_renderholo"))
        {
            ply.SetBool("173_renderholo", false);
            ent.model.SetRenderColor(new Colour(255,255,255,0));
        }
    }

    delete ent;
}

public bool TRFilter(int ent, int mask)
{
    if (0 < ent <= MaxClients)
        return false;
    return true;
}

public bool IsStuck(float pos[3], int client)
{
    float mins[3], maxs[3];
    
    GetClientMins(client, mins);
    GetClientMaxs(client, maxs);

    for (int i=0; i < 3; ++i)
    {
        mins[i] -= 3;
        maxs[i] += 3;
    }
    
    TR_TraceHullFilter(pos, pos, mins, maxs, MASK_SOLID, TRFilter, client);
    
    return TR_DidHit();
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