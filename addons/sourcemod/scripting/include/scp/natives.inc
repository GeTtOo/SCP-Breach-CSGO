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

//////////////////////////////////////////////////////////////////////////////
//
//                                Natives
//
//////////////////////////////////////////////////////////////////////////////

public any NativeGameMode_CollisionGroup(Handle Plugin, int numArgs) { return gamemode.GetInt("collisiongroup"); }

public any NativeGameMode_Config(Handle Plugin, int numArgs) { return view_as<Config>(gamemode.GetHandle("Config")); }

public any NativeGameMode_Meta(Handle Plugin, int numArgs) { return view_as<Meta>(gamemode.GetHandle("Meta")); }

public any NativeGameMode_Manager(Handle Plugin, int numArgs) { return view_as<Manager>(gamemode.GetHandle("Manager")); }

public any NativeGameMode_Nuke(Handle Plugin, int numArgs) { return view_as<NuclearWarhead>(gamemode.GetHandle("Nuke")); }

public any NativeGameMode_Timers(Handle Plugin, int numArgs) { return view_as<Timers>(gamemode.GetHandle("Timers")); }

public any NativeStatusEffect_GetList(Handle Plugin, int numArgs) { return statuseffect.GetArrayList("list"); }

public any NativeGameMode_Logger(Handle Plugin, int numArgs) { return view_as<Logger>(gamemode.GetHandle("Logger")); }

public any NativeGameMode_TeamList(Handle Plugin, int numArgs) {
    bool filter = GetNativeCell(2);
    ArrayList list = new ArrayList(32);
    StringMapSnapshot snap = view_as<JSON_OBJECT>(gamemode.GetHandle("Teams")).Snapshot();
    int keylength;
    for (int i=0; i < snap.Length; i++) {
        keylength = snap.KeyBufferSize(i);
        char[] teamName = new char[keylength];
        snap.GetKey(i, teamName, keylength);
        if (filter && gamemode.team(teamName).percent == 0) continue;
        list.PushString(teamName);
    }
    delete snap;
    return list;
}

public any NativeGameMode_GetTeam(Handle Plugin, int numArgs) {
    char name[32];
    GetNativeString(2, name, sizeof(name));
    return view_as<Teams>(gamemode.GetHandle("Teams")).get(name);
}

public any NativeTimers_GetList(Handle Plugin, int numArgs) { return timer.GetArrayList("timers"); }

public any NativeTimers_HideCreate(Handle Plugin, int numArgs) {
    char timername[64], funcname[64];
    GetNativeString(3, timername, sizeof(timername));
    GetNativeString(6, funcname, sizeof(funcname));

    Tmr tmr = new Tmr(GetNativeCell(2), timername, GetNativeCell(4), GetNativeCell(5), funcname, GetNativeCell(7));
    tmr.active = true;
    timer.GetArrayList("timers").Push(tmr);
    return tmr;
}

public any NativeStatusEffects_Create(Handle Plugin, int numArgs) {
    Player ply = GetNativeCell(2);
    char sename[64];
    GetNativeString(3, sename, sizeof(sename));

    StatusEffect se = new StatusEffect(ply, sename, GetNativeCell(4));
    
    if (!statuseffect.IsHave(ply, sename)) statuseffect.list.Push(se);
    return se;
}

public any NativeStatusEffects_Remove(Handle Plugin, int numArgs) {
    Player ply = GetNativeCell(2);
    char sename[32];
    GetNativeString(3, sename, sizeof(sename));

    for (int i=0; i < statuseffect.list.Length; i ++)
    {
        StatusEffect se = statuseffect.list.Get(i);
        
        char sen[64];
        se.name(sen, sizeof(sen));

        if (se.ply == ply && StrEqual(sename, sen))
        {
            char funcname[64];
            
            se.meta.name(funcname, sizeof(funcname));
            Format(funcname, sizeof(funcname), "%s_ForceEnd", funcname);

            if (se.meta.end && GetFunctionByName(se.meta.end.hndl, funcname) != INVALID_FUNCTION)
            {
                Call_StartFunction(se.meta.end.hndl, GetFunctionByName(se.meta.end.hndl, funcname));
                Call_PushCell(se.ply);
                Call_Finish();
            }

            statuseffect.list.Erase(i);
            delete se;
            i--;
            return true;
        }
    }

    return false;
}

