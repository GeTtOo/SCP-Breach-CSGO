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

/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------Player classes----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/
#pragma semicolon 1
#pragma newdecls required

#include <sdktools>
#include <cstrike>
#include <json>

#include "scp/classes/inventory"
#include "scp/classes/progress"

methodmap Player < Entity {

    public Player(int id) {
        Player client = view_as<Player>(new Entity(id));
        client.SetBool("spawned", false);
        client.SetValue("inventory", new Inventory(client));
        client.SetValue("progress", new ProgressBar(client));

        return client;
    }

    property int lang {
        public get() { return GetClientLanguage(this.id); }
    }

    property int health {
        public set(int val) { SetEntityHealth(this.id, val); }
        public get() { return GetClientHealth(this.id); }
    }

    property int armor {
        public set(int val) { SetEntProp(this.id, Prop_Send, "m_ArmorValue", val); }
        public get() { return GetEntProp(this.id, Prop_Send, "m_ArmorValue"); }
    }

    property float speed {
        public set(float val) { SetEntPropFloat(this.id, Prop_Send, "m_flMaxspeed", val); }
        public get() { return GetEntPropFloat(this.id, Prop_Send, "m_flMaxspeed"); }
    }

    property float multipler {
        public set(float val) { SetEntPropFloat(this.id, Prop_Send, "m_flLaggedMovementValue", val); }
        public get() { return GetEntPropFloat(this.id, Prop_Send, "m_flLaggedMovementValue"); }
    }

    property Class class {
        public set(Class val) { this.SetValue("class", val); }
        public get() { Class val; return this.GetValue("class", val) ? val : null; }
    }

    property bool IsSCP {
        public get() { char team[32]; this.GetString("team", team, sizeof(team)); return StrEqual(team, "SCP"); }
    }

    property bool fullaccess {
        public set(bool card) { this.SetBool("fullaccess", card); }
        public get() { return this.GetBool("fullaccess", false); }
    }

    property bool FirstSpawn {
        public set(bool val) { this.SetBool("firstspawn", val); }
        public get() { return this.GetBool("firstspawn", true); }
    }

    property Entity ragdoll {
        public set(Entity val) { this.RemoveValue("ragdoll"); this.SetValue("ragdoll", val); }
        public get() { Entity val; return this.GetValue("ragdoll", val) ? val : null; }
    }

    property Inventory inv {
        public set(Inventory inv) { this.SetValue("inventory", inv); }
        public get() { Inventory inv; return this.GetValue("inventory", inv) ? inv : null; }
    }

    property ProgressBar progress {
        public set(ProgressBar pb) { this.SetValue("progress", pb); }
        public get() { ProgressBar pb; return this.GetValue("progress", pb) ? pb : null; }
    }

    public bool GetName(char[] buffer, int max_size = 0) { return GetClientName(this.id, buffer, max_size); }

    public bool GetAuth(char[] buffer, int max_size = 0, AuthIdType type = AuthId_Steam2) { return GetClientAuthId(this.id, type, buffer, max_size, true); }

    public bool IsAlive() { return IsPlayerAlive(this.id); }

    public bool IsAdmin() { return (GetUserAdmin(this.id) != INVALID_ADMIN_ID) ? true : false; }

    public bool InGame() { return IsClientInGame(this.id); }

    public Vector EyePos() { float vecarr[3]; GetClientEyePosition(this.id, vecarr); return new Vector(vecarr[0], vecarr[1], vecarr[2]); }

    public Angle GetAng() { float angarr[3]; GetClientEyeAngles(this.id, angarr); return new Angle(angarr[0], angarr[1], angarr[2]); }

    public void SetHandsModel(char[] model) { SetEntPropString(this.id, Prop_Send, "m_szArmsModel", model); }

    public void Team(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("team", buffer) : this.GetString("team", buffer, max_size); }

    public void PrintNotify(const char[] format, any ...) {
        int len = strlen(format) + 8192;
        char[] formattedText = new char[len];
        VFormat(formattedText, len, format, 3);
        PrintHintText(this.id, "%s", formattedText);
    }

    public void PrintWarning(const char[] format, any ...) {
        int len = strlen(format) + 8192;
        char[] formattedText = new char[len];
        VFormat(formattedText, len, format, 3);
        PrintCenterText(this.id, "%s", formattedText);
    }

    public void ShowOverlay(char[] name) {
        char overlay[128], langcode[3];
        GetLanguageInfo(GetClientLanguage(this.id), langcode, sizeof(langcode));
        Format(overlay, sizeof(overlay), "r_screenoverlay models/eternity/overlays/%s_%s", name, langcode);
        ClientCommand(this.id, overlay);
    }

    public void HideOverlay() {
        ClientCommand(this.id, "r_screenoverlay none");
    }

    public void SetListen(Player ply, bool islisten)
    {
        SetListenOverride(this.id, ply.id, islisten ? Listen_Yes : Listen_No);
    }

    public void PlaySound(char[] path, int channel = 1, int level = 100, int entity = 0) {
        int clients[1];
        clients[0] = this.id;

        EmitSound(clients, sizeof(clients), path, (entity == 0) ? this.id : entity, channel, level);
    }

    public void StopSound(char[] path, int channel = 1) {
        StopSound(this.id, channel, path);
    }

    public int Give(char[] item) { return GivePlayerItem(this.id, item); }

    public void DropWeapons() {
        int item, weparrsize = GetEntPropArraySize(this.id, Prop_Send, "m_hMyWeapons");

        for(int weparridx = 0; weparridx < weparrsize; weparridx++)
        { 
            item = GetEntPropEnt(this.id, Prop_Send, "m_hMyWeapons", weparridx);

            if(item != -1)
            {
                char wepclass[128];
                GetEntityClassname(item, wepclass, sizeof(wepclass));
                if (!StrEqual(wepclass, "weapon_fists"))
                    CS_DropWeapon(this.id, item, false, false);
            }
        }
    }

    public void RestrictWeapons() {
        ArrayList entities = view_as<Base>(gamemode).GetBase("entities").GetArrayList("entities");
        int item, weparrsize = GetEntPropArraySize(this.id, Prop_Send, "m_hMyWeapons");

        for(int weparridx = 0; weparridx < weparrsize; weparridx++)
        {
            item = GetEntPropEnt(this.id, Prop_Send, "m_hMyWeapons", weparridx);

            if(item != -1)
            {
                int idx = entities.FindValue(item, 0);
                if (idx != -1)
                    entities.Erase(idx);
                RemovePlayerItem(this.id, item);
                AcceptEntityInput(item, "Kill");
            }
        }
    }

    public bool Check(char[] val, int check) {
        ArrayList checklist = this.GetArrayList(val);
        if (checklist) {
            for (int k=0; k < checklist.Length; k++)
                if (checklist.Get(k) == check)
                    return true;
        }
        else
        {
            if (this.GetInt(val) == check)
                return true;
        }

        return false;
    }
    
    public Entity CreateRagdoll() {
        Entity ragdoll;
        
        ragdoll = this.GetPropEnt("m_hRagdoll");

        if (ragdoll && ragdoll.id != -1)
            ragdoll.Input("Kill");

        ragdoll.Dispose();

        ragdoll = new Entity();

        char model[128];
        this.model.GetPath(model, sizeof(model));

        ragdoll.meta = new EntityMeta();
        ragdoll.meta.model(model);
        ragdoll.meta.bodygroup = this.GetProp("m_nBody");
        ragdoll.meta.skin = this.GetProp("m_nSkin");
        ragdoll.meta.spawnflags = 4;

        ragdoll.Create("prop_ragdoll");
        ragdoll.Spawn();
        ragdoll.SetPos(this.GetPos(), this.GetAng());

        return ragdoll;
    }

    //public void SetCollisionGroup(int group) { gamemode.mngr.SetCollisionGroup(this.id, group); }

    public void Kill() {
        this.DropWeapons();
        ForcePlayerSuicide(this.id);
    }

    public void SilenceKill() {
        this.RestrictWeapons();
        this.inv.FullClear();
        this.Kill();
    }

    public void Kick(char[] reason) { KickClient(this.id, reason); }

    public void SetupBaseStats(Class class) {
        this.armor = 0;
        this.SetProp("m_bHasHelmet", 0);
        
        this.health = class.health;
        if (class.armor != 0)
            this.armor = class.armor;
        if (class.helmet)
            this.SetProp("m_bHasHelmet", 1);
        this.speed = class.speed;
        this.multipler = class.multipler;
    }

    public void Setup() {
        this.SetupBaseStats(this.class);

        JSON_OBJECT pos = this.class.GetVecAng();
        if (pos)
        {
            this.SetPos(pos.GetVector("vec"), pos.GetAngle("ang"));
            pos.SetInt("lock", true);
            this.SetBase("spawnpos", view_as<Base>(pos));
        }

        if (this.class.items)
        {
            JSON_ARRAY items = this.class.items;

            for (int i=0; i < items.Length; i++) {
                if (view_as<int>(items.GetKeyType(i)) == 0)
                {
                    char entclass[32];
                    items.GetString(i, entclass, sizeof(entclass));
                    this.inv.Give(entclass);
                }
                else
                {
                    JSON_OBJECT itemsobj = view_as<JSON_OBJECT>(items.GetObject(i));
                    StringMapSnapshot sitems = itemsobj.Snapshot();
                    int keylen;
                    int random = GetRandomInt(1,100);
                    int count = 0;
                    for (int k=0; k < sitems.Length; k++) {
                        keylen = sitems.KeyBufferSize(k);
                        char[] chance = new char[keylen];
                        sitems.GetKey(k, chance, keylen);
                        if (json_is_meta_key(chance)) continue;

                        count += StringToInt(chance);
                        if (count >= random) {
                            char entclass[32];
                            itemsobj.GetString(chance, entclass, sizeof(entclass));
                            this.inv.Give(entclass);
                            break;
                        }
                    }

                    delete sitems;
                }
            }
        }

        if (this.class.weapons)
            for (int i=0; i < this.class.weapons.Length; i++) {
                if (view_as<int>(this.class.weapons.GetKeyType(i)) != 4) {
                    char weapon[32];
                    this.class.weapons.GetString(i, weapon, sizeof(weapon));
                    Entity ent = new Entity(this.Give(weapon));
                    ent.SetString("class", weapon);

                    any data[2];
                    data[0] = ent.id;
                    data[1] = ent;
                    
                    Base entities;
                    gamemode.GetValue("entities", entities);
                    entities.GetArrayList("entities").PushArray(data);
                }
                else
                {
                    JSON_ARRAY multigive = view_as<JSON_ARRAY>(this.class.weapons.GetObject(i));

                    for (int k=0; k < multigive.GetInt(1); k++)
                    {
                        char weapon[32];
                        multigive.GetString(0, weapon, sizeof(weapon));
                        Entity ent = new Entity(this.Give(weapon));
                        ent.SetString("class", weapon);

                        any data[2];
                        data[0] = ent.id;
                        data[1] = ent;
                        
                        Base entities;
                        gamemode.GetValue("entities", entities);
                        entities.GetArrayList("entities").PushArray(data);
                    }
                }
            }
        
        if (this.class.doors)
            if (this.class.GetInt("doorsdelay") == 0)
                for (int i=0; i < this.class.doors.Length; i++)
                    AcceptEntityInput(this.class.doors.GetInt(i), "Open");
            else
                this.TimerSimple(this.class.GetInt("doorsdelay") * 1000, "OpenCameraDoors", this.class.doors);

        if (this.class.HasKey("model")) {
            if (view_as<int>(this.class.GetKeyType("model")) == 0)
            {
                char modelName[256];
                this.class.Model(modelName, sizeof(modelName));
                this.model.SetPath(modelName);
            }
            else
            {
                JSON_OBJECT modeldata = this.class.GetObject("model");

                char mdlid[32];
                modeldata.GetString("id", mdlid, sizeof(mdlid));
                this.model.SetById(mdlid);

                if (modeldata.HasKey("bodygroups"))
                    if (view_as<int>(modeldata.GetKeyType("bodygroups")) == 1) {
                        this.SetProp("m_nBody", modeldata.GetInt("bodygroups", 0));
                    }
                    else
                    {
                        JSON_OBJECT pgi = modeldata.GetObject("bodygroups");
                        StringMapSnapshot spgi = pgi.Snapshot();
                        
                        int keylen;
                        for (int i=0; i < spgi.Length; i++) {
                            keylen = spgi.KeyBufferSize(i);
                            char[] name = new char[keylen];
                            spgi.GetKey(i, name, keylen);
                            if (json_is_meta_key(name)) continue;
                            
                            if (view_as<int>(pgi.GetKeyType(name)) == 1)
                                this.model.SetBodyGroup(name, pgi.GetInt(name));
                            else
                                this.model.SetBodyGroup(name, pgi.GetArray(name).GetInt(GetRandomInt(0, pgi.GetArray(name).Length - 1)));
                        }

                        delete spgi;
                    }

                if (modeldata.HasKey("skin"))
                    this.model.SetSkin(modeldata.GetInt("skin"));
            }
        }

        if (this.class.HasKey("handsmodel")) {
            char handsModel[128];
            this.class.Model(handsModel, sizeof(handsModel));

            //this.SetHandsModel(handsModel); Bug...
        }

        char teamName[32], className[32];
        this.Team(teamName, sizeof(teamName));
        this.class.Name(className, sizeof(className));
        PrintToChat(this.id, " \x07[SCP] \x01%t", "Show class when player spawn", teamName, className);
    }

    public void Spawn() {
        this.spawned = true;
        if (!IsPlayerAlive(this.id))
            CS_RespawnPlayer(this.id);
    }

    public void UpdateClass() {
        this.Spawn();
        this.RestrictWeapons();
        this.inv.FullClear();
        this.Setup();
    }

    public void Dispose() {
        delete this;
    }
}