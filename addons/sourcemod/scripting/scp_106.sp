#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] 106",
    author = "Andrey::Dono",
    description = "SCP-106 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/Eternity-Development-Team/csgo_scp"
};

public void SCP_OnPlayerJoin(Client &ply) {

}

public void SCP_OnPlayerLeave(Client &ply) {
    
}

public void SCP_OnPlayerSpawn(Client &ply) {
    if (ply != null && ply.class != null && ply.class.Is("106"))
    {
        gamemode.mngr.SetCollisionGroup(ply.id, 5);
        SDKHook(ply.id, SDKHook_StartTouch, CheckSurface);
    }
}

public void SCP_OnButtonPressed(Client &ply, int doorId) {
    
}

public void CheckSurface(int client, int entity) {

    char className[32];
    GetEntityClassname(entity, className, sizeof(className));

    if (StrEqual(className, "func_door") || StrEqual(className, "func_door_rotating") || StrEqual(className, "prop_door_rotating")) {
        if (gamemode.config.debug)
            PrintToChat(client, "Ded touched door. (ID: %i)", entity);

        float plyvecarr[3];
        GetEntPropVector(client, Prop_Send, "m_vecOrigin", plyvecarr);
        Vector pv = new Vector(plyvecarr[0], plyvecarr[1], plyvecarr[2]);

        float doorvecarr[3];
        GetEntPropVector(entity, Prop_Send, "m_vecOrigin", doorvecarr);
        Vector dv = new Vector(doorvecarr[0], doorvecarr[1], doorvecarr[2]);

        Vector sv = pv - dv;

        PrintToChat(client, "x: %f, y: %f, z: %f", sv.x, sv.y, sv.z);

        gamemode.mngr.SetCollisionGroup(entity, 15);
    }
}