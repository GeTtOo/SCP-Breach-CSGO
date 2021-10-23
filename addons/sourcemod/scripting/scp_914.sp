#include <sdkhooks>
#include <scpcore>
#include <json>

#pragma semicolon 1
#pragma newdecls required

#define MATH_COUNTER_VALUE_OFFSET 924

JSON_OBJECT gconfig;

int Counter = 0;

char modes[5][32] = {"rough", "coarse", "one_by_one", "fine", "very_fine"};
char curmode[32] = "rough";

public Plugin myinfo = {
    name = "[SCP] 914",
    author = "Andrey::Dono",
    description = "SCP-914 for CS:GO modification SCP Foundation",
    version = "1.0",
    url = "https://github.com/GeTtOo/csgo_scp"
};

public void SCP_OnLoad() {
    LoadTranslations("scpcore.phrases");
    HookEvent("round_start", OnRoundStart);
}

public void SCP_OnUnload() {
    //gamemode.timer.PluginClear();
}

public void OnMapStart() {
    char mapName[128];
    GetCurrentMap(mapName, sizeof(mapName));

    gconfig = ReadConfig(mapName, "914");
}

public Action OnRoundStart(Event ev, const char[] name, bool dbroadcast) {
    if (gamemode.config.pl.GetBool("usemathcounter")) {
        int entId = 0;
        while ((entId = FindEntityByClassname(entId, "math_counter")) != -1) {
            char findedCounterName[32], configCounterName[32];
            GetEntPropString(entId, Prop_Data, "m_iName", findedCounterName, sizeof(findedCounterName));
            gamemode.config.pl.GetString("countername", configCounterName, sizeof(configCounterName));
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

public void SCP_OnButtonPressed(Client &ply, int doorId) {
    if (doorId == gamemode.config.pl.GetInt("runbutton"))
        gamemode.timer.Simple(gamemode.config.pl.GetInt("runtime") * 1000, "Transform", ply);
    
    if (doorId == gamemode.config.pl.GetInt("switchbutton"))
        if (gamemode.config.pl.GetBool("usemathcounter"))
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
    JSON_OBJECT recipes = gconfig.GetObject("recipes").GetObject(curmode);
    bool AmbientPlay = false;

    char filter[3][32] = {"prop_physics", "weapon_", "player"};
    
    ArrayList ents = Ents.FindInBox(gamemode.config.pl.GetArray("searchzone").GetVector(0), gamemode.config.pl.GetArray("searchzone").GetVector(1), filter, sizeof(filter));

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
                Vector oitempos = ent.GetPos() - gamemode.config.pl.GetVector("distance");

                JSON_OBJECT oentdata = recipes.GetObject(ientclass);
                JSON_ARRAY recipe;

                if (oentdata.IsArray)
                {
                    recipe = view_as<JSON_ARRAY>(oentdata).GetArray(GetRandomInt(0, view_as<JSON_ARRAY>(oentdata).Length - 1));
                }
                else
                {
                    StringMapSnapshot soentdata = oentdata.Snapshot();
                    int keylen2;
                    int random = GetRandomInt(1,100);
                    int count = 0;
                    for (int v=0; v < soentdata.Length; v++) {
                        keylen2 = soentdata.KeyBufferSize(v);
                        char[] chance = new char[keylen2];
                        soentdata.GetKey(v, chance, keylen2);
                        if (json_is_meta_key(chance)) continue;

                        count += StringToInt(chance);
                        if (count >= random) {
                            recipe = oentdata.GetArray(chance);
                            break;
                        }
                    }
                }

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
                            .UseCB(view_as<SDKHookCB>(CB_EntUse))
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
            Vector oitempos = ent.GetPos() - gamemode.config.pl.GetVector("distance");

            if (StrEqual(entclass, "player"))
            {
                ent.SetPos(oitempos);
            }
            else
            {
                Ents.Create(entclass).SetPos(oitempos, ent.GetAng()).UseCB(view_as<SDKHookCB>(CB_EntUse)).Spawn();
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
    
    ply.multipler *= 2.0;
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