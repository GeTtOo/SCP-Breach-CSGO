#include <sourcemod>
#include <sdkhooks>
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

public void SCP_OnPlayerJoin(Client &ply) {
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void SCP_OnPlayerSpawn(Client &ply)
{
    char class[32];
    ply.class.Name(class, sizeof(class));

    if(StrEqual(class, "457"))
    {
        SetEntityRenderMode(ply.id, RENDER_NONE);
        //SetEntityRenderColor(ply.id, 255, 255, 255, 0);
        IgniteEntity(ply.id, 3600.0);
    }
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
    Client vic = SCP_GetClient(victim), atk = SCP_GetClient(attacker);
    
    char victimClass[32], attackerClass[32];
    vic.class.Name(victimClass, sizeof(victimClass));
    
    if(StrEqual(victimClass, "457") && damagetype == DMG_BURN)
    {
        damage = 0.0;
        return Plugin_Handled;
    }
    else if(atk != null)
    {
        atk.class.Name(attackerClass, sizeof(attackerClass));

        if(StrEqual(attackerClass, "457"))
        {
            IgniteEntity(vic.id, 20.0);
        }
    }

    return Plugin_Handled;
}
