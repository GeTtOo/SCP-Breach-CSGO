#include <sourcemod>
#include <sdkhooks>
#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

// Время горения в конфиг

public Plugin myinfo = {
	name = "[Siberian SCP] SCP 457",
	author = "GeTtOo",
	description = "SCP 457",
	version = "1.0",
	url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnPlayerSpawn(Client ply)
{
    char class[32];
    ply.class.Name(class, sizeof(class));

    if(StrEqual(class, "SCP_457"))
    {
        SetEntityRenderMode(client, RENDER_TRANSCOLOR);
        SetEntityRenderColor(client, 255, 255, 255, 0);
        IgniteEntity(victim, 3600.0);
    }
}

public void SCP_OnTakeDamage(Client vic, Client atk, float damage)
{
    char victimClass[32], attackerClass[32];
    vic.class.Name(victimClass, sizeof(victimClass)); 
    
    if(StrEqual(class, "SCP_457") && damagetype == DMG_BURN))
    {
        return Plugin_Stop;
    }
    else if(atk != null)
    {
        atk.class.Name(attackerClass, sizeof(attackerClass));

        if(StrEqual(attackerClass, "SCP_457"))
        {
            IgniteEntity(vic.id, 20.0);
        }
    }
}
