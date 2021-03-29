#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = {
    name = "SCP 914",
    author = "Andrey::Dono",
    description = "SCP-914 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = ""
};

public void OnPluginStart() {

}

public void OnMapStart() 
{
    char mapName[128];
    GetCurrentMap(mapName, sizeof(mapName));

    gamemode.config.Add("recipes", ReadConfig(mapName, "914"));
}

public void SCP_OnPlayerSpawn(Client &ply) {
    //Client client = Clients.Get(ply.id);
}

public void SCP_OnButtonPressed(Client &ply, int doorId) {
    if (doorId == gamemode.config.GetObject("recipes").GetObject("config").GetInt("button")) {
        gamemode.timers.Add("Transform", 10, false, ply);
    }
}

public void Transform(Client ply) {
    char filter[3][32];
    filter[0] = "prop_physic_override";
    filter[1] = "weapon_";
    filter[2] = "player";

    ArrayList ents = Ents.FindInBox(new Vector(3630.0, -2072.0, 30.0), new Vector(3762.0, -1947.0, 90.0), filter, sizeof(filter));

    PrintToChatAll("Ents count: %i", ents.Length);

    for(int i=0; i < ents.Length; i++) 
    {
        Entity ent = ents.Get(i);

        char entclass[32];
        ent.GetClass(entclass, sizeof(entclass));
        
        PrintToChat(ply.id, "class: %s, id: %i", entclass, ent.id);

        Vector itmvec = ent.GetPos();

        ent.SetPos(new Vector(itmvec.x,itmvec.y - 425.0,itmvec.z));
    }

    delete ents;
}