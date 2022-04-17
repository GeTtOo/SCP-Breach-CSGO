/**
 * =============================================================================
 * Copyright (C) 2021 Eternity team (Andrey::Dono, GeTtOo).
 * =============================================================================
 *
 * This file is part of the SCP Breach CS:GO.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 **/

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

public void SCP_RegisterMetaData() {
    gamemode.meta.RegisterStatusEffect("Butchering");
    gamemode.meta.RegStatusEffectEvent(INIT, "Butchering", "Butchering");
    
    gamemode.meta.RegisterStatusEffect("Injure", 0.5);
    gamemode.meta.RegStatusEffectEvent(INIT, "Injure", "Injure_Init");
    gamemode.meta.RegStatusEffectEvent(UPDATE, "Injure", "Injure_Update");
    gamemode.meta.RegStatusEffectEvent(END, "Injure", "Injure_End");
    
    gamemode.meta.RegisterStatusEffect("Metamarphose", 2.0);
    gamemode.meta.RegStatusEffectEvent(INIT, "Metamarphose", "Metamarphose_Init");
    gamemode.meta.RegStatusEffectEvent(UPDATE, "Metamarphose", "Metamarphose_Update");
    gamemode.meta.RegStatusEffectEvent(END, "Metamarphose", "Metamarphose_End");

    gamemode.meta.RegisterStatusEffect("Heal", 1.5);
    gamemode.meta.RegStatusEffectEvent(INIT, "Heal", "Heal_Init");
    gamemode.meta.RegStatusEffectEvent(UPDATE, "Heal", "Heal_Update");
}

public void SCP_OnLoad() {
    LoadTranslations("scpcore.phrases");

    char mapName[128];
    GetCurrentMap(mapName, sizeof(mapName));

    gconfig = ReadConfig(mapName, "914");
    
    if (gconfig) gamemode.log.Debug("Recipes loaded");
}

public void SCP_OnUnload() {
    gconfig.Dispose();
    gamemode.log.Debug("[Memory status] SCP-914 Recipes unload.");
    //gamemode.timer.PluginClear();
}

public void SCP_OnRoundStart() {
    if (gamemode.plconfig.GetBool("usemathcounter")) {
        int entId = 0;
        while ((entId = FindEntityByClassname(entId, "math_counter")) != -1) {
            char findedCounterName[32], configCounterName[32];
            GetEntPropString(entId, Prop_Data, "m_iName", findedCounterName, sizeof(findedCounterName));
            gamemode.plconfig.GetString("countername", configCounterName, sizeof(configCounterName));
            if (StrEqual(findedCounterName, configCounterName))
                Counter = entId;
        }

        gamemode.log.Debug("Using math counter");
    }
    else
    {
        Counter = 0;

        gamemode.log.Debug("Using simple counter");
    }

    gamemode.timer.PluginClear();
}

public void SCP_OnButtonPressed(Player &ply, int doorId) {
    if (doorId == gamemode.plconfig.GetInt("runbutton"))
        gamemode.timer.Simple(gamemode.plconfig.GetInt("runtime") * 1000, "Transform", ply);
    
    if (doorId == gamemode.plconfig.GetInt("switchbutton"))
        if (gamemode.plconfig.GetBool("usemathcounter"))
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

public void Transform(Player ply) {
    JSON_OBJECT recipes = gconfig.GetObject("recipes").GetObject(curmode);

    char filter[3][32] = {"prop_physics", "weapon_", "player"};
    
    ArrayList entities = ents.FindInBox(gamemode.plconfig.GetArray("searchzone").GetVector(0), gamemode.plconfig.GetArray("searchzone").GetVector(1), filter, sizeof(filter));

    char entlist[3072];

    for(int i=0; i < entities.Length; i++)
    {
        Entity ent = entities.Get(i);

        bool upgraded = false;

        char entclass[32];
        ent.GetClass(entclass, sizeof(entclass));

        Format(entlist, sizeof(entlist), "%s\nClass: %s (id: %i)", entlist, entclass, ent.id);

        StringMapSnapshot srecipes = recipes.Snapshot();

        int keylen;
        for (int k = 0; k < srecipes.Length; k++)
        {
            keylen = srecipes.KeyBufferSize(k);
            char[] ientclass = new char[keylen];
            srecipes.GetKey(k, ientclass, keylen);
            if (json_is_meta_key(ientclass)) continue;

            if (StrEqual(entclass, ientclass)) {
                Vector oitempos = ent.GetPos() - gamemode.plconfig.GetVector("distance");

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

                    delete soentdata;
                }
                
                if (ent.IsClass("player"))
                {
                    Player entply = view_as<Player>(ent);
                    
                    gamemode.mngr.Fade(entply.id, 800, 3000, new Colour(0,0,0,255));

                    if (recipe.GetInt(2) >= GetRandomInt(1, 100)) {
                        char statusname[32];
                        recipe.GetString(0, statusname, sizeof(statusname));
                        
                        entply.se.Create(statusname, recipe.GetInt(1));
                        
                        entply.SetPos(oitempos);
                    }
                    else
                    {
                        entply.SetPos(oitempos);
                    }
                }
                else
                {
                    char oentclass[32];
                    recipe.GetString(0, oentclass, sizeof(oentclass));

                    if (recipe.GetInt(1) <= GetRandomInt(1, 100))
                    {
                        if (recipe.GetInt(2) >= GetRandomInt(1, 100))
                        {
                            ents.Create(oentclass)
                            .SetPos(oitempos, ent.GetAng())
                            .Spawn();

                            ents.Remove(ent);
                        }
                        else
                        {
                            ent.SetPos(oitempos);
                        }
                    }
                    else
                    {
                        ents.Remove(ent);
                    }
                }
                
                upgraded = true;
            }
        }

        delete srecipes;

        if (!upgraded) {
            Vector oitempos = ent.GetPos() - gamemode.plconfig.GetVector("distance");

            if (StrEqual(entclass, "player"))
            {
                ent.SetPos(oitempos);
            }
            else
            {
                ents.Create(entclass).SetPos(oitempos, ent.GetAng()).Spawn();
                ents.Remove(ent);
            }
        }
    }

    gamemode.log.Debug("Transforming iteration started by player %i (Mode: %s).\nFinded %i entities:%s", ply.id, curmode, entities.Length, entlist);

    delete entities;
}

