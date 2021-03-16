#include <sourcemod>
#include <cstrike>
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

public void OnPluginStart()
{
	HookEvent("player_death", Event_PlayerDeath, EventHookMode_Post);
}

public void OnMapStart()
{
    for(int y = 0; y < 10; y++)
    {
        FakePrecacheSound(sounds[y]);
    }
}

public Action Event_PlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
	int attacker = GetClientOfUserId(GetEventInt(event, "attacker"));

	if(IsClientExist(attacker))
	{
		char attackerClass[32];
		Client atk = Clients.Get(attacker);
		atk.class.Name(attackerClass, sizeof(attackerClass));

		if(StrEqual(attackerClass, "049"))
		{
			int victim = GetClientOfUserId(GetEventInt(event, "userid"));

			if(IsClientExist(victim) && !IsPlayerAlive(victim) && !IsClientInSpec(victim))
			{
				float pos[3];
				GetClientAbsOrigin(victim, pos);

				Client vic = Clients.Get(victim);
				//Тут меняем класс игроку и делаем туц туц туц 
				
				CS_RespawnPlayer(victim);
				TeleportEntity(victim, pos, NULL_VECTOR, NULL_VECTOR);
			}
		}
	}

	return Plugin_Continue;
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

public Action SCP_OnTakeDamage(Client vic, Client atk, int &inflictor, float &damage, int &damagetype)
{
	char attackerClass[32];
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