public any NativeStatusEffects_ClearAllOnPlayer(Handle Plugin, int numArgs) {
    Player ply = GetNativeCell(2);
    
    for (int i=0; i < statuseffect.list.Length; i ++)
    {
        StatusEffect se = statuseffect.list.Get(i);

        if (se.ply == ply)
        {
            char funcname[64];
            
            se.meta.name(funcname, sizeof(funcname));
            Format(funcname, sizeof(funcname), "%s_ForceEnd", funcname);

            if (se.meta.end && GetFunctionByName(se.meta.end.hndl, funcname) != INVALID_FUNCTION)
            {
                Call_StartFunction(se.meta.end.hndl, GetFunctionByName(se.meta.end.hndl, funcname));
                Call_PushCell(se.ply);
                Call_Finish();
            }

            statuseffect.list.Erase(i);
            delete se;
            i--;
        }
    }

    return false;
}

public any NativeEntities_GetList(Handle Plugin, int numArgs) { return ents.GetArrayList("entities"); }

public any NativeClients_Add(Handle Plugin, int numArgs) {
    return ents.Push(new Player(GetNativeCell(2)));
}

public any NativeClients_Remove(Handle Plugin, int numArgs) {
    ArrayList entities = ents.list;
    int idx = entities.FindValue(GetNativeCell(2), 0);
    view_as<Player>(entities.Get(idx, 1)).Dispose();
    entities.Erase(idx);
    return false;
}

public any NativeEntities_Create(Handle Plugin, int numArgs) {
    char EntName[32];
    GetNativeString(2, EntName, sizeof(EntName));

    EntityMeta entdata = gamemode.meta.GetEntity(EntName);
    
    Entity entity;
    if (entdata)
    {
        entity = new Entity();
        entity.meta = entdata;
        entity.Create();
        
        entity.SetHook(SDKHook_Use, CB_EntUse);
        if (entity.meta.ontouch) entity.SetHook(SDKHook_TouchPost, CB_EntTouch);
    }
    else
    {
        entity = new Entity(CreateEntityByName(EntName));
    }

    entity.spawned = false;
    entity.SetClass(EntName);
    
    if (view_as<bool>(GetNativeCell(3)))
    {
        ents.Push(entity);
    }

    return entity;
}

public any NativeEntities_Push(Handle Plugin, int numArgs) {
    Entity ent = GetNativeCell(2);

    any data[2];
    data[0] = ent.id;
    data[1] = ent;

    ents.list.PushArray(data);

    return ent;
}

public any NativeEntities_Remove(Handle Plugin, int numArgs) {
    ArrayList entities = ents.list;
    Entity entin = GetNativeCell(2);
    int idx = entities.FindValue(entin, 1);
    if (idx != -1)
    {
        Entity ent = entities.Get(idx, 1);
        if (ent.meta)
        {
            ent.RemoveHook(SDKHook_Use, CB_EntUse);
            if (ent.meta.ontouch) ent.RemoveHook(SDKHook_TouchPost, CB_EntTouch);
        }
        ent.Remove();
        entities.Erase(idx);
    }
    else
    {
        char classname[32];
        entin.GetClass(classname, sizeof(classname));
        gamemode.log.Error("Cant find entity in storage. id:%i, class:%s", entin.id, classname);
        entin.Remove();
    }

    return false;
}

public any NativeEntities_RemoveByID(Handle Plugin, int numArgs) {
    ArrayList entities = ents.list;
    int id = GetNativeCell(2);
    int idx = entities.FindValue(id, 0);
    if (idx != -1)
    {
        Entity ent = entities.Get(idx, 1);
        if (ent.meta)
        {
            ent.RemoveHook(SDKHook_Use, CB_EntUse);
            if (ent.meta.ontouch) ent.RemoveHook(SDKHook_TouchPost, CB_EntTouch);
        }
        ent.Remove();
        entities.Erase(idx);
    }
    else
    {
        gamemode.log.Error("Cant find entity in storage. id:%i", id);
    }

    return false;
}

