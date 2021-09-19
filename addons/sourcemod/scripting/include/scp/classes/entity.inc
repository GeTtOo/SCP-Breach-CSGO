/*----------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------Entity class-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#pragma semicolon 1
#pragma newdecls required

#include <sdktools>

methodmap Entity < Base {

    public Entity(int id) {
        Entity entity = view_as<Entity>(new Base());

        entity.SetValue("id", id);
        entity.SetValue("active", true);

        return entity;
    }

    property int id {
        public get() { return this.GetInt("id"); }
    }

    property bool active {
        public set(bool value) { this.SetBool("active", value); }
        public get() { return this.GetBool("active"); }
    }

    public void GetClass(char[] name, int max_size) { if (!this.GetString("class", name, max_size)) GetEntityClassname(this.id, name, max_size); }

    public Vector GetPos() { float pos[3]; GetEntPropVector(this.id, Prop_Send, "m_vecOrigin", pos); return new Vector(pos[0], pos[1], pos[2]); }

    public Angle GetAng() { float ang[3]; GetEntPropVector(this.id, Prop_Send, "m_angRotation", ang); return new Angle(ang[0], ang[1], ang[2]); }
    
    public Entity SetPos(Vector vec, Angle ang = null) {
        float vectorArr[3], angleArr[3];
        vec.GetArr(vectorArr);
        if (ang != null)
            ang.GetArr(angleArr);
        
        if (this.active)
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

    public Entity Spawn() {
        DispatchSpawn(this.id);
        return this;
    }

    public Entity Input(char[] input, Entity activator = null, Entity caller = null) {
        AcceptEntityInput(this.id, input, (activator) ? activator.id : -1, (caller) ? caller.id : -1);
    }

    public void Remove() {
        RemoveEntity(this.id);
        delete this;
    }
}