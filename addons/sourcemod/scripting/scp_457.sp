#include <sourcemod>
#include <sdkhooks>
#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

// Время горения в конфиг
int particle = -1;

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
    HookEvent("round_end", OnRoundEnd);
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void SCP_OnPlayerSpawn(Client &ply)
{
    if(ply.class.Is("457"))
    {
        SetEntityRenderMode(ply.id, RENDER_NONE);
        IgniteEffect(ply.id);
    }
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
    Client vic = Clients.Get(victim), atk = Clients.Get(attacker);
    
    if (atk == null) return Plugin_Continue;
    
    if(vic.class.Is("457") && damagetype == DMG_BURN)
    {
        return Plugin_Handled;
    }
    else if(atk != null && atk.class.Is("457") && atk.id != vic.id)
    {
        IgniteEntity(vic.id, 20.0);
    }

    return Plugin_Continue;
}

public Action Event_PlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    DeleteEffect(GetClientOfUserId(GetEventInt(event, "userid")));
}

public void OnRoundEnd(Event event, const char[] name, bool dbroadcast) 
{
    DeleteEffect(GetClientOfUserId(GetEventInt(event, "userid")));
}

public void OnClientDisconnect(int client)
{
    DeleteEffect(client);
}

void IgniteEffect(int client)
{
    particle = CreateEntityByName("info_particle_system");

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

void DeleteEffect(int client)
{
    Client ply = Clients.Get(client);

    if (ply != null && ply.class != null && ply.class.Is("457"))
    {
        SetEntityRenderMode(ply.id, RENDER_NORMAL);

        if(particle != -1)
        {
            AcceptEntityInput(particle, "Kill");
            particle = -1;
        }

        int ent = GetEntPropEnt(ply.id, Prop_Send, "m_hRagdoll");
        if (ent > MaxClients && IsValidEdict(ent))
        {
            AcceptEntityInput(ent, "Kill");
        }
    }
}

stock bool IsClientExist(int client)
{
    if((0 < client < MaxClients) && IsClientInGame(client) && !IsClientSourceTV(client))
    {
        return true;
    }

    return false;
}