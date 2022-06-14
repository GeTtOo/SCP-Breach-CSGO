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
    name = "[SCP] Handcuffs",
    author = "Andrey::Dono",
    description = "Handcuffs for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnLoad()
{
    LoadTranslations("handcuff");
}

public void SCP_OnPlayerJoin(Player &ply)
{
    SDKHook(ply.id, SDKHook_WeaponCanUse, OnWeaponTake);
}

public void SCP_OnPlayerLeave(Player &ply)
{
    SDKUnhook(ply.id, SDKHook_WeaponCanUse, OnWeaponTake);
}

public Action OnWeaponTake(int client, int iWeapon)
{
    Player ply = player.GetByID(client);

    if(ply && ply.class && ply.GetBool("handcuffed")) return Plugin_Handled;

    return Plugin_Continue;
}

public void SCP_OnPlayerTakeWeapon(Player &ply, Entity &ent) {
    if (ent.IsClass("weapon_taser") && ent.GetBool("firsttake", true))
    {
        ent.SetProp("m_iClip1", gamemode.plconfig.GetInt("ammocount", 3));
        ent.SetBool("firsttake", false);
    }
}

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype, int &inflictor)
{
    if (atk == null || atk.class == null) return Plugin_Continue;

    char wepname[32], inflictorname[32];
    GetClientWeapon(atk.id, wepname, sizeof(wepname));
    GetEntityClassname(inflictor, inflictorname, sizeof(inflictorname));

    if (!vic.IsSCP && StrEqual(wepname, "weapon_taser") && StrEqual(inflictorname, "player"))
    {
        damage = 0.0;
        
        vic.SetBool("handcuffed", true);
        vic.DropWeapons();
        vic.inv.DropAll();

        vic.ShowOverlay("eternity/overlays/arrested_{lang}_fh");

        return Plugin_Changed;
    }

    return Plugin_Continue;
}

public EscapeInfo SCP_OnPlayerEscape(Player &ply, EscapeInfo &data)
{
    if (ply.GetBool("handcuffed"))
    {
        JSON_OBJECT arrestarr = gamemode.plconfig.GetObject("arrestedesc");
        StringMapSnapshot arrestarrsnap = arrestarr.Snapshot();
        
        char teamname[32], classname[32];
        for (int i=0; i < arrestarrsnap.Length; i++)
        {
            int kl = arrestarrsnap.KeyBufferSize(i);
            char[] class = new char[kl];
            arrestarrsnap.GetKey(i, class, kl);
            if (json_is_meta_key(class)) continue;
            
            ply.class.Name(classname, sizeof(classname));

            if (StrEqual(class, classname))
            {
                EscapeInfo arrestdata = view_as<EscapeInfo>(arrestarr.GetObject(class));
                arrestdata.team(teamname, sizeof(teamname));
                arrestdata.class(classname, sizeof(classname));
                
                data.trigger = arrestdata.trigger;
                data.team(teamname);
                data.class(classname);

                ply.RemoveValue("handcuffed");
                ply.RemoveValue("hc_breaking");
                ply.HideOverlay();
            }
        }
    }

    return data;
}

public void SCP_OnPlayerClear(Player &ply) {
    ply.RemoveValue("handcuffed");
    ply.RemoveValue("hc_breaking");
}

public void SCP_OnInput(Player &ply, int buttons) {
    if (buttons & IN_USE && !(buttons & IN_FORWARD || buttons & IN_BACK || buttons & IN_MOVELEFT || buttons & IN_MOVERIGHT) && ply.GetBool("handcuffed") && !ply.GetBool("hc_breaking") && !ply.GetBool("hc_bcd"))
	{
        ply.SetBool("hc_breaking", true);
        ply.progress.Start(gamemode.plconfig.GetInt("breaktime", 3) * 1000, "HcBreak");
        ply.PrintNotify("%t", "Handcuff breaking");

        char sound[128];
        JSON_ARRAY sndarr = gamemode.plconfig.GetObject("sound").GetArray("breaking");
        sndarr.GetString(GetRandomInt(0, sndarr.Length - 1), sound, sizeof(sound));
        ply.PlayAmbient(sound);

        ply.SetBool("hc_bcd", true);
        ply.TimerSimple(gamemode.plconfig.GetInt("breakingcd", 5) * 1000, "BCD", ply);
	}

    if ((buttons & IN_FORWARD || buttons & IN_BACK || buttons & IN_MOVELEFT || buttons & IN_MOVERIGHT) && ply.GetBool("handcuffed") && ply.GetBool("hc_breaking"))
    {
        ply.progress.Stop();
        ply.SetBool("hc_breaking", false);
    }
}

public void BCD(Player ply)
{
    ply.SetBool("hc_bcd", false);
}

public void HcBreak(Player ply) {
    ply.progress.Stop(false);
    ply.SetBool("hc_breaking", false);
    
    if (GetRandomInt(1, 100) <= gamemode.plconfig.GetInt("breakchance", 25))
    {
        ply.SetBool("handcuffed", false);
        ply.PrintNotify("%t", "Handcuff breaked");

        ply.HideOverlay();

        char sound[128];
        JSON_ARRAY sndarr = gamemode.plconfig.GetObject("sound").GetArray("breaked");
        sndarr.GetString(GetRandomInt(0, sndarr.Length - 1), sound, sizeof(sound));
        ply.PlayAmbient(sound);
    }
    else
    {
        ply.PrintNotify("%t", "Handcuff breaking failed");
    }
}