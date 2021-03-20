/*----------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------Entity class-----------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#pragma semicolon 1
#pragma newdecls required

#include <sdktools>

methodmap Entity < StringMap {

    public Entity(int id) {
        Entity entity = view_as<Entity>(new StringMap());

        entity.SetValue("id", id);
        entity.SetValue("active", true);

        return entity;
    }

    property int id {
        public get() { int id; return this.GetValue("id", id) ? id : -1; }
    }

    property bool active {
        public set(bool value) { this.SetValue("active", value); }
        public get() { bool value; return this.GetValue("active", value) ? value : true; }
    }

    public void GetClass(char[] name, int max_size) {
        int id;
        this.GetValue("id", id);
        GetEntityClassname(id, name, max_size);
    }
    
    public Entity SetPos(Vector vec) {
        float vectorArr[3];
        vec.GetArr(vectorArr);
        
        if (this.active)
            TeleportEntity(this.id, vectorArr, NULL_VECTOR, NULL_VECTOR);
        else
            DispatchKeyValueVector(this.id, "origin", vectorArr);

        delete vec;
        return this;
    }

    public Entity Spawn() {
        DispatchSpawn(this.id);
        return this;
    }

    public Vector GetPos() {
        int id;
        this.GetValue("id", id);

        float vec[3];
        GetEntPropVector(id, Prop_Send, "m_vecOrigin", vec);

        return new Vector(vec[0], vec[1], vec[2]);
    }

    public void GetAng(float[3] ang) {
        int id;
        this.GetValue("id", id);
        GetEntPropVector(id, Prop_Send, "m_angRotation", ang);
    }

    public Entity UseCB(SDKHookCB cb) {
        SDKHook(this.id, SDKHook_Use, cb);
        return this;
    }
}

methodmap EntitySingleton < StringMap {

    public EntitySingleton() {
        EntitySingleton self = view_as<EntitySingleton>(new StringMap());
        ArrayList entArr = new ArrayList();
        self.SetValue("Entities", entArr);
        return self;
    }
    
    public native Entity Get(int id);

    public Entity Create(char[] EntName) {
        Entity entity;
        if (gamemode.entities.HasKey(EntName))
        {
            EntConfig entcfg = gamemode.entities.get(EntName);
            entity = new Entity(CreateEntityByName("prop_physics_override"));

            char modelName[128], mass[12];
            entcfg.model(modelName, sizeof(modelName));
            entcfg.mass(mass, sizeof(mass));

            DispatchKeyValue(entity.id, "model", modelName);
            DispatchKeyValue(entity.id, "physicsmode", "2");
            DispatchKeyValue(entity.id, "massScale", mass);
            DispatchKeyValue(entity.id, "Solid", "6");
            if (entcfg.spawnflags != 0) {
                char flags[8];
                IntToString(entcfg.spawnflags, flags, sizeof(flags));
                DispatchKeyValue(entity.id, "spawnflags", flags);
            }
        }
        else
        {
            entity = new Entity(CreateEntityByName(EntName));
        }

        entity.active = false;

        ArrayList entArr;
        this.GetValue("Entities", entArr);

        PushArrayCell(entArr, entity);

        return entity;
    }

    public void Clear() {
        ArrayList entArr;
        this.GetValue("Entities", entArr);

        for(int i=0; i < entArr.Length; i++)
            delete view_as<Entity>(entArr.Get(i, 0));

        entArr.Clear();
    }

    public ArrayList FindInBox(Vector vecMin, Vector vecMax) {
        int entId = 0;
        ArrayList entArr = new ArrayList(128);
        
        while ((entId = FindEntityByClassname(entId, "*")) != -1) {
            if (!IsValidEntity(entId)) continue;

            char className[32];
            GetEntityClassname(entId, className, sizeof(className));
            if ((StrContains(className, "prop_") != -1) || (StrContains(className, "weapon_") != -1)) {

                Entity ent = new Entity(entId);

                Vector vecEnt = ent.GetPos();

                if ((vecMin.x < vecEnt.x < vecMax.x) && (vecMin.y < vecEnt.y < vecMax.y) && (vecMin.z < vecEnt.z < vecMax.z)) {
                    PushArrayCell(entArr, ent);
                }
            }
        }

        return entArr;
    }
}