public void Regeneration(Player ply) {
    PrintToChat(ply.id, " \x07[SCP] \x01 Вы ощущаете необычайный прилив сил");

    char  timername[128];
    Format(timername, sizeof(timername), "regeneration-%i", ply.id);
    
    gamemode.timer.Create(timername, 1000, 60, "Buff_Regeneration", ply);
}

public void Buff_Regeneration(Player ply) {
    if (ply.health < ply.class.health)
        if (ply.health + (ply.class.health * 5 /100) > ply.class.health)
            ply.health = ply.class.health;
        else
            ply.health += ply.class.health * 5 /100;
}

public void Speed(Player ply) {
    PrintToChat(ply.id, " \x07[SCP] \x01 Вы впадаете в ярость");
    
    ply.multipler *= 2.0;
}

//////////////////////////////////////////////////////////////////////////////
//
//                           Metamarphose status effect
//
//////////////////////////////////////////////////////////////////////////////

public void Metamarphose_Init(Player ply) {
    ply.multipler = 2.5;
    ply.SetArrayList("dooraccess", gamemode.meta.GetEntity("005_picklock").GetArrayList("access"));
    ply.PrintWarning("Они все недостойны... Убить их всех...");
}

public void Metamarphose_Update(Player ply) {
    int max_health = 2500;
    if (ply.health < max_health)
        if (ply.health + (max_health * 5 / 100) > max_health)
            ply.health = max_health;
        else
            ply.health += max_health * 5 / 100;
}

public void Metamarphose_End(Player ply) {
    ply.RemoveValue("dooraccess");
    ply.Kill();
    ply.PrintWarning("Вы перешли на другой уровень бытия...");
}

public void Metamarphose_End_Force(Player ply) {
    ply.multipler = ply.class.multipler;
    ply.RemoveValue("dooraccess");
    ply.PrintWarning("Вы исцелились");
}

//////////////////////////////////////////////////////////////////////////////
//
//                              Heal status effect
//
//////////////////////////////////////////////////////////////////////////////

public void Heal_Init(Player ply) {
    ply.PrintWarning("Скорость твоего метаболизма возрасла в разы...");
}

public void Heal_Update(Player ply) {
    if (ply.health < ply.class.health)
        if (ply.health + (ply.class.health * 5 / 100) > ply.class.health)
            ply.health = ply.class.health;
        else
            ply.health += ply.class.health * 5 / 100;
}

//////////////////////////////////////////////////////////////////////////////
//
//                              Speed status effect
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
//                              Injure status effect
//
//////////////////////////////////////////////////////////////////////////////

public void Injure_Init(Player ply) {
    ply.PrintWarning("Ваше тело начинает кровоточить из за множества мелких ран");

    char sound[128];
    JSON_ARRAY soundarr = gamemode.plconfig.GetObject("sound").GetArray("playerkill");
    soundarr.GetString(GetRandomInt(0, soundarr.Length - 1), sound, sizeof(sound));
    
    ply.PlaySound(sound);
}

public void Injure_Update(Player ply) {
    ply.health -= (ply.class.health * 2 / 100);
}

public void Injure_End(Player ply) {
    ply.PrintWarning("Вы чувствуете себя немного лучше");
}

//////////////////////////////////////////////////////////////////////////////
//
//                             Butchering status effect
//
//////////////////////////////////////////////////////////////////////////////

public void Butchering(Player ply) {
    ply.PrintWarning("Ваше тело было разделано на компоненты");

    char sound[128];
    JSON_ARRAY soundarr = gamemode.plconfig.GetObject("sound").GetArray("playerkill");
    soundarr.GetString(GetRandomInt(0, soundarr.Length - 1), sound, sizeof(sound));
    
    ply.PlaySound(sound);

    ply.Kill();
}