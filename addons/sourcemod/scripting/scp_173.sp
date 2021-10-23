#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] 173",
    author = "Andrey::Dono",
    description = "SCP 173 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnPlayerSpawn(Client &ply) 
{
    if (ply.class != null && ply.class.Is("173")) {
        char  timername[128];
        Format(timername, sizeof(timername), "SCP-173-%i", ply.id);

        gamemode.timer.Create(timername, 250, 0, "CheckVisualContact", ply);
    }
}

public void SCP_OnPlayerClear(Client &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("173")) {
        char  timername[128];
        Format(timername, sizeof(timername), "SCP-173-%i", ply.id);

        gamemode.timer.Remove(timername);

        ply.RemoveValue("173_isvis");
    }
}

public void SCP_OnPlayerDeath(Client &ply)
{
    if (ply != null && ply.class != null && ply.class.Is("173")) {
        char sound[128];
        JSON_ARRAY ds = gamemode.plconfig.GetObject("sound").GetArray("death");
        ds.GetString(GetRandomInt(0, ds.Length - 1), sound, sizeof(sound));
        gamemode.mngr.PlayAmbient(sound, ply);
    }
}

public void CheckVisualContact(Client ply) 
{
    if (ply != null && ply.class != null && ply.IsAlive() && ply.class.Is("173")) 
    {
        bool visible = false;

        float scpPosArr[3];
        ply.EyePos().GetArrD(scpPosArr);

        char filter[1][32] = {"player"};
        ArrayList players = Ents.FindInBox(ply.GetPos() - new Vector(2000.0, 2000.0, 400.0), ply.GetPos() + new Vector(2000.0, 2000.0, 400.0), filter, sizeof(filter));

        for (int i=0; i < players.Length; i++) {
            Client checkply = players.Get(i);
            
            if (ply == checkply || checkply.IsSCP || !checkply.IsAlive()) continue;

            float checkPlyPosArr[3];
            checkply.EyePos().GetArrD(checkPlyPosArr);

            ArrayList checklist = Ents.FindInPVS(checkply, 2000);

            if (checklist.FindValue(ply) != -1)
            {
                Handle ray = TR_TraceRayFilterEx(checkPlyPosArr, scpPosArr, MASK_VISIBLE, RayType_EndPoint, RayFilter);
                if (!TR_DidHit(ray))
                {
                    visible = true;
                }

                delete ray;
            }

            delete checklist;
        }
        
        delete players;

        if (visible)
        {
            if (!ply.GetBool("173_isvis"))
            {
                ply.SetMoveType(MOVETYPE_NONE);
                ply.SetBool("173_isvis", true);
            }
        }
        else
        {
            if (ply.GetBool("173_isvis"))
            {
                ply.SetMoveType(MOVETYPE_WALK);
                ply.SetBool("173_isvis", false);
            }
        }
    }
}

public bool RayFilter(int ent, int mask, any plyidx) 
{
    if (ent >= 1 && ent <= MaxClients) return false;
    return true;
}

public void SCP_OnInput(Client &atk, int buttons)
{
    if (atk.class.Is("173") && !atk.GetBool("173_isvis") && buttons & IN_ATTACK)  // 2^0 +attack
    {
        ArrayList entArr = Ents.FindInPVS(atk, 130);

        for(int i=0; i < entArr.Length; i++) {
            Client vic = entArr.Get(i);
            
            if (atk.id != vic.id)
                vic.Kill();

            entArr.Erase(i);
            delete vic;
        }

        delete entArr;
    }
}