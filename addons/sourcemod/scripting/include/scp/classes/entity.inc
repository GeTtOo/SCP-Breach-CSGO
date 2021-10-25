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
/*-----------------------------------------------------Entity class-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#pragma semicolon 1
#pragma newdecls required

#include <sdktools>

methodmap Entity < Base {

    public Entity(int id = 5000) {
        Entity entity = view_as<Entity>(new Base());
        entity.SetValue("id", id);
        entity.SetValue("spawned", true);

        return entity;
    }

    property EntityMeta meta {
        public set(EntityMeta value) { this.SetValue("entmeta", value); }
        public get() { EntityMeta meta; return this.GetValue("entmeta", meta) ? meta : null; }
    }

    property ModelMeta mdlmeta {
        public set(ModelMeta value) { this.SetValue("mdlmeta", value); }
        public get() { ModelMeta meta; return this.GetValue("mdlmeta", meta) ? meta : null; }
    }

    property int id {
        public set(int id) { this.SetInt("id", id); }
        public get() { return this.GetInt("id"); }
    }

    property bool spawned {
        public set(bool value) { this.SetBool("spawned", value); }
        public get() { return this.GetBool("spawned"); }
    }

    property ArrayList bglist {
        public set(ArrayList list) { this.SetArrayList("bglist", list); }
        public get() { return this.GetArrayList("bglist"); }
    }

    public bool HasProp(char[] name, any type = Prop_Send) {
        return HasEntProp(this.id, type, name);
    }

    public Entity SetKV(char[] name, char[] value) {
        DispatchKeyValue(this.id, name, value);
        return this;
    }

    public Entity SetProp(char[] name, any value, any type = Prop_Send, int element = 0) {
        SetEntProp(this.id, type, name, value, _, element);
        return this;
    }

    public Entity SetPropFloat(char[] name, any value, any type = Prop_Send) {
        SetEntPropFloat(this.id, type, name, float(value));
        return this;
    }

    public Entity SetPropString(char[] name, char[] value, any type = Prop_Send) {
        SetEntPropString(this.id, type, name, value);
        return this;
    }

    public int GetProp(char[] name, any type = Prop_Send, int element = 0) {
        return GetEntProp(this.id, type, name, _, element);
    }

    public float GetPropFloat(char[] name, any type = Prop_Send) {
        return GetEntPropFloat(this.id, type, name);
    }

    public int GetPropString(char[] name, char[] value, int max_size, any type = Prop_Send) {
        return GetEntPropString(this.id, type, name, value, max_size);
    }

    public Entity GetPropEnt(char[] name, any type = Prop_Send, int element = 0) {
        return new Entity(GetEntPropEnt(this.id, type, name, element));
    }

    public Entity Input(char[] input, Entity activator = null, Entity caller = null) {
        AcceptEntityInput(this.id, input, (activator) ? activator.id : -1, (caller) ? caller.id : -1);
        return this;
    }

    public void SetModel(char[] modelName) { SetEntityModel(this.id, modelName); }

    public void GetModel(char[] modelName, int max_size) { this.GetPropString("m_ModelName", modelName, max_size, Prop_Data); }

    public void SetModelById(char[] modelid) {
        this.mdlmeta = gamemode.meta.GetModel(modelid);

        char path[256];
        this.mdlmeta.Path(path, sizeof(path));
        SetEntityModel(this.id, path);

        int len = this.mdlmeta.bg.Length;

        ArrayList list = new ArrayList();
        for (int i=0; i < len; i++)
            list.Push(0);
        this.bglist = list;
    }

    public void SetBodyGroup(char[] name, int idx) {
        char plymdl[128];
        this.GetModel(plymdl, sizeof(plymdl));

        this.bglist.Set(this.mdlmeta.GetBgId(name), this.mdlmeta.GetBgIdx(name, idx));

        int bgmath = 0;
        for (int i=0; i < this.bglist.Length; i++)
            bgmath += this.bglist.Get(i);

        this.SetProp("m_nBody", bgmath);
    }

    public void SetSkin(int skin) { this.SetProp("m_nSkin", skin); }

    public void SetRenderMode(RenderMode mode) { SetEntityRenderMode(this.id, mode); }

    public void TimerSimple(int delay, char[] funcname, any args = false) {
        char timername[64], pname[64];
        GetPluginInfo(GetMyHandle(), PlInfo_Name, pname, sizeof(pname));
        FormatEx(timername, sizeof(timername), "%s_entid-%i_%i_%i", pname, this.id, GetTime(), GetRandomInt(1, 1000));
        gamemode.timer.Create(timername, delay, 1, funcname, args);
    }

    public void SetMoveType(MoveType type) { SetEntityMoveType(this.id, type); }

    public Entity Create(char[] entclass = "prop_physics_override") {

        this.id = CreateEntityByName(entclass);

        if (this.id == -1)
        {
            gamemode.log.Warning("Can't create a %s entity. (Invalid classname)", entclass);
            return this;
        }

        if (this.meta == null && this.id == 5000)
            gamemode.log.Warning("Can't find a %s entity metadata.", entclass);

        if (this.meta)
        {
            char modelName[128], mass[12], name[32], bodygroup[12], skin[12];
            this.meta.model(modelName, sizeof(modelName));
            this.meta.name(name, sizeof(name));
            FloatToString(this.meta.mass, mass, sizeof(mass));
            IntToString(this.meta.bodygroup, bodygroup, sizeof(bodygroup));
            IntToString(this.meta.skin, skin, sizeof(skin));

            this.SetKV("model", modelName);
            this.SetKV("massScale", mass);
            this.SetKV("body", bodygroup);
            this.SetKV("skin", skin);

            if (this.meta.spawnflags != 0) {
                char flags[8];
                IntToString(this.meta.spawnflags, flags, sizeof(flags));
                this.SetKV("spawnflags", flags);
            }
            
            if (this.meta.usable) {
                char funcname[32];
                this.meta.GetString("function", funcname, sizeof(funcname));
            }
        }

        return this;
    }

    public Entity SetClass(char[] name) { this.SetString("class", name); return this; }

    public void GetClass(char[] name, int max_size) { if (!this.GetString("class", name, max_size)) GetEntityClassname(this.id, name, max_size); }

    public Vector GetPos() { float pos[3]; GetEntPropVector(this.id, Prop_Send, "m_vecOrigin", pos); return new Vector(pos[0], pos[1], pos[2]); }

    public Angle GetAng() { float ang[3]; GetEntPropVector(this.id, Prop_Send, "m_angRotation", ang); return new Angle(ang[0], ang[1], ang[2]); }
    
    public Entity SetPos(Vector vec, Angle ang = null) {
        float vectorArr[3], angleArr[3];
        vec.GetArr(vectorArr);
        if (ang != null)
            ang.GetArr(angleArr);
        
        if (this.spawned)
            TeleportEntity(this.id, vectorArr, (ang == null) ? NULL_VECTOR : angleArr, NULL_VECTOR);
        else
        {
            DispatchKeyValueVector(this.id, "origin", vectorArr);
            if (ang != null)
                DispatchKeyValueVector(this.id, "angles", angleArr);
        }

        delete vec;
        if (ang != null)
            delete ang;
        return this;
    }

    public Entity Push(Entity ent, float force) {
        float vecvel[3];
        this.GetPos().GetFromPoint(ent.GetPos()).Normalize().Scale(force).GetArrD(vecvel);
        TeleportEntity(ent.id, NULL_VECTOR, NULL_VECTOR, vecvel);
        return this;
    }

    public Entity ReversePush(Vector vec, float force) {
        float vecvel[3];
        vec.GetFromPoint(this.GetPos()).Normalize().Scale(force).GetArrD(vecvel);
        TeleportEntity(this.id, NULL_VECTOR, NULL_VECTOR, vecvel);
        return this;
    }

    public Entity SetHook(SDKHookType type, SDKHookCB cb) {
        SDKHook(this.id, type, cb);
        return this;
    }

    public Entity RemoveHook(SDKHookType type, SDKHookCB cb) {
        SDKUnhook(this.id, type, cb);
        return this;
    }

    public Entity Spawn() {
        DispatchSpawn(this.id);
        return this;
    }

    public Entity Activate() {
        ActivateEntity(this.id);
        return this;
    }

    public void Dispose() {
        delete this.bglist;
        delete this;
    }

    public void WorldRemove() {
        RemoveEntity(this.id);
        this.spawned = false;
        this.id = 5000;
    }

    public void Remove() {
        RemoveEntity(this.id);
        this.Dispose();
    }
}