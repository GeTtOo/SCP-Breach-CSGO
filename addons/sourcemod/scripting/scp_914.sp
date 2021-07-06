#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

#define MATH_COUNTER_VALUE_OFFSET 924

JSON_OBJECT config;

int Counter = 0;

char modes[5][32] = {"rough", "coarse", "one_by_one", "fine", "very_fine"};
char curmode[32] = "rough";

public Plugin myinfo = {
    name = "[SCP] SCP-914",
    author = "Andrey::Dono",
    description = "SCP-914 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
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
    config = gamemode.config.GetObject("914").GetObject("config");
}

public Action OnRoundStart(Event ev, const char[] name, bool dbroadcast) {
    if (config.GetBool("usemathcounter")) {
        int entId = 0;
        while ((entId = FindEntityByClassname(entId, "math_counter")) != -1) {
            char findedCounterName[32], configCounterName[32];
            GetEntPropString(entId, Prop_Data, "m_iName", findedCounterName, sizeof(findedCounterName));
            config.GetString("countername", configCounterName, sizeof(configCounterName));
            if (StrEqual(findedCounterName, configCounterName))
                Counter = entId;
        }
    }
    else
    {
        Counter = 0;
    }

    gamemode.timer.PluginClear();
}

public SDKHookCB Callback_EntUse(int eid, int cid) {
    Client ply = Clients.Get(cid);
    Entity ent = Ents.Get(eid);

    if (ply.IsSCP) return;

    char entClassName[32];
    ent.GetClass(entClassName, sizeof(entClassName));

    if (gamemode.entities.HasKey(entClassName))
        if (ply.inv.Add(entClassName))
            Ents.Remove(ent.id);
        else
            PrintToChat(ply.id, " \x07[SCP] \x01Твой инвентарь переполнен");
}

public void SCP_OnButtonPressed(Client &ply, int doorId) {
    if (doorId == config.GetInt("runbutton"))
        gamemode.timer.Simple(config.GetInt("runtime") * 1000, "Transform", ply);
    
    if (doorId == config.GetInt("switchbutton"))
        if (config.GetBool("usemathcounter"))
            curmode = modes[RoundToZero(GetEntDataFloat(Counter, MATH_COUNTER_VALUE_OFFSET))];
        else
            if (Counter < 4) {
                Counter++;
                curmode = modes[Counter];
            }
            else
            {
                Counter = 0;
                curmode = modes[Counter];
            }
}

public void Transform(Client ply) {
    JSON_OBJECT recipes = gamemode.config.GetObject("914").GetObject("recipes").GetObject(curmode);
    bool AmbientPlay = false;

    char filter[3][32] = {"prop_physics", "weapon_", "player"};
    
    ArrayList ents = Ents.FindInBox(config.GetArray("searchzone").GetVector(0), config.GetArray("searchzone").GetVector(1), filter, sizeof(filter));

    if (gamemode.config.debug)
        PrintToChatAll("Ents count: %i", ents.Length);

    for(int i=0; i < ents.Length; i++)
    {
        Entity ent = ents.Get(i);

        bool upgraded = false;

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
                Vector oitempos = ent.GetPos() - config.GetVector("distance");

                JSON_ARRAY oentdata = recipes.GetArray(ientclass);
                JSON_ARRAY recipe = oentdata.GetArray(GetRandomInt(0, oentdata.Length - 1));

                char oentclass[32];
                recipe.GetString(0, oentclass, sizeof(oentclass));
                
                if (StrEqual(entclass, "player"))
                {
                    Handle umsg = StartMessageOne("Fade", ent.id, USERMSG_RELIABLE);
                    PbSetInt(umsg, "duration", 800);
                    PbSetInt(umsg, "hold_time", 3000);
                    PbSetInt(umsg, "flags", 0x0001);
                    PbSetColor(umsg, "clr", {0,0,0,255});
                    EndMessage();

                    if (StrEqual(curmode, "rough") || StrEqual(curmode, "coarse")) {
                        if (!AmbientPlay) {
                            Vector emitpos = ent.GetPos();
                            float nativepos[3];
                            emitpos.GetArr(nativepos);
                            
                            EmitAmbientSound("*/scp/914_player_rough.mp3", nativepos);
                            AmbientPlay = true;
                        }
                        
                        EmitSoundToClient(ent.id, "*/scp/914_player_rough.mp3");
                    }

                    if (recipe.GetInt(1) >= GetRandomInt(1, 100)) {
                        char statusname[32];
                        recipe.GetString(0, statusname, sizeof(statusname));
                        
                        Call_StartFunction(null, GetFunctionByName(null, statusname));
                        Call_PushCell(ply);
                        Call_Finish();
                        
                        ent.SetPos(oitempos);
                    }
                    else
                    {
                        ent.SetPos(oitempos);
                    }
                }
                else
                {
                    if (recipe.GetInt(1) <= GetRandomInt(1, 100))
                    {
                        if (recipe.GetInt(2) >= GetRandomInt(1, 100))
                        {
                            Ents.Create(oentclass)
                            .SetPos(oitempos, ent.GetAng())
                            .UseCB(view_as<SDKHookCB>(Callback_EntUse))
                            .Spawn();

                            Ents.Remove(ent.id);
                        }
                        else
                        {
                            ent.SetPos(oitempos);
                        }
                    }
                    else
                    {
                        Ents.Remove(ent.id);
                    }
                }
                
                upgraded = true;
            }
        }

        if (!upgraded) {
            Vector oitempos = ent.GetPos() - config.GetVector("distance");

            if (StrEqual(entclass, "player"))
            {
                ent.SetPos(oitempos);
            }
            else
            {
                Ents.Create(entclass).SetPos(oitempos, ent.GetAng()).UseCB(view_as<SDKHookCB>(Callback_EntUse)).Spawn();
                Ents.Remove(ent.id);
            }
        }
    }

    delete ents;
}

public void Regeneration(Client ply) {
    PrintToChat(ply.id, " \x07[SCP] \x01 Вы ощущаете необычайный прилив сил");

    char  timername[128];
    Format(timername, sizeof(timername), "regeneration-%i", ply.id);
    
    gamemode.timer.Create(timername, 1000, 60, "Buff_Regeneration", ply);
}

public void Buff_Regeneration(Client ply) {
    if (ply.health < ply.class.health)
        if (ply.health + (ply.class.health * 5 /100) > ply.class.health)
            ply.health = ply.class.health;
        else
            ply.health += ply.class.health * 5 /100;
}

public void Speed(Client ply) {
    PrintToChat(ply.id, " \x07[SCP] \x01 Вы впадаете в ярость");
    
    ply.speed *= 2.0;
}

public void Injure(Client ply) {
    PrintToChat(ply.id, " \x07[SCP] \x01 Ваше тело начинает кровоточить из за множества мелких ран");

    char  timername[128];
    Format(timername, sizeof(timername), "injure-%i", ply.id);
    
    gamemode.timer.Create(timername, 2000, 30, "Debuff_Injure", ply);
}

public void Debuff_Injure(Client ply) {
    ply.health -= (ply.class.health * 3 / 100);
}

public void Butchering(Client ply) {
    PrintToChat(ply.id, " \x07[SCP] Ваше тело было разделано на компоненты.");
    ply.Kill();
}