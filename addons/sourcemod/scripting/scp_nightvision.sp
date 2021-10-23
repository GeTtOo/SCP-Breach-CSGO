#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] Nightvision googles",
    author = "Any",
    description = "Nightvision googles item for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void OnPluginStart() {
    PrintToServer("Plugin loaded");
}

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntEvent(ON_USE, "ent_id", "NightVisionEnable");
    gamemode.meta.RegEntEvent(ON_DROP, "ent_id", "NightVisionDrop");
}

public void NightVisionEnable(Client &ply) {
    if (!ply.GetBool("nightvision"))
    {
        ply.SetBool("nightvision", true);
        ply.SetProp("m_bNightVisionOn", 1);
    }
    else
    {
        ply.SetBool("nightvision", false);
        ply.SetProp("m_bNightVisionOn", 0);
    }
}

public void NightVisionEnable(Client &ply) {
    if (ply.GetBool("nightvision"))
    {
        ply.RemoveValue("nightvision", true);
        ply.SetProp("m_bNightVisionOn", 0);
    }
}