public any NativeEntities_Dissolve(Handle Plugin, int numArgs) {
    ArrayList entities = ents.list;
    Entity ent = GetNativeCell(2);
    int idx = entities.FindValue(ent, 1);

    char targetname[32];
    ent.GetTargetName(targetname, sizeof(targetname));

    Entity disolver = new Entity();
    disolver.Create("env_entity_dissolver");
    disolver.SetKV("dissolvetype", "0");
    disolver.SetKV("target", targetname);

    if (idx != -1)
    {
        ent.Dispose();
        ents.list.Erase(idx);
    }
    else
    {
        char classname[32];
        ent.GetClass(classname, sizeof(classname));
        gamemode.log.Error("Cant find entity in storage. id:%i, class:%s", ent.id, classname);
        ent.Dispose();
    }

    disolver.Input("Dissolve");
    disolver.Remove();

    return false;
}

public any NativeEntities_IndexUpdate(Handle Plugin, int numArgs) {
    Entity ent = GetNativeCell(2);
    ArrayList entities = ents.list;
    int idx = entities.FindValue(ent, 1);
    if (idx != -1)
    {
        entities.Set(idx, ent.id, 0);
        return true;
    }
    else
        return false;
}

public any NativeEntities_Clear(Handle Plugin, int numArgs) {

    ArrayList entities = ents.list;

    for(int i=0; i < entities.Length; i++)
    {
        int id = entities.Get(i, 0);

        if (id > MaxClients)
        {
            Entity ent = entities.Get(i, 1);
            ent.Dispose();
            entities.Erase(i);
            i--;
        }
    }

    return false;
}

public any NativeWT_Create(Handle Plugin, int numArgs) {
    WorldText wt = view_as<WorldText>(ents.Create("point_worldtext"));

    wt.type = GetNativeCell(4);
    wt.SetPos(GetNativeCell(2), GetNativeCell(3));

    return wt;
}

public any NativeWT_Remove(Handle Plugin, int numArgs) {
    ArrayList entities = ents.list;
    int idx = entities.FindValue(GetNativeCell(2), 1);
    if (idx != -1)
    {
        Entity ent = entities.Get(idx, 0);
        ent.Remove();
        ents.list.Erase(idx);
    }

    return false;
}

public any NativePlayer_SetupEquipment(Handle Plugin, int numArgs) {
    Player ply = GetNativeCell(1);

    if (ply.class.items)
    {
        for (int i=0; i < ply.class.items.Length; i++) {
            if (ply.class.items.GetType(i) == String)
            {
                char itemname[32];
                ply.class.items.GetString(i, itemname, sizeof(itemname));
                ply.inv.Give(itemname);
            }
            else
            {
                JSON_OBJECT items = view_as<JSON_OBJECT>(ply.class.items.Get(i));

                if (items.IsArray)
                {
                    char itemname[32];
                    view_as<JSON_ARRAY>(items).GetString(GetRandomInt(0, view_as<JSON_ARRAY>(items).Length - 1), itemname, sizeof(itemname));
                    ply.inv.Give(itemname);
                    continue;
                }
                
                StringMapSnapshot sitems = items.Snapshot();
                int keylen, random = GetRandomInt(1,100), count = 0;

                for (int k=0; k < sitems.Length; k++) {
                    keylen = sitems.KeyBufferSize(k);
                    char[] chance = new char[keylen];
                    sitems.GetKey(k, chance, keylen);

                    count += StringToInt(chance);
                    char itemname[32];
                    if (count >= random) {
                        items.GetString(chance, itemname, sizeof(itemname));
                        ply.inv.Give(itemname);
                        break;
                    }
                }

                delete sitems;
            }
        }
    }

    if (ply.class.fists) EquipPlayerWeapon(ply.id, ply.Give("weapon_fists").id);

    if (ply.class.weapons)
        for (int i=0; i < ply.class.weapons.Length; i++) {
            if (ply.class.weapons.GetType(i) == String) {
                char weapon[32];
                ply.class.weapons.GetString(i, weapon, sizeof(weapon));
                ply.Give(weapon);
            }
            else
            {
                JSON_OBJECT weapons = view_as<JSON_OBJECT>(ply.class.weapons.Get(i));

                if (weapons.IsArray)
                {
                    char weapon[32];
                    view_as<JSON_ARRAY>(weapons).GetString(GetRandomInt(0, view_as<JSON_ARRAY>(weapons).Length - 1), weapon, sizeof(weapon));
                    ply.Give(weapon);
                    continue;
                }
                
                StringMapSnapshot sweapons = weapons.Snapshot();
                int keylen, random = GetRandomInt(1,100), count = 0;
                
                for (int k=0; k < sweapons.Length; k++) {
                    keylen = sweapons.KeyBufferSize(k);
                    char[] chance = new char[keylen];
                    sweapons.GetKey(k, chance, keylen);

                    count += StringToInt(chance);
                    char weapon[32];
                    if (count >= random) {
                        weapons.GetString(chance, weapon, sizeof(weapon));
                        ply.Give(weapon);
                        break;
                    }
                }

                delete sweapons;
            }
        }
    
    if (ply.class.doors)
        if (ply.class.GetInt("doorsdelay") == 0)
            for (int i=0; i < ply.class.doors.Length; i++)
            {
                char entclass[32];
                GetEntityClassname(ply.class.doors.GetInt(i), entclass, sizeof(entclass))

                if (StrEqual(entclass, "func_button")) AcceptEntityInput(ply.class.doors.GetInt(i), "Use");
                else if (StrEqual(entclass, "func_door")) AcceptEntityInput(ply.class.doors.GetInt(i), "Open");
            }
        else
            ply.TimerSimple(ply.class.GetInt("doorsdelay") * 1000, "OpenCameraDoors", ply.class.doors);

    return true;
}

