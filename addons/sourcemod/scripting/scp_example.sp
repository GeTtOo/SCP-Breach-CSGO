#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] Plugin example",
    author = "Any",
    description = "Plugin example for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void OnPluginStart() {
    PrintToServer("Plugin loaded");
}

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntOnUse("ent_id", "Function name");
    gamemode.meta.RegEntOnPickup("ent_id", "Function name", true); // true disable pick up to inventory (def false).
}

public void SCP_OnPlayerJoin(Client &ply) {
    
}

public void SCP_OnPlayerLeave(Client &ply) {
    
}

public void SCP_OnPlayerSpawn(Client &ply) {
    
}

public void SCP_OnButtonPressed(Client &ply, int doorId) {
    
}