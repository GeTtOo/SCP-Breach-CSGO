#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

// Урон в секунду в конфиг
// Подсчет игроков

public Plugin myinfo = {
    name = "[SCP] 035",
    author = "GeTtOo",
    description = "Added SCP-035",
    version = "1.0",
    url = "https://github.com/Eternity-Development-Team/csgo_scp"
};

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntEvent(ON_PICKUP, "035_mask", "Logic", true);
}

public void SCP_OnPlayerJoin(Client &ply)
{
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void SCP_OnPlayerClear(Client &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("035"))
    {
        gamemode.timer.Remove("Timer_SCP-035_Hit");
    }
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
    Client atk = Clients.Get(attacker);

    if (atk == null || atk.class == null) return Plugin_Continue;

    if(atk.class.Is("035"))
    {
        damage += 180;
        return Plugin_Changed;
    }

    return Plugin_Continue;
}

public Action HandlerHitSCP(Client ply)
{
    if(ply.health > 10)
        ply.health -= 10;
    else
        ply.Kill();
}

public void Logic(Client &ply)
{
    Vector sp = ply.GetPos();
    Angle sa = ply.GetAng();

    ply.Kill();

    ply.Team("SCP");
    ply.class = gamemode.team("SCP").class("035");
    
    ply.Spawn();

    ply.SetPos(sp, sa);
    
    gamemode.timer.Create("Timer_SCP-035_Hit", 2500, 0, "HandlerHitSCP", ply);
}