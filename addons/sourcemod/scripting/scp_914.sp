#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

#define MATH_COUNTER_VALUE_OFFSET 924

int MathCounter;

char modes[5][32] = {"rough", "coarse", "one_by_one", "fine", "very_fine"};
char curmode[32] = "rough";

public Plugin myinfo = {
    name = "SCP 914",
    author = "Andrey::Dono",
    description = "SCP-914 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = ""
};

public void OnPluginStart() {
    HookEvent("round_start", OnRoundStart);
}

public void OnPluginEnd() {
    gamemode.timer.PluginClear();
}

public void OnMapStart() {
    char mapName[128];
    GetCurrentMap(mapName, sizeof(mapName));

    gamemode.config.Add("914", ReadConfig(mapName, "914"));
}

public Action OnRoundStart(Event ev, const char[] name, bool dbroadcast) {
    int entId = 0;
    while ((entId = FindEntityByClassname(entId, "math_counter")) != -1) {
        char findedCounterName[32], configCounterName[32];
        GetEntPropString(entId, Prop_Data, "m_iName", findedCounterName, sizeof(findedCounterName));
        gamemode.config.GetObject("914").GetObject("config").GetString("countername", configCounterName, sizeof(configCounterName));
        if (StrEqual(findedCounterName, configCounterName))
            MathCounter = entId;
    }
}

public void SCP_OnPlayerSpawn(Client &ply) {
    //Client client = Clients.Get(ply.id);
}

public void SCP_OnButtonPressed(Client &ply, int doorId) {
    JSON_Object config = gamemode.config.GetObject("914").GetObject("config");
    
    if (doorId == config.GetInt("runbutton"))
        gamemode.timer.Simple(config.GetInt("runtime"), "Transform", ply);
    if (doorId == config.GetInt("switchbutton"))
        curmode = modes[RoundToZero(GetEntDataFloat(MathCounter, MATH_COUNTER_VALUE_OFFSET))];
}

public void Transform(Client ply) {
    JSON_Object recipes = gamemode.config.GetObject("914").GetObject("recipes").GetObject(curmode);

    char filter[3][32] = {"prop_physics", "weapon_", "player"};
    ArrayList ents = Ents.FindInBox(new Vector(3630.0, -2072.0, 20.0), new Vector(3762.0, -1947.0, 90.0), filter, sizeof(filter));

    if (gamemode.config.debug)
        PrintToChatAll("Ents count: %i", ents.Length);

    for(int i=0; i < ents.Length; i++) 
    {
        Entity ent = ents.Get(i);

        char entclass[32];
        ent.GetClass(entclass, sizeof(entclass));
        
        if (gamemode.config.debug)
            PrintToChat(ply.id, "class: %s, id: %i", entclass, ent.id);

        StringMapSnapshot srecipes = recipes.Snapshot();

        int keylen;
        for (int k = 0; k < srecipes.Length; k++)
        {
            keylen = srecipes.KeyBufferSize(k);
            char[] ientclass = new char[keylen];
            srecipes.GetKey(k, ientclass, keylen);
            if (json_is_meta_key(ientclass)) continue;

            if (StrEqual(entclass, ientclass)) {
                Vector oItemPos = ent.GetPos() - new Vector(0.0, 425.0, 0.0);

                JSON_Array recipe = view_as<JSON_Array>(recipes.GetObject(ientclass));
                
                PrintToServer("%i", recipe.GetKeyType(0));

                switch (recipe.GetKeyType(0)) {
                    case JSON_Type_String: {
                        char oentclass[32];
                        recipe.GetString(0, oentclass, sizeof(oentclass));

                        PrintToChat(ply.id, "%s", oentclass);
                        
                        Ents.Create(oentclass)
                        .SetPos(oItemPos)
                        .UseCB(view_as<SDKHookCB>(Callback_EntUse))
                        .Spawn();
                        
                        Ents.Remove(ent.id);
                    }
                    case JSON_Type_Object: {
                        for (int v=0; v < recipe.Length; v++) {
                            JSON_Array oentdata = view_as<JSON_Array>(recipe.GetObject(v));
                        }
                    }
                }
            }
        }
    }

    delete ents;
}

public SDKHookCB Callback_EntUse(int eid, int cid) {
    Client ply = Clients.Get(cid);
    Entity ent = Ents.Get(eid);

    char entClassName[32];
    ent.GetClass(entClassName, sizeof(entClassName));

    if (gamemode.entities.HasKey(entClassName))
        if (ply.inv.TryAdd(entClassName))
            Ents.Remove(ent.id);
        else
            PrintToChat(ply.id, " \x07[SCP] \x01Твой инвентарь переполнен");
}