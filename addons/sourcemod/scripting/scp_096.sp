#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] 096",
    author = "Andrey::Dono",
    description = "SCP-096 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/Eternity-Development-Team/csgo_scp"
};

public void SCP_OnPlayerJoin(Client &ply) 
{
    SDKHook(ply.id, SDKHook_OnTakeDamage, OnTakeDamage);
}

public void SCP_OnPlayerSpawn(Client &ply) {
    if (ply.class != null && ply.class.Is("096")) {

        ply.SetBool("IsRage", false);
        ply.SetBool("cooldown", false);

        gamemode.mngr.SetCollisionGroup(ply.id, 5);
        SDKHook(ply.id, SDKHook_StartTouch, CheckSurface);
        
        char  timername[128];
        Format(timername, sizeof(timername), "SCP-096-%i", ply.id);
        gamemode.timer.Create(timername, 250, 0, "CheckVision", ply);

        Format(timername, sizeof(timername), "SCP-096-S-%i", ply.id);
        gamemode.timer.Create(timername, 46000, 0, "Crying", ply);

        Crying(ply);
    }
}

public void SCP_OnPlayerClear(Client &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("096")) {
        
        ply.RemoveValue("IsRage");
        ply.RemoveValue("cooldown");
        
        char  timername[128];
        Format(timername, sizeof(timername), "SCP-096-%i", ply.id);
        gamemode.timer.Remove(timername);

        Format(timername, sizeof(timername), "SCP-096-S-%i", ply.id);
        gamemode.timer.Remove(timername);

        ply.StopSound("*/scp/scp-096_crying.mp3", 280);
    }
}

public Action OnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	Client atk = Clients.Get(attacker);
	if (atk == null || atk.class == null) return Plugin_Continue;

	if(atk.class.Is("096"))
	{
        if (atk.GetBool("IsRage"))
            damage += 800;
        else
            damage = 0.0;

        return Plugin_Changed;
	}

	return Plugin_Continue;
}

public void CheckSurface(int client, int entity) {
    Client ply = Clients.Get(client);

    if (ply.GetBool("IsRage")) {
        int pid = GetEntPropEnt(entity, Prop_Data, "m_hMoveParent");
        if (pid != -1) {
            char classname[32];
            GetEntityClassname(pid, classname, sizeof(classname));
            
            if (StrEqual(classname, "func_door"))
                RemoveEntity(pid);
            //ply.PlayAmbient("")
        }
    }
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
                            if (!ply.GetBool("cooldown") && !ply.GetBool("IsRage"))
                            {
                                ply.SetBool("IsRage", true);
                                ply.speed = 0.1;

                                char  timername[128];
                                Format(timername, sizeof(timername), "SCP-096-S-%i", ply.id);
                                gamemode.timer.Remove(timername);

                                ply.StopSound("*/scp/scp-096_crying.mp3", 280);
                                
                                ply.TimerSimple(5500, "Rage", ply);
                                gamemode.mngr.PlayAmbient("*/scp/scp-096_rage_start.mp3", ply);
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

    gamemode.mngr.PlayAmbient("*/scp/scp-096_rage.mp3", ply);

    ply.TimerSimple(10500, "Tranquility", ply);
}

public void Tranquility(Client ply) {
    if (ply != null && ply.class != null && ply.IsAlive()) {
        ply.speed = ply.class.speed;
        ply.multipler = ply.class.multipler;
        ply.SetBool("IsRage", false);
        ply.SetBool("cooldown", true);

        gamemode.mngr.PlayAmbient("*/scp/scp-096_tranquility.mp3", ply);

        ply.TimerSimple(25000, "CooldownReset", ply);
        ply.TimerSimple(5900, "StartCrying", ply);
    }
}

public void StartCrying(Client ply) {
    char  timername[128];
    Format(timername, sizeof(timername), "SCP-096-S-%i", ply.id);
    gamemode.timer.Create(timername, 46000, 0, "Crying", ply);

    Crying(ply);
}

public void Crying(Client ply) {
    ply.PlaySound("*/scp/scp-096_crying.mp3", 280);
}

public void CooldownReset(Client ply) {
    ply.SetBool("cooldown", false);
}

public void SCP_OnCallActionMenu(Client &ply) {
    if (ply.GetBool("IsRage")){
        ply.TimerSimple(500, "DisableAbility", ply);
        ply.multipler *= 2;
    }
}

public void DisableAbility(Client ply) {
    if (ply.GetBool("IsRage")){
        ply.multipler = 2.5;
    } else {
        ply.speed = ply.class.speed;
        ply.multipler = ply.class.multipler;
    }
}