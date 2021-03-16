#include <sourcemod>
#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

// Время горения в конфиг

public Plugin myinfo = {
	name = "[SCP] SCP 457",
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
        SetEntityRenderMode(ply.id, RENDER_TRANSCOLOR);
        SetEntityRenderColor(ply.id, 255, 255, 255, 0);
        IgniteEntity(ply.id, 3600.0);
    }
}

public Action SCP_OnTakeDamage(Client vic, Client atk, int &inflictor, float &damage, int &damagetype)
{
    char victimClass[32], attackerClass[32];
    vic.class.Name(victimClass, sizeof(victimClass)); 
    
    if(StrEqual(victimClass, "SCP_457") && damagetype == DMG_BURN)
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

    return Plugin_Continue;
}
