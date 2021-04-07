#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] SCP-914",
    author = "Andrey::Dono",
    description = "SCP 173 for CS:GO modification SCP Foundation",
    version = "0.1",
    url = ""
};

public void OnPluginStart() {
    PrintToServer("Plugin loaded");
    SetClientViewEntity(1, 1);
}

public void SCP_OnPlayerJoin(Client &ply) {
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void SCP_OnPlayerLeave(Client &ply) {
    
}

public void SCP_OnPlayerSpawn(Client &ply) {
    
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
    Client vic = Clients.Get(victim), atk = Clients.Get(attacker);
    
    if (atk == null) return Plugin_Continue;
    char victimClass[32], attackerClass[32];
    vic.class.Name(victimClass, sizeof(victimClass));
    
    if(atk != null)
    {
        atk.class.Name(attackerClass, sizeof(attackerClass));

        if(StrEqual(attackerClass, "173"))
        {
            vic.Kill();
        }
    }

    return Plugin_Continue;
}