#include <sdkhooks>
#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] Plugin example",
    author = "Anyone",
    description = "Plugin example for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/author/plugin"
};

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntEvent(ON_PICKUP, "ent_id", "Function name", true); // @arg1 Client, @arg2 Entity, @arg3 disable pickup to inventory (def false).
    gamemode.meta.RegEntEvent(ON_TOUCH, "ent_id", "Function name"); // @arg1 Entity, @arg2 Entity
    gamemode.meta.RegEntEvent(ON_USE, "ent_id", "Function name"); // @arg1 Client, @arg2 Entity
    gamemode.meta.RegEntEvent(ON_DROP, "ent_id", "Function name"); // @arg1 Client, @arg2 Entity
}

public void SCP_OnLoad() {
    
}

public void SCP_OnUnload() {
    
}

public void SCP_OnPlayerJoin(Client &ply) {
    
}

public void SCP_OnPlayerLeave(Client &ply) {
    
}

public void SCP_OnPlayerClear(Client &ply) {
    
}

public void SCP_OnPlayerSpawn(Client &ply) {
    
}

public Action SCP_OnTakeDamage(Client &vic, Client &atk, float &damage, int &damagetype) {
    
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