public any NativePlayer_SetupModel(Handle Plugin, int numArgs) {
    Player ply = GetNativeCell(1);
    
    if (ply.class.HasKey("model")) {
        if (ply.class.GetType("model") == String)
        {
            char modelName[256];
            ply.class.Model(modelName, sizeof(modelName));
            ply.model.SetPath(modelName);
        }
        else
        {
            JSON_OBJECT modeldata = ply.class.Get("model");

            char mdlid[32];
            modeldata.GetString("id", mdlid, sizeof(mdlid));
            ply.model.SetById(mdlid);

            if (modeldata.HasKey("bodygroups"))
                if (modeldata.GetType("bodygroups") == Int) {
                    ply.SetProp("m_nBody", modeldata.GetInt("bodygroups", 0));
                }
                else
                {
                    JSON_OBJECT pgi = modeldata.Get("bodygroups");
                    StringMapSnapshot spgi = pgi.Snapshot();
                    
                    int keylen;
                    for (int i=0; i < spgi.Length; i++) {
                        keylen = spgi.KeyBufferSize(i);
                        char[] name = new char[keylen];
                        spgi.GetKey(i, name, keylen);
                        
                        if (pgi.GetType(name) == Int)
                            ply.model.SetBodyGroup(name, pgi.GetInt(name));
                        else
                            ply.model.SetBodyGroup(name, pgi.GetArr(name).GetInt(GetRandomInt(0, pgi.GetArr(name).Length - 1)));
                    }

                    delete spgi;
                }

            if (modeldata.HasKey("skin"))
                ply.model.SetSkin(modeldata.GetInt("skin"));
        }
    }

    if (ply.class.HasKey("handsmodel")) {
        char handsModel[128];
        ply.class.HandsModel(handsModel, sizeof(handsModel));

        ply.SetHandsModel(handsModel);
    }

    return true;
}

