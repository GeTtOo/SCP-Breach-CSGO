#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] 500",
    author = "Andrey",
    description = "SCP-500 for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/Eternity-Development-Team/csgo_scp"
};

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntOnUse("500_panacea", "OnUse");
}

public void OnUse(Client &ply) {
    ply.health = ply.class.health;
    ply.inv.Remove("500_panacea");
}