#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] SCP-096",
    author = "Andrey::Dono",
    description = "SCP-096 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

bool IsRage = false;

public void SCP_OnPlayerJoin(Client &ply) 
{
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void SCP_OnPlayerSpawn(Client &ply) {
    if (ply.class != null && ply.class.Is("096")) {
        char  timername[128];
        Format(timername, sizeof(timername), "SCP-096-%i", ply.id);

        gamemode.timer.Create(timername, 250, 0, "CheckVision", ply);
    }
}

public void SCP_OnPlayerClear(Client &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("096")) {
        char  timername[128];
        Format(timername, sizeof(timername), "SCP-096-%i", ply.id);

        gamemode.timer.Remove(timername);
    }
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	Client atk = Clients.Get(attacker);
	if (atk == null) return Plugin_Continue;

	if(atk.class.Is("096"))
	{
        PrintToServer("Test");
        if (IsRage)
            damage += 800;
        else
            damage = 0.0;

        return Plugin_Changed;
	}

	return Plugin_Continue;
}

public void CheckVision(Client ply) {
    if (ply != null && ply.class != null && ply.IsAlive() && ply.class.Is("096"))
    {
        float scpPosArr[3];
        Vector pos = ply.EyePos();
        pos.GetArr(scpPosArr);
        delete pos;

        char filter[1][32] = {"player"};

        ArrayList players = Ents.FindInBox(ply.GetPos() - new Vector(2000.0, 2000.0, 400.0), ply.GetPos() + new Vector(2000.0, 2000.0, 400.0), filter, sizeof(filter));

        for (int i=0; i < players.Length; i++) {
            Client cply = players.Get(i);

            float playerPosArr[3];
            Vector cpos = cply.EyePos();
            cpos.GetArr(playerPosArr);
            delete cpos;

            if (cply.IsAlive()) {
                ArrayList checklist = Ents.FindInPVS(cply, 2000);

                for (int v=0; v < checklist.Length; v++)
                {
                    if (view_as<Client>(checklist.Get(v)).id == ply.id)
                    {
                        Handle ray = TR_TraceRayFilterEx(scpPosArr, playerPosArr, MASK_PLAYERSOLID_BRUSHONLY, RayType_EndPoint, RayFilter);
                        if (!TR_DidHit(ray))
                        {
                            if (!IsRage)
                            {
                                IsRage = true;
                                ply.speed = 0.0;
                                gamemode.timer.Simple(2000, "Rage", ply);
                            }
                        }

                        delete ray;
                    }
                }

                delete checklist;
            }
        }

        delete players;
    }
}

public bool RayFilter(int ent, int mask, any data) 
{
    if (ent >= 1 && ent <= MaxClients) return false;
    return true;
}

public void Rage(Client ply) {
    ply.speed = 260.0;
    ply.multipler = 2.5;

    gamemode.timer.Simple(15000, "Tranquility", ply);
}

public void Tranquility(Client ply) {
    if (ply != null && ply.class != null && ply.IsAlive()) {
        ply.speed = ply.class.speed;
        ply.multipler = ply.class.multipler;
        IsRage = false;
    }
}