public any NativePlayer_Spawn(Handle Plugin, int numArgs) {
    Player ply = GetNativeCell(1);
    if (!ply.HasKey("meta-setpos")) ply.SetBool("meta-setpos", GetNativeCell(2));
    if (!ply.HasKey("meta-setmdl")) ply.SetBool("meta-setmdl", GetNativeCell(3));
    if (!ply.HasKey("meta-setequip")) ply.SetBool("meta-setequip", GetNativeCell(4));
    if (!ply.HasKey("meta-isclassupdate")) ply.SetBool("meta-isclassupdate", GetNativeCell(5));
    if (!ply.HasKey("meta-cleanequip")) ply.SetBool("meta-cleanequip", GetNativeCell(6));
    if (!ply.HasKey("meta-cleanse")) ply.SetBool("meta-cleanse", GetNativeCell(7));

    ply.spawned = true;

    if (!ply.IsAlive())
    {
        CS_RespawnPlayer(ply.id);
        return true;
    }

    bool setpos = ply.GetBool("meta-setpos");
    bool setmdl = ply.GetBool("meta-setmdl");
    bool setequip = ply.GetBool("meta-setequip");
    bool isclassupdate = ply.GetBool("meta-isclassupdate");
    bool cleanequip = ply.GetBool("meta-cleanequip");
    bool cleanse = ply.GetBool("meta-cleanse");
    ply.RemoveValue("meta-setpos");
    ply.RemoveValue("meta-setmdl");
    ply.RemoveValue("meta-setequip");
    ply.RemoveValue("meta-isclassupdate");
    ply.RemoveValue("meta-cleanequip");
    ply.RemoveValue("meta-cleanse");

    if (isclassupdate)
    {
        char timername[32];
        FormatEx(timername, sizeof(timername), "ent-%i", ply.id);
        timer.RemoveIsContains(timername);

        if (cleanse) ply.se.ClearAll();
        if (cleanequip)
        {
            ply.inv.FullClear();
            ply.RestrictWeapons();
        }

        if (ply.GetHandle("spawnpos")) view_as<JSON_OBJECT>(ply.GetHandle("spawnpos")).SetBool("lock", false);

        Call_StartForward(PreClientSpawnForward);
        Call_PushCellRef(ply);
        Call_Finish();
    }

    ply.SetupPos(setpos);

    if (ply.ragdoll)
    {
        delete ply.ragdoll.meta;
        ents.Remove(ply.ragdoll);
        ply.ragdoll = null;
    }

    ply.SetupBaseStats();

    if (setmdl) ply.SetupModel();
    if (setequip) ply.SetupEquipment();

    char team[32], class[32];

    ply.Team(team, sizeof(team));
    ply.class.GetString("name", class, sizeof(class));
    
    Call_StartForward(OnPlayerSpawnForward);
    Call_PushCellRef(ply);
    Call_Finish();

    if (isclassupdate)
    {
        Call_StartForward(OnPlayerUpdateClass);
        Call_PushCellRef(ply);
        Call_Finish();
    }

    if (ply.class.HasKey("overlay"))
    {
        char path[256];
        ply.class.overlay(path, sizeof(path));
        ply.ShowOverlay(path);
    
        ply.TimerSimple(gamemode.config.showoverlaytime * 1000, "PlyHideOverlay", ply);
    }

    PrintToChat(ply.id, " \x07[SCP] \x01%t", "Show class when player spawn", team, class);

    ply.TimerSimple(1, "PostPlayerSpawn", ply);

    gamemode.log.Debug("Player %L spawned | Team/Class: (%s - %s)", ply.id, team, class);

    return true;
}

public any NativePlayer_GiveWeapon(Handle Plugin, int numArgs) {
    Player ply = GetNativeCell(1);
    
    char itemname[32];
    GetNativeString(2, itemname, sizeof(itemname));

    Entity item = new Entity(GivePlayerItem(ply.id, itemname));
    item.SetClass(itemname);

    ents.Push(item);

    return item;
}

public any NativePlayer_Inventory_GiveItem(Handle Plugin, int numArgs) {
    Inventory inv = GetNativeCell(1);
    Player ply = view_as<Player>(inv.GetBase("ply"));
    
    char itemname[32];
    GetNativeString(2, itemname, sizeof(itemname));

    if (inv.list.Length <= gamemode.config.invsize) {
        EntityMeta entdata = gamemode.meta.GetEntity(itemname);

        if (entdata)
        {
            Entity ent = new Entity();
            ent.meta = entdata;
            ent.spawned = false;
            ent.SetClass(itemname);

            inv.list.Push(ents.Push(ent));

            if (ent.meta.onpickup)
            {
                char funcname[32];
                ent.meta.onpickup.name(funcname, sizeof(funcname));

                Call_StartFunction(ent.meta.onpickup.hndl, GetFunctionByName(ent.meta.onpickup.hndl, funcname));
                Call_PushCellRef(ply);
                Call_PushCellRef(ent);
                Call_Finish();
            }

            return ent;
        }
        else
        {
            gamemode.log.Warning("Can't give item: %s for player with id: %i. (Doesn't have metadata)", itemname, ply.id);
        }
    }
    else
    {
        gamemode.log.Info("Can't give item: %s for player with id: %i. (Don't have enough space in inventory)", itemname, ply.id);
    }

    return view_as<Entity>(null);
}

