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
        public set(EntityMeta value) { this.SetValue("meta", value); }
        public get() { EntityMeta meta; return this.GetValue("meta", meta) ? meta : null; }
    }

    property int id {
        public set(int id) { this.SetInt("id", id); }
        public get() { return this.GetInt("id"); }
    }

    property bool spawned {
        public set(bool value) { this.SetBool("spawned", value); }
        public get() { return this.GetBool("spawned"); }
    }

    public void TimerSimple(int delay, char[] funcname, any args = false) {
        char timername[64], pname[64];
        GetPluginInfo(GetMyHandle(), PlInfo_Name, pname, sizeof(pname));
        FormatEx(timername, sizeof(timername), "%s_entid-%i_%i_%i", pname, this.id, GetTime(), GetRandomInt(1, 1000));
        gamemode.timer.Create(timername, delay, 1, funcname, args);
    }

    public Entity Create(char[] entclass = "prop_physics_override") {
        int id = CreateEntityByName(entclass);
        char modelName[128], mass[12], name[32], bodygroup[12], skin[12];
        this.meta.model(modelName, sizeof(modelName));
        this.meta.name(name, sizeof(name));
        FloatToString(this.meta.mass, mass, sizeof(mass));
        IntToString(this.meta.bodygroup, bodygroup, sizeof(bodygroup));
        IntToString(this.meta.skin, skin, sizeof(skin));

        DispatchKeyValue(id, "model", modelName);
        DispatchKeyValue(id, "physicsmode", "2");
        DispatchKeyValue(id, "massScale", mass);
        DispatchKeyValue(id, "Solid", "6");
        DispatchKeyValue(id, "body", bodygroup);
        DispatchKeyValue(id, "skin", skin);
        if (this.meta.spawnflags != 0) {
            char flags[8];
            IntToString(this.meta.spawnflags, flags, sizeof(flags));
            DispatchKeyValue(id, "spawnflags", flags);
        }
        if (this.meta.usable) {
            char funcname[32];
            this.meta.GetString("function", funcname, sizeof(funcname));
        }
        
        this.id = id;

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

    public Entity UseCB(SDKHookCB cb) {
        SDKHook(this.id, SDKHook_Use, cb);
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

    public Entity Spawn() {
        DispatchSpawn(this.id);
        return this;
    }

    public Entity Input(char[] input, Entity activator = null, Entity caller = null) {
        AcceptEntityInput(this.id, input, (activator) ? activator.id : -1, (caller) ? caller.id : -1);
    }

    public void Dispose() {
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