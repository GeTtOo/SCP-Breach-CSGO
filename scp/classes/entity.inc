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

        return entity;
    }

    property int id {
        public get() { int id; return this.GetValue("id", id) ? id : -1; }
    }

    public void GetClass(char[] name, int max_size) {
        int id;
        this.GetValue("id", id);
        GetEntityClassname(id, name, max_size);
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
}

methodmap Ents < Handle {

    public static ArrayList FindInBox(Vector vecMin, Vector vecMax) {
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
                    PrintToServer("%s Pos: %f, %f, %f", className, vecEnt.x, vecEnt.y, vecEnt.z);
                    PushArrayCell(entArr, ent);
                }
            }
        }

        return entArr;
    }
}