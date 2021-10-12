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
    gamemode.meta.RegEntEvent(ON_USE, "ent_id", "Function name");
    gamemode.meta.RegEntEvent(ON_PICKUP, "ent_id", "Function name", true); // true disable pick up to inventory (def false).
    gamemode.meta.RegEntEvent(ON_DROP, "ent_id", "Function name");
}

public void SCP_OnPlayerJoin(Client &ply) {
    
}

public void SCP_OnPlayerLeave(Client &ply) {
    
}

public void SCP_OnPlayerClear(Client &ply) {
    
}

public void SCP_OnPlayerSpawn(Client &ply) {
    
}

public Action SCP_OnTakeDamage(Client vic, Client atk, float &damage, int &damagetype) {
    
}

public void SCP_OnPlayerDeath(Client &vic, Client &atk) {

}

public void SCP_OnPlayerReset(Client &ply) {

}

public void SCP_OnRoundStart() {

}

public void SCP_OnRoundEnd() {

}

public void SCP_OnInput(Client &ply, int buttons) {

}

public void SCP_OnButtonPressed(Client &ply, int doorId) {
    
}

public void SCP_OnCallActionMenu(Client &ply) {
    
}