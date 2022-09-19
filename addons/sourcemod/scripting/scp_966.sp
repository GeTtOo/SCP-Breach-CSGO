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
    name = "[SCP] 966",
    author = "Andrey::Dono",
    description = "Plugin adding SCP-966 for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnLoad()
{
    LoadTranslations("scpcore.phrases");
}

public void SCP_OnPlayerSpawn(Player &ply) {
    if (ply.class.Is("966"))
    {
        ply.SetHook(SDKHook_SetTransmit, TransmitHandler);
    
        char timername[64];
        Format(timername, sizeof(timername), "SCP-966-SoundEffect-%i", ply.id);
        timer.Create(timername, 5000, 0, "SoundEffect", ply);

        /*if (player.Alive() >= 15)
        {
            Player second = player.GetRandom();
            
            second.Team("SCP");
            second.class = gamemode.team("SCP").class("966");
            ply.SetupBaseStats();
        }*/
    }
}

public void SCP_PostPlayerSpawn(Player &ply) {
    if (ply.class.Is("966")) ply.SetProp("m_iHideHUD", 0);
}

public void SCP_OnPlayerClear(Player &ply) {
    if (ply.class && ply.class.Is("966"))
    {
        ply.SetProp("m_iHideHUD", 1<<12);
        ply.RemoveHook(SDKHook_SetTransmit, TransmitHandler);

        char timername[64];

        Format(timername, sizeof(timername), "SCP-966-SoundEffect-%i", ply.id);
        timer.RemoveByName(timername);

        if (ply.GetHandle("966_abtmr"))
        {
            timer.Remove(view_as<Tmr>(ply.GetHandle("966_abtmr")));
            ply.RemoveValue("966_abtmr");
        }
    }
}

public void SCP_OnPlayerSetupOverlay(Player &ply) {
    if (ply.class.Is("966"))
        ply.ShowOverlay("eternity/overlays/vignette_effect");
}

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype, int &inflictor) {
    if (atk == null || atk.class == null) return Plugin_Continue;

    if(atk.class.Is("966"))
    {
        damage = gamemode.plconfig.GetFloat("damage", 10.0);
        atk.PlayAmbient("*/eternity/scp/966/attack.mp3");
        
        return Plugin_Changed;
    }

    return Plugin_Continue;
}

public void SCP_OnCallAction(Player &ply)
{
    if (ply.class.Is("966"))
        if (!ply.GetHandle("966_abtmr"))
        {
            float abradius = float(gamemode.plconfig.GetInt("abradius", 250));
            
            char filter[1][32] = {"player"};
            ArrayList players = ents.FindInBox(ply.GetPos() - new Vector(abradius, abradius, 400.0), ply.GetPos() + new Vector(abradius, abradius, 400.0), filter, sizeof(filter));

            for (int i=0; i < players.Length; i++) {
                Player target = players.Get(i);
                
                if (ply == target || target.IsSCP || !target.IsAlive()) continue;

                target.speed /= gamemode.plconfig.GetFloat("abcsr", 2.0);
                
                target.TimerSimple(4000, "SetNormalSpeed", target);
            }

            delete players;

            ply.SetHandle("966_abtmr", ply.TimerSimple(gamemode.plconfig.GetInt("abcd", 15) * 1000, "AbilityUnlock", ply));
        }
        else
        {
            int cdsec = view_as<Tmr>(ply.GetHandle("966_abtmr")).GetTimeLeft();
            char str[128];
            FormatEx(str, sizeof(str), "%t", "Ability cooldown", cdsec);
            if (ply.lang == 22) Utils.AddRuTimeChar(str, sizeof(str), cdsec);
            ply.PrintWarning(str);
        }
}

public void SetNormalSpeed(Player ply)
{
    ply.speed = ply.class.speed;
}

public void AbilityUnlock(Player ply)
{
    ply.RemoveValue("966_abtmr");
    ply.progress.Stop(false);
}

public Action TransmitHandler(int entity, int client)
{
    Player ply = player.GetByID(client);

    if (entity == client || ply.IsSCP || !ply.IsAlive() || ply.inv.Have("visor_nv") && ply.inv.GetByClass("visor_nv").GetBool("active")) return Plugin_Continue;

    return Plugin_Handled;
}

public void SoundEffect(Player ply)
{
    char soundname[128];
    ArrayList players = ents.FindInPVS(ply, 500);

    if (players.Length == 0)
    {
        JSON_ARRAY sarr = gamemode.plconfig.Get("sound").GetArr("idle");
        sarr.GetString(GetRandomInt(0, sarr.Length - 1), soundname, sizeof(soundname));
    }
    else
    {
        JSON_ARRAY sarr = gamemode.plconfig.Get("sound").GetArr("angry");
        sarr.GetString(GetRandomInt(0, sarr.Length - 1), soundname, sizeof(soundname));
    }
    
    delete players;

    ply.PlayAmbient(soundname);
}