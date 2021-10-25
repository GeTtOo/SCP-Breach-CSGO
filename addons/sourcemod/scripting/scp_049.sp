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
#include <cstrike>
#include <sdkhooks>
#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

bool g_MusicPlay = false;

char sounds[10][32] = {
    { "*/scp/049/049_2.wav" },
    { "*/scp/049/049_3.wav" }, 
    { "*/scp/049/049_4.wav" },  
    { "*/scp/049/049_5.wav" }, 
    { "*/scp/049/049_6.wav" },
    { "*/scp/049/049_7.wav" },
    { "*/scp/049/049_8.wav" },
    { "*/scp/049/049_alert_1.wav" },
    { "*/scp/049/049_alert_2.wav" },
    { "*/scp/049/049_alert_3.wav" }
};

public Plugin myinfo = {
	name = "[SCP] 049",
	author = "Andrey::Dono, GeTtOo",
	description = "SCP 049 for CS:GO SCP modification",
	version = "1.0",
	url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnInput(Client &ply, int buttons)
{
	if (buttons & IN_USE && ply.class.Is("049") && !ply.GetBool("049_reviving"))
	{
		if (buttons & IN_USE)
		{
			char filter[1][32] = {"prop_ragdoll"};
			ArrayList ragdolls = Ents.FindInPVS(ply, _, _, filter);
			
			if (ragdolls.Length > 0)
			{
				ply.SetBool("049_reviving", true);
				ply.progress.Start((gamemode.plconfig.GetObject("revive").GetBool("multi", true)) ? gamemode.plconfig.GetObject("revive").GetInt("time", 3000) * 1000 * ragdolls.Length : gamemode.plconfig.GetObject("revive").GetInt("time", 3000) * 1000, "Revive");
				ply.PrintNotify("Reviving");
			}
		}
	}

	if ((buttons & IN_FORWARD || buttons & IN_BACK || buttons & IN_MOVELEFT || buttons & IN_MOVERIGHT) && ply.class.Is("049") && ply.GetBool("049_reviving"))
	{
		ply.progress.Stop();
		ply.SetBool("049_reviving", false);
		//gamemode.timer.Simple(3000, "ReviveUnlock", ply);
	}
}

public void Revive(Client ply) 
{
	ply.progress.Stop();
	ply.SetBool("049_reviving", false);

	if (gamemode.plconfig.GetObject("revive").GetBool("inpvs", true))
	{
		char filter[1][32] = {"prop_ragdoll"};
		ArrayList ragdolls = Ents.FindInPVS(ply, _, _, filter);

		if (ragdolls.Length > 0)
		{
			for (int i=0; i < ragdolls.Length; i++)
			{
				Entity vicrag = ragdolls.Get(i);

				ArrayList players = Clients.GetAll();

				Client vic;
				for(int k=0; k < players.Length; k++)
				{
					vic = players.Get(k);

					if (vic != ply && !vic.IsAlive() && vic.ragdoll)
					{
						if (vic.ragdoll.id == vicrag.id)
						{
							ArrayList bglist = vic.bglist;
							char modelname[256];
							
							vic.ragdoll.meta.model(modelname, sizeof(modelname));
							
							vic.Team("SCP");
							vic.class = gamemode.team("SCP").class("049_2");

							vic.Spawn();
							vic.SetModel(modelname);

							vic.bglist = bglist;
							//vic.SetBodyGroup("body", 0);
							vic.SetSkin(1);
							
							vic.SetPos(vic.ragdoll.GetPos(), ply.GetAng() - new Angle(0.0, 180.0, 0.0));
						}
					}
				}

				delete players;

				if (!gamemode.plconfig.GetObject("revive").GetBool("multi", true)) break;
			}
		}

		delete ragdolls;
	}
	else
	{
		ArrayList players = Clients.GetAll();

		Client vic;
		for(int i=0; i < players.Length; i++)
		{
			vic = players.Get(i);

			if (ply == vic) continue;
			if (vic.IsAlive()) continue;
			if (!vic.ragdoll) continue;

			if ((ply.GetPos() - new Vector(200.0, 200.0, 100.0)) < vic.ragdoll.GetPos() && (ply.GetPos() + new Vector(200.0, 200.0, 100.0)) > vic.ragdoll.GetPos())
			{
				vic.Team("SCP");
				vic.class = gamemode.team("SCP").class("049_2");

				vic.Spawn();
				vic.SetPos(vic.ragdoll.GetPos(), ply.GetAng() - new Angle(0.0, 180.0, 0.0));
			}
		}

		delete players;
	}
}

public void ReviveUnlock(Client ply) {
	ply.SetBool("049_lock", false);
}

public Action SCP_OnTakeDamage(Client &vic, Client &atk, float &damage, int &damagetype)
{
	if(atk.class.Is("049"))
	{
		damage += 250.0;
		return Plugin_Changed;
	}
	else if(atk.class.Is("049_2"))
	{
		damage += 70.0;
		return Plugin_Changed;
	}
	
	if(vic.class.Is("049") && vic.GetBool("049_reviving"))
	{
		vic.progress.Stop();
		vic.SetBool("049_reviving", false);
	}

	return Plugin_Continue;
}

public void SCP_OnCallActionMenu(Client &ply)
{
    if (ply.class.Is("049") && !g_MusicPlay)
	{
		g_MusicPlay = true;
		
		float pos[3];
		GetClientAbsOrigin(ply.id, pos);

		int rnd = GetRandomInt(0, 9);
		EmitAmbientSound(sounds[rnd], pos, ply.id);
		gamemode.timer.Simple(15000, "AllowMusicPlay");
	}
}

public void AllowMusicPlay()
{
	g_MusicPlay = false;
}

stock bool IsClientExist(int client)
{
    if((0 < client < MaxClients) && IsClientInGame(client) && !IsClientSourceTV(client))
    {
        return true;
    }

    return false;
}

stock bool IsClientInSpec(int client)
{
    if(GetClientTeam(client) != 1)
    {
        return false;
    }

    return true;
}