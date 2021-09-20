#include <sourcemod>
#include <cstrike>
#include <sdkhooks>
#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

bool g_MusicPlay = false;

char sounds[10][32] = {
    { "*/049_2.wav" },
    { "*/049_3.wav" }, 
    { "*/049_4.wav" },  
    { "*/049_5.wav" }, 
    { "*/049_6.wav" },
    { "*/049_7.wav" },
    { "*/049_8.wav" },
    { "*/049_alert_1.wav" },
    { "*/049_alert_2.wav" },
    { "*/049_alert_3.wav" }
};

public Plugin myinfo = {
	name = "[SCP] 049",
	author = "Andrey::Dono, GeTtOo",
	description = "SCP 049 for CS:GO SCP modification",
	version = "1.0",
	url = "https://github.com/Eternity-Development-Team/csgo_scp"
};

public void SCP_OnPlayerJoin(Client &ply) 
{
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void OnMapStart()
{
    for(int y = 0; y < 10; y++)
    {
        FakePrecacheSound(sounds[y]);
    }
}

public void SCP_OnInput(Client &ply, int buttons)
{
	if (buttons & IN_USE && ply.class.Is("049") && !ply.GetBool("049_reviving"))
	{
		if (buttons & IN_USE)
		{
			ply.SetBool("049_reviving", true);
			ply.progress.Start(3000, "Revive");
		}
	}

	if ((buttons & IN_FORWARD || buttons & IN_BACK || buttons & IN_MOVELEFT || buttons & IN_MOVERIGHT) && ply.class.Is("049") && ply.GetBool("049_reviving"))
	{
		ply.progress.Stop();
		ply.SetBool("049_reviving", false);
		//gamemode.timer.Simple(3000, "ReviveUnlock", ply);
	}
}

public void Revive(Client ply) {
	ply.progress.Stop();
	ply.SetBool("049_reviving", false);

	ArrayList players = Clients.GetAll();

	Client vic;
	for(int i=0; i < players.Length; i++)
	{
		vic = players.Get(i);

		if (ply == vic) continue;
		if (vic.IsAlive()) continue;

		if ((ply.GetPos() - new Vector(200.0, 200.0, 100.0)) < vic.deathpos && (ply.GetPos() + new Vector(200.0, 200.0, 100.0)) > vic.deathpos)
		{
			vic.Team("SCP");
			vic.class = gamemode.team("SCP").class("049_2");

			vic.Spawn();
			vic.SetPos(vic.deathpos, ply.GetAng() - new Angle(0.0, 180.0, 0.0));
		}
	}

	delete players;
}

public void ReviveUnlock(Client ply) {
	ply.SetBool("049_lock", false);
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	Client atk = Clients.Get(attacker);
	Client vic = Clients.Get(victim);
	if (atk == null || atk.class == null) return Plugin_Continue;

	if(atk.class.Is("049"))
	{
		damage += 180;
		return Plugin_Changed;
	}
	else if(atk.class.Is("049_2"))
	{
		damage += 70;
		return Plugin_Changed;
	}

	if (vic == null || vic.class == null) return Plugin_Continue;
	
	if(vic.class.Is("049") && vic.GetBool("049_reviving"))
	{
		vic.progress.Stop();
		vic.SetBool("049_reviving", false);
	}

	return Plugin_Continue;
}

public void SCP_OnPressF(Client &ply) {
    if (ply.class.Is("049") && !g_MusicPlay) {
		g_MusicPlay = true;
		
		float pos[3];
		GetClientAbsOrigin(ply.id, pos);

		int rnd = GetRandomInt(0, 9);
		EmitAmbientSound(sounds[rnd], pos, ply.id);
		gamemode.timer.Simple(15000, "AllowMusicPlay");
	}
}

public Action AllowMusicPlay()
{
	g_MusicPlay = false;
	return Plugin_Stop;
}

void FakePrecacheSound(const char[] szPath)
{
    AddToStringTable(FindStringTable( "soundprecache" ), szPath);
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