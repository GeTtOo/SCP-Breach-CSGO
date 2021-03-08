#include <sourcemod>
#include <sdkhooks>

#pragma semicolon 1
#pragma newdecls required

// Время горения в конфиг

public Plugin myinfo = {
	name = "[Siberian SCP] SCP 914",
	author = "GeTtOo",
	description = "SCP 914",
	version = "1.0",
	url = "https://github.com/GeTtOo/csgo_scp"
};

public void OnPluginStart()
{
    HookEvent("player_hurt", Event_PlayerHurt, EventHookMode_Pre);
}

public void OnClientPostAdminCheck(int client)
{
    SDKHook(client, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void SCP_OnPlayerSpawn(int client)
{
    if(IsClientExist(client))
    {
        if(client == "SCP_457")
        {
            SetEntityRenderMode(client, RENDER_TRANSCOLOR);
            SetEntityRenderColor(client, 255, 255, 255, 0);
            IgniteEntity(victim, 3600.0);
        }
    }
}

public Action Event_PlayerHurt(Event event, const char[] name, bool dontBroadcast)
{
    int victim = GetClientOfUserId(GetEventInt(event, "userid"));
    int attacker = GetClientOfUserId(GetEventInt(event, "attacker"));

    if(IsClientExist(attacker))
    {
        if(attacker == "SCP_457")
        {
            if(IsClientExist(victim) && IsPlayerAlive(victim))
            {
                IgniteEntity(victim, 20.0);
            }
        }
    }
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
    if(IsClientExist(victim))
    {
        if(victim == "SCP_457" && damagetype == DMG_BURN)
        {
            return Plugin_Stop;
        }
    }
}

stock bool IsClientExist(int client)
{
    if((0 < client < MaxClients) && IsClientInGame(client) && !IsFakeClient(client) && !IsClientSourceTV(client))
    {
        return true;
    }

    return false;
}
