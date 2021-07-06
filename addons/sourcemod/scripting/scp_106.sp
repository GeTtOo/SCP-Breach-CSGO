#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] SCP-106",
    author = "Andrey::Dono",
    description = "Plugin example for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void OnPluginStart() {
    PrintToServer("Plugin loaded");
}

public void SCP_OnPlayerJoin(Client &ply) {
    
}

public void SCP_OnPlayerLeave(Client &ply) {
    
}

public void SCP_OnPlayerSpawn(Client &ply) {
    
}

public void SCP_OnButtonPressed(Client &ply, int doorId) {
    
}