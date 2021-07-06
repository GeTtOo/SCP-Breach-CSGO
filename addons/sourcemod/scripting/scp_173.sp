#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] SCP-173",
    author = "Andrey::Dono",
    description = "SCP 173 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnPlayerSpawn(Client &ply) {
    if (ply.class != null && ply.class.Is("173")) {
        char  timername[128];
        Format(timername, sizeof(timername), "SCP-173-%i", ply.id);

        gamemode.timer.Create(timername, 250, 0, "CheckVisualContact", ply);
    }
}

public void SCP_OnPlayerReset(Client &ply)
{
    DestroyVisualChecker(ply);
}

public void SCP_OnPlayerDeath(Client &ply)
{
    DestroyVisualChecker(ply);
}

public void SCP_OnPlayerLeave(Client &ply)
{
    DestroyVisualChecker(ply);
}

public void CheckVisualContact(Client ply) {
    if (ply != null && ply.class != null && ply.IsAlive() && ply.class.Is("173")) {
        bool isvis = false;
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
                            ply.multipler = 0.0;
                            isvis = true;
                        }

                        delete ray;
                    }
                }

                delete checklist;
            }
        }

        delete players;
        
        if(!isvis)
            if (ply.multipler == 0.0)
            ply.multipler = ply.class.multipler;
    }
}

public bool RayFilter(int ent, int mask, any data) {
    if (ent >= 1 && ent <= MaxClients) return false;
    return true;
}

public void DestroyVisualChecker(Client ply) {
    if (ply != null && ply.class != null && ply.class.Is("173")) {
        char  timername[128];
        Format(timername, sizeof(timername), "SCP-173-%i", ply.id);

        gamemode.timer.Remove(timername);
    }
}

public void SCP_OnInput(Client &atk, int buttons)
{
    if (atk.class.Is("173") && buttons & IN_ATTACK)  // 2^0 +attack
    {
        ArrayList entArr = Ents.FindInPVS(atk, 130);

        for(int i=0; i < entArr.Length; i++) {
            Client vic = entArr.Get(i);
            
            if (atk.id != vic.id)
                vic.Kill();
        }

        delete entArr;
    }
}