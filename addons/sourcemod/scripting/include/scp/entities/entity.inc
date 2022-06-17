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

#include "scp/classes/entity/model"

methodmap Entity < Base {

    public Entity(int id = 5000) {
        Entity entity = view_as<Entity>(new Base());
        entity.SetInt("id", id);
        entity.SetBool("spawned", true);
        entity.SetHandle("model", new Mdl(id));

        return entity;
    }

    property int id {
        public set(int id) { this.SetInt("id", id); this.GetBase("model").SetInt("entid", id); }
        public get() { return this.GetInt("id"); }
    }

    property bool spawned {
        public set(bool value) { this.SetBool("spawned", value); }
        public get() { return this.GetBool("spawned"); }
    }

    property Mdl model {
        public set(Mdl value) { this.SetHandle("model", value); }
        public get() { return view_as<Mdl>(this.GetHandle("model")); }
    }

    property EntityMeta meta {
        public set(EntityMeta value) { this.SetHandle("entmeta", value); }
        public get() { return view_as<EntityMeta>(this.GetHandle("entmeta")); }
    }

    public Entity SetKV(char[] name, char[] value) {
        DispatchKeyValue(this.id, name, value);
        return this;
    }

    public bool HasProp(char[] name, any type = Prop_Send) {
        return HasEntProp(this.id, type, name);
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

    public Entity SetPropEnt(char[] name, Entity ent, any type = Prop_Send) {
        SetEntPropEnt(this.id, type, name, ent.id);
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

    public int GetPropEntId(char[] name, any type = Prop_Send, int element = 0) {
        return GetEntPropEnt(this.id, type, name, element);
    }

    public Vector GetPropVector(char[] name, any type = Prop_Send) {
        float pos[3];
        GetEntPropVector(this.id, type, name, pos);
        return new Vector(pos[0], pos[1], pos[2]);
    }

    public Angle GetPropAngle(char[] name, any type = Prop_Send) {
        float pos[3];
        GetEntPropVector(this.id, type, name, pos);
        return new Angle(pos[0], pos[1], pos[2]);
    }

    public Entity Input(char[] input, int activator = -1, int caller = -1) {
        AcceptEntityInput(this.id, input, activator, caller);
        return this;
    }

    public bool IsValid() { return (this.id != 5000 && IsValidEdict(this.id) && IsValidEntity(this.id) ? true : false); }

    public Tmr TimerSimple(int delay, char[] funcname, any args = false) {
        char timername[64];
        FormatEx(timername, sizeof(timername), "ent-%i|time-%i|seed-%i", this.id, GetTime(), GetRandomInt(1, 1000000));
        return gamemode.timer.Create(timername, delay, 1, funcname, args);
    }

    public Entity SetCollisionGroup(int val) { SetEntData(this.id, gamemode.collisiongroup, val, 4, true); return this; }

    public Entity SetMoveType(MoveType type) { SetEntityMoveType(this.id, type); return this; }

    public Entity Create(char[] entclass = "prop_physics_override") {

        this.id = CreateEntityByName(entclass);

        if (this.id == -1)
        {
            gamemode.log.Warning("Can't create a %s entity. (Invalid classname)", entclass);
            return this;
        }

        if (!this.meta && this.id == 5000)
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
        }

        return this;
    }

    public Entity SetClass(char[] name) { this.SetString("class", name); return this; }

    public void GetClass(char[] name, int max_size) { if (!this.GetString("class", name, max_size)) GetEntityClassname(this.id, name, max_size); }

    public bool IsClass(char[] equalClass) { char thisClass[32]; this.GetClass(thisClass, sizeof(thisClass)); return StrEqual(thisClass, equalClass); }

    public Vector GetPos() { return this.GetPropVector("m_vecOrigin"); }

    public Angle GetAng() { return this.GetPropAngle("m_angRotation"); }
    
    public Entity SetPos(Vector vec = null, Angle ang = null) {
        float vectorArr[3], angleArr[3];

        if (vec) vec.GetArr(vectorArr);
        if (ang) ang.GetArr(angleArr);
        
        if (this.spawned)
            TeleportEntity(this.id, (!vec) ? NULL_VECTOR : vectorArr, (!ang) ? NULL_VECTOR : angleArr, NULL_VECTOR);
        else
        {
            if (vec) DispatchKeyValueVector(this.id, "origin", vectorArr);
            if (ang) DispatchKeyValueVector(this.id, "angles", angleArr);
        }

        if (vec) delete vec;
        if (ang) delete ang;

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
        this.model.Dispose();
        delete this;
    }

    public void WorldRemove() {
        RemoveEntity(this.id);
        this.spawned = false;
        this.id = 5000;
    }

    public void Remove() {
        this.SetBool("removing", true);
        RemoveEntity(this.id);
        this.Dispose();
    }
}