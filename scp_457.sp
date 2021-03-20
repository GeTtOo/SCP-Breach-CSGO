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

public void SCP_OnPlayerJoin(Client &ply) 
{
    HookEvent("player_death", Event_PlayerDeath);
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void SCP_OnPlayerSpawn(Client &ply)
{
    char class[32];
    ply.class.Name(class, sizeof(class));

    if(StrEqual(class, "457"))
    {
        SetEntityRenderMode(ply.id, RENDER_NONE);
        IgniteEffect(ply.id);
    }
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
    Client vic = SCP_GetClient(victim), atk = SCP_GetClient(attacker);
    
    char victimClass[32], attackerClass[32];
    vic.class.Name(victimClass, sizeof(victimClass));
    
    if(StrEqual(victimClass, "457") && damagetype == DMG_BURN)
    {
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

    return Plugin_Continue;
}

public Action Event_PlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(GetEventInt(event, "userid"));
    Client ply = Clients.Get(client);

    char class[32];
    ply.class.Name(class, sizeof(class));

    if(StrEqual(class, "457"))
    {
        SetEntityRenderMode(ply.id, RENDER_NORMAL);

        int ent = GetEntPropEnt(ply.id, Prop_Send, "m_hRagdoll");
        if (ent > MaxClients && IsValidEdict(ent))
        {
            AcceptEntityInput(ent, "Kill");
        }
    }
}

stock void IgniteEffect(int client)
{
    int particle = CreateEntityByName("info_particle_system");

    if(IsValidEdict(particle))
    {
        char name[64];
        float position[3];
        
        GetEntPropVector(client, Prop_Send, "m_vecOrigin", position);
        TeleportEntity(particle, position, NULL_VECTOR, NULL_VECTOR);
        
        GetEntPropString(client, Prop_Data, "m_iName", name, sizeof(name));

        DispatchKeyValue(particle, "targetname", "tf2particle");
        DispatchKeyValue(particle, "parentname", name);
        DispatchKeyValue(particle, "effect_name", "env_fire_large");
        DispatchSpawn(particle);
        
        SetVariantString("!activator");
        AcceptEntityInput(particle, "SetParent", client);
        ActivateEntity(particle);
        AcceptEntityInput(particle, "Start");
    }
}