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

char sounds[10][64] = {
    { "*/eternity/scp/049/1.wav" },
    { "*/eternity/scp/049/2.wav" }, 
    { "*/eternity/scp/049/3.wav" },  
    { "*/eternity/scp/049/4.wav" }, 
    { "*/eternity/scp/049/5.wav" },
    { "*/eternity/scp/049/6.wav" },
    { "*/eternity/scp/049/7.wav" },
    { "*/eternity/scp/049/alert_1.wav" },
    { "*/eternity/scp/049/alert_2.wav" },
    { "*/eternity/scp/049/alert_3.wav" }
};

int soundstime[10] = {12,2,2,2,3,4,5,1,1,4};

public Plugin myinfo = {
	name = "[SCP] 049",
	author = "Andrey::Dono, GeTtOo",
	description = "SCP 049 for CS:GO SCP modification",
	version = "1.0",
	url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnInput(Player &ply, int buttons)
{
	if (ply.class.Is("049"))
	{
		if (buttons & IN_USE && !ply.GetBool("049_reviving"))
		{
			if (buttons & IN_USE)
			{
				char filter[1][32] = {"prop_ragdoll"};
				ArrayList ragdolls = ents.FindInPVS(ply, _, _, filter);
				
				if (ragdolls.Length > 0)
				{
					ply.SetBool("049_reviving", true);
					ply.progress.Start((gamemode.plconfig.GetObject("revive").GetBool("multi", true)) ? gamemode.plconfig.GetObject("revive").GetInt("time", 3000) * 1000 * ragdolls.Length : gamemode.plconfig.GetObject("revive").GetInt("time", 3000) * 1000, "Revive");
					ply.PrintNotify("Reviving");
				}

				delete ragdolls;
			}
		}

		if ((buttons & IN_FORWARD || buttons & IN_BACK || buttons & IN_MOVELEFT || buttons & IN_MOVERIGHT) && ply.GetBool("049_reviving"))
		{
			ply.progress.Stop();
			ply.SetBool("049_reviving", false);
			//timer.Simple(3000, "ReviveUnlock", ply);
		}
	}
}

public void SCP_OnPlayerClear(Player &ply)
{
	if (ply.class && ply.class.Is("049")) {
		ply.RemoveValue("049_reviving");
		ply.RemoveValue("049_lock");
		ply.RemoveValue("049_saying");
	}
}

public void Revive(Player ply)
{
	ply.progress.Stop(false);
	ply.SetBool("049_reviving", false);

	if (gamemode.plconfig.GetObject("revive").GetBool("inpvs", true))
	{
		char filter[1][32] = {"prop_ragdoll"};
		ArrayList ragdolls = ents.FindInPVS(ply, _, _, filter);

		if (ragdolls.Length > 0)
		{
			for (int i=0; i < ragdolls.Length; i++)
			{
				Entity vicrag = ragdolls.Get(i);

				ArrayList players = player.GetAll();

				Player vic;
				for(int k=0; k < players.Length; k++)
				{
					vic = players.Get(k);

					if (vic != ply && !vic.IsAlive() && vic.ragdoll)
					{
						if (vic.ragdoll == vicrag && !vic.ragdoll.GetBool("IsSCP"))
						{
							ArrayList bglist = vic.model.bglist;
							char modelname[256];
							
							vic.ragdoll.meta.model(modelname, sizeof(modelname));
							
							vic.Team("SCP");
							vic.class = gamemode.team("SCP").class("049_2");

							vic.Spawn();
							vic.model.SetPath(modelname);

							vic.model.bglist = bglist;
							//vic.SetBodyGroup("body", 0);
							vic.model.SetSkin(vic.model.GetSkin() + 1);
							
							vic.SetHandle("049_2_vec", vic.ragdoll.GetPos());
							vic.SetHandle("049_2_ang", ply.GetAng() - new Angle(0.0, 180.0, 0.0));

							ply.health += gamemode.plconfig.GetInt("healing", 2500);
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
		ArrayList players = player.GetAll();

		Player vic;
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

public void SCP_PostPlayerSpawn(Player &ply)
{
	if (ply.class.Is("049_2") && ply.GetHandle("049_2_vec") && ply.GetHandle("049_2_ang"))
	{
		ply.SetPos(view_as<Vector>(ply.GetHandle("049_2_vec")), view_as<Angle>(ply.GetHandle("049_2_ang")));
		ply.RemoveValue("049_2_vec");
		ply.RemoveValue("049_2_ang");
	}
}

public void ReviveUnlock(Player ply) {
	ply.SetBool("049_lock", false);
}

public Action SCP_OnTakeDamage(Player &vic, Player &atk, float &damage, int &damagetype)
{
	if (atk == null || atk.class == null) return Plugin_Continue;
	
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

public void SCP_OnCallAction(Player &ply)
{
    if (ply.class.Is("049") && !ply.GetBool("049_saying"))
	{
		ply.SetBool("049_saying", true);
		
		float pos[3];
		GetClientAbsOrigin(ply.id, pos);

		int rnd = GetRandomInt(0, 9);
		ply.PlayAmbient(sounds[rnd]);
		ply.TimerSimple(soundstime[rnd] * 1000, "AllowMusicPlay", ply);
	}
}

public void AllowMusicPlay(Player ply)
{
	ply.SetBool("049_saying", false);
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