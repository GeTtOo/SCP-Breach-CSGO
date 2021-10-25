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

methodmap ClientSingleton < Base {

    public ClientSingleton() {
        return view_as<ClientSingleton>(new Base());
    }

    public native void Add(int id);

    public native void Remove(int id);

    public native ArrayList GetAll();

    public native Client Get(int id);

    public native Client GetRandom();

    public native int InGame();

    public native int Alive();

    property int Length {
        public get() { return this.InGame(); }
    }
}

methodmap EntitySingleton < Base {

    public EntitySingleton() {
        EntitySingleton self = view_as<EntitySingleton>(new Base());
        self.CreateArrayList("entities", 2);
        return self;
    }

    property ArrayList list {
        public get() { return this.GetArrayList("entities"); }
    }

    public native ArrayList GetAll();
    
    public native Entity Get(int id);
    
    public native Entity TryGet(int id, Entity &ent);

    public native Entity Create(char[] EntName, bool register = true);

    public native void Remove(int id);

    public native bool IndexUpdate(Entity ent);

    public native void Clear();

    public void SpawnedClear() {
        ArrayList list = this.list;

        for (int i=0; i < list.Length; i++)
        {
            Entity ent = list.Get(i, 1);

            if (ent != null && ent.id > MaxClients && ent.id != 5000)
            {
                ent.Remove();
                list.Erase(i);
                i--;
            }
        }
    }

    public ArrayList FindInBox(Vector vecMin, Vector vecMax, char[][] filter, int filter_size) {
        int entId = 0;
        ArrayList entArr = new ArrayList(128);
        
        while ((entId = FindEntityByClassname(entId, "*")) != -1) {
            if (!IsValidEntity(entId)) continue;

            char className[32];
            GetEntityClassname(entId, className, sizeof(className));

            for (int i=0; i < filter_size; i++) {
                if (StrContains(className, filter[i]) == 0) {
                    float vec[3];
                    GetEntPropVector(entId, Prop_Send, "m_vecOrigin", vec);
                    Vector vecEnt = new Vector(vec[0], vec[1], vec[2]);

                    if ((vecMin.x < vecEnt.x < vecMax.x) && (vecMin.y < vecEnt.y < vecMax.y) && (vecMin.z < vecEnt.z < vecMax.z)) {
                        Entity ent;
                        if (this.TryGet(entId, ent))
                            entArr.Push(ent);
                        else
                            entArr.Push(new Entity(entId));
                    }

                    delete vecEnt;
                }
            }
        }

        delete vecMin;
        delete vecMax;

        return entArr;
    }

    public ArrayList FindInCone(Vector coneApex, Vector coneEnd, int coneAperture, char[][] filter, int filter_size) {
        int entId = 0;
        ArrayList entArr = new ArrayList(128);
        
        while ((entId = FindEntityByClassname(entId, "*")) != -1) {
            if (!IsValidEntity(entId)) continue;

            char className[32];
            GetEntityClassname(entId, className, sizeof(className));

            for (int i=0; i < filter_size; i++) {
                if (StrContains(className, filter[i]) == 0) {
                    float vec[3];
                    GetEntPropVector(entId, Prop_Send, "m_vecOrigin", vec);
                    Vector vecEnt = new Vector(vec[0], vec[1], vec[2]);

                    int halfAperture = coneAperture / 2;
                    Vector atp = coneApex.Clone() - vecEnt;
                    Vector axis = coneApex.Clone() - coneEnd.Clone();

                    bool isInInfCone = atp.Clone().DotProduct(axis.Clone()) / atp.Clone().Length() / axis.Clone().Length() < Cosine(float(halfAperture));
                    bool isInCone = atp.DotProduct(axis.Clone()) / axis.Clone().Length() < axis.Length();

                    if (!isInInfCone && isInCone) {
                        Entity ent;
                        if (this.TryGet(entId, ent))
                            entArr.Push(ent);
                        else
                            entArr.Push(new Entity(entId));
                    }
                }
            }
        }

        delete coneApex;
        delete coneEnd;

        return entArr;
    }

    public ArrayList FindInPVS(Client ply, int distance = 100, int fov = 90, char filter[1][32] = {"player"}) {
        return this.FindInCone(ply.EyePos(), ply.GetAng().Forward(ply.EyePos(), float(distance)), fov, filter, sizeof(filter));
    }
}