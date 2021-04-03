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
	name = "[SCP] SCP 049",
	author = "GeTtOo",
	description = "SCP 049",
	version = "1.0",
	url = "https://github.com/GeTtOo/csgo_scp"
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

public void SCP_OnPlayerDeath(Client &vic, Client &atk)
{
	if(atk != null && atk.class != null)
	{
		char attackerClass[32];
		atk.class.Name(attackerClass, sizeof(attackerClass));

		if(StrEqual(attackerClass, "049"))
		{
			if(vic != null && vic.class != null && vic != atk)
			{
				float pos[3];
				GetClientAbsOrigin(vic.id, pos);

				vic.Team("SCP");
				vic.class = gamemode.team("SCP").class("049_2");

				gamemode.mngr.DeadPlayers--;
				gamemode.mngr.team("SCP").count++;
				
				vic.Spawn();
				TeleportEntity(vic.id, pos, NULL_VECTOR, NULL_VECTOR);
			}
		}
	}
}

public Action OnLookAtWeaponPressed(int client, const char[] command, int argc)
{
	if(IsClientExist(client))
	{
		if(!g_MusicPlay)
		{
			char class[32];
		
			Client ply = Clients.Get(client);
			ply.class.Name(class, sizeof(class));
			
			if(StrEqual(class, "049"))
			{
				g_MusicPlay = true;
				
				float pos[3];
				GetClientAbsOrigin(client, pos);

				int rnd = GetRandomInt(0, 9);
				EmitAmbientSound(sounds[rnd], pos, client);
				CreateTimer(15.0, AllowMusicPlay);
			}
		}
	}
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	char attackerClass[32];
	Client atk = Clients.Get(attacker);
	if (atk == null) return Plugin_Continue;
	atk.class.Name(attackerClass, sizeof(attackerClass)); 

	if(StrEqual(attackerClass, "049"))
	{
		damage += 180;
		return Plugin_Changed;
	}
	else if(StrEqual(attackerClass, "049_2"))
	{
		damage += 70;
		return Plugin_Changed;
	}

	return Plugin_Continue;
}

public Action AllowMusicPlay(Handle hTimer)
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