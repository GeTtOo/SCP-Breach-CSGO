#include <sourcemod>
#include <cstrike>
#include <sdkhooks>
#include <scpcore>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "SCP test plugin",
    author = "Andrey::Dono, GeTtOo",
    description = "SCP test for CS:GO",
    version = "0.1",
    url = ""
};

public void OnPluginStart() {
    PrintToServer("Test plugin loaded");
}

public void SCP_OnPlayerJoin(Client &ply) {
    //PrintToServer("Test plugin. Client connected. ID: %i", ply.id);
}

public void SCP_OnPlayerLeave(Client &ply) {
    //PrintToServer("Test plugin. Client disconnected. ID: %i", ply.id);
}

public void SCP_OnPlayerSpawn(Client &ply) {
    Client client = Clients.Get(ply.id);
    //PrintToChat(ply.id, "chance: %i", client.class.percent);
}

public void SCP_OnButtonPressed(Client ply, int doorId) {
    //PrintToChat(ply.id, "Client id: (%i) activated door (%i)", ply.id, doorId);
}