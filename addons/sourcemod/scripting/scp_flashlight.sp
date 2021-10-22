#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] Flashlight",
    author = "GeTtOo",
    description = "Flashlight for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void OnPluginStart() {
    PrecacheSound("items/flashlight1.wav");
}

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntEvent(ON_PICKUP, "flashlight", "OnPickup");
    gamemode.meta.RegEntEvent(ON_USE, "flashlight", "OnUse");
    gamemode.meta.RegEntEvent(ON_DROP, "flashlight", "OnDrop");
}

public void OnPickup(Client &ply, InvItem &item) {
    item.disabled = true;
}

public void OnUse(Client &ply, InvItem &item) {
    if(item.disabled == true)
        item.disabled = false;
    else
        item.disabled = true;

    ToogleFlashLight(ply);
}

public void OnDrop(Client &ply, InvItem &item) {
    if(item.disabled == false) {
        item.disabled = true;
        ToogleFlashLight(ply);
    }
}

void ToogleFlashLight(Client &ply) {
    SetEntProp(ply.id, Prop_Send, "m_fEffects", GetEntProp(ply.id, Prop_Send, "m_fEffects") ^ 4);
    EmitSoundToClient(ply.id, "items/flashlight1.wav");
}