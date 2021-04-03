/*----------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------Entity class-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#pragma semicolon 1
#pragma newdecls required

#include <sdktools>

methodmap Entity < Base {

    public Entity(int id) {
        Entity entity = view_as<Entity>(CreateTrie());

        entity.SetValue("id", id);
        entity.SetValue("active", true);

        return entity;
    }

    property int id {
        public get() { return this.GetInt("id"); }
    }

    property bool active {
        public set(bool value) { this.SetValue("active", value); }
        public get() { bool value; return this.GetValue("active", value) ? value : true; }
    }

    public void InvName(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("invname", buffer) : this.GetString("invname", buffer, max_size); }

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
        }

        delete vec;
        return this;
    }

    public Entity UseCB(SDKHookCB cb) {
        SDKHook(this.id, SDKHook_Use, cb);
        return this;
    }

    public Entity Spawn() {
        DispatchSpawn(this.id);
        return this;
    }

    public void Remove() {
        RemoveEntity(this.id);
        delete this;
    }
}