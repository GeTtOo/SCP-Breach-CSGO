#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] SCP-079",
    author = "Andrey::Dono",
    description = "Plugin example for CS:GO modification SCP Foundation",
    version = "0.1",
    url = ""
};

public void OnPluginStart() {
    PrintToServer("Plugin loaded");
}

public void SCP_OnPlayerJoin(Client &ply) {
    
}

public void SCP_OnPlayerLeave(Client &ply) {
    
}

public void SCP_OnPlayerSpawn(Client &ply) {
    if (ply.class == gamemode.team("SCP").class("079")) {
        SetEntityMoveType(ply.id, MOVETYPE_NONE);
        SetEntityRenderMode(ply.id, RENDER_NONE);
        SetEntProp(ply.id, Prop_Send, "m_iHideHUD", 1<<12|1<<4|1<<3);
        ply.SetPos(Ents.TryGetOrAdd(147).GetPos() - new Vector(0.0,0.0,75.0));
    }
}

public void SCP_OnButtonPressed(Client &ply, int doorId) {
    
}