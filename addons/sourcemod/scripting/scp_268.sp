#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "[SCP] 268",
    author = "Any",
    description = "SCP-268 for CS:GO modification - SCP Foundation",
    version = "1.0",
    url = "https://github.com/Eternity-Development-Team/csgo_scp"
};

public void SCP_RegisterMetaData() {
    gamemode.meta.RegEntEvent(ON_USE, "268_cap", "OnUse");
    gamemode.meta.RegEntEvent(ON_DROP, "268_cap", "OnDrop");
}

public void OnUse(Client &ply, InvItem &item) {
    if (!item.disabled) {
        SetEntityRenderMode(ply.id, RENDER_NONE);
        ply.progress.Start(15000, "InvisibleEffect");
        item.CooldownStart(gamemode.config.GetInt("scp-268_cd", 60) * 1000, "ItemUnlocked", item);
        item.disabled = true;
    }
}

public void OnDrop(Client &ply, InvItem &item) {
    SetEntityRenderMode(ply.id, RENDER_NORMAL);
}

public void ItemUnlocked(InvItem item) {
    item.disabled = false;
}

public void InvisibleEffect(Client ply) {
    SetEntityRenderMode(ply.id, RENDER_NORMAL);
}