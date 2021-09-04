#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

// Урон в секунду в конфиг
// Подсчет игроков

public Plugin myinfo = {
    name = "[SCP] SCP-035",
    author = "GeTtOo",
    description = "Added SCP-035",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnPlayerJoin(Client &ply)
{
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void SCP_OnRoundStart()
{
    Ents.Create("035_mask")
    .SetPos(new Vector(-7682.0, 464.0, 118.0), new Angle(0.0, 0.0, 0.0))
    .UseCB(view_as<SDKHookCB>(Callback_EntUse))
    .Spawn();
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

    if (atk == null) return Plugin_Continue;

    if(atk.class.Is("035"))
    {
        damage += 180;
        return Plugin_Changed;
    }

    return Plugin_Continue;
}

public SDKHookCB Callback_EntUse(int eid, int cid) 
{
    Client ply = Clients.Get(cid);

    if (ply.IsSCP && ply != null && ply.class != null) return;

    Ents.Remove(eid);

    Vector sp = ply.GetPos();
    Angle sa = ply.GetAng();

    ply.Kill();

    ply.Team("SCP");
    ply.class = gamemode.team("SCP").class("035");
    
    ply.Spawn();

    ply.SetPos(sp, sa);
    
    gamemode.timer.Create("Timer_SCP-035_Hit", 2500, 0, "HandlerHitSCP", ply);
}

public Action HandlerHitSCP(Client ply)
{
    if(ply.health > 10)
        ply.health -= 10;
    else
        ForcePlayerSuicide(ply.id);
}