public int NativePlayer_DropWeapons(Handle Plugin, int numArgs) {
    Player ply = GetNativeCell(1);

    int itemid, weparrsize = GetEntPropArraySize(ply.id, Prop_Send, "m_hMyWeapons");

    for(int weparridx = 0; weparridx < weparrsize; weparridx++)
    { 
        itemid = GetEntPropEnt(ply.id, Prop_Send, "m_hMyWeapons", weparridx);

        if(itemid != -1)
        {
            char wepclass[64];
            GetEntityClassname(itemid, wepclass, sizeof(wepclass));
            
            if (!StrEqual(wepclass, "weapon_fists"))
                CS_DropWeapon(ply.id, itemid, false, false);
        }
    }

    return 0;
}

public int NativePlayer_RestrictWeapons(Handle Plugin, int numArgs) {
    Player ply = GetNativeCell(1);

    int itemid, weparrsize = GetEntPropArraySize(ply.id, Prop_Send, "m_hMyWeapons");
    for(int weparridx = 0; weparridx < weparrsize; weparridx++)
    {
        itemid = GetEntPropEnt(ply.id, Prop_Send, "m_hMyWeapons", weparridx);

        if(itemid != -1)
        {
            char wepclass[64];
            GetEntityClassname(itemid, wepclass, sizeof(wepclass));
            
            if (!StrEqual(wepclass, "weapon_fists")) ents.RemoveByID(itemid);
            
            RemovePlayerItem(ply.id, itemid);
            AcceptEntityInput(itemid, "Kill");
        }
    }

    return 0;
}

public any NativePlayer_Inventory_Drop(Handle Plugin, int numArgs) {
    Inventory inv = GetNativeCell(1);
    InvItem item = GetNativeCell(2);
    Player ply = view_as<Player>(inv.ply);
    
    if (!item) return view_as<InvItem>(null);

    inv.list.Erase(inv.list.FindValue(item));

    if (item.meta.ondrop)
    {
        char funcname[32];
        item.meta.ondrop.name(funcname, sizeof(funcname));

        Call_StartFunction(item.meta.ondrop.hndl, GetFunctionByName(item.meta.ondrop.hndl, funcname));
        Call_PushCellRef(ply);
        Call_PushCellRef(item);
        Call_Finish();
    }

    item
    .Create()
    .SetPos(ply.GetAng().GetForwardVectorScaled(ply.EyePos(), 5.0) - new Vector(0.0, 0.0, 15.0), ply.GetAng())
    .Spawn()
    .ReversePush(ply.EyePos() - new Vector(0.0, 0.0, 15.0), 250.0);

    item.SetHook(SDKHook_Use, CB_EntUse);
    if (item.meta.ontouch) item.SetHook(SDKHook_TouchPost, CB_EntTouch);

    ents.IndexUpdate(item);
    
    return item;
}

public any NativePlayer_Inventory_DropByIdx(Handle Plugin, int numArgs) {
    Inventory inv = GetNativeCell(1);
    Player ply = view_as<Player>(inv.ply);
    return ply.inv.Drop(inv.Get(GetNativeCell(2)));
}

public int NativePlayer_Inventory_DropAll(Handle Plugin, int numArgs) {
    Player ply = view_as<Player>(view_as<Base>(GetNativeCell(1)).GetHandle("ply"));

    while (ply.inv.list.Length != 0)
        ply.inv.DropByIdx();

    return 0;
}

public any NativePlayer_Inventory_Remove(Handle Plugin, int numArgs) {
    Inventory inv = GetNativeCell(1);
    InvItem item = GetNativeCell(2);
    
    if (!item) return false;

    for (int i=0; i < inv.list.Length; i++) {
        if (inv.list.Get(i) == item) {
            inv.list.Erase(i);
            
            if (item.id == 5000)
            {
                ents.list.Erase(ents.list.FindValue(item, 1));
                delete item;
                return true;
            }
            
            ents.Remove(item);
            delete item;
            return true;
        }
    }
    
    return false;
}

public int NativePlayer_Inventory_FullClear(Handle Plugin, int numArgs) {
    ArrayList entities = ents.list;
    Player ply = view_as<Player>(view_as<Base>(GetNativeCell(1)).GetHandle("ply"));

    while (ply.inv.list.Length != 0) {
        InvItem item = ply.inv.Get();
        ply.inv.list.Erase(0);
        entities.Erase(entities.FindValue(item, 1));
        item.Dispose();
    }

    return 0;
}