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

methodmap EntitySingleton < Base {

    public EntitySingleton() {
        EntitySingleton self = view_as<EntitySingleton>(new Base());
        self.CreateArrayList("entities", 2);
        return self;
    }

    property ArrayList list {
        public native get();
    }

    public ArrayList GetAll()
    {
        ArrayList entities = new ArrayList();
        for (int i=0; i < this.list.Length; i++)
        {
            Entity ent = this.list.Get(i, 1);
            if (ent) entities.Push(ent);
        }
        return entities;
    }
    
    public Entity Get(int id)
    {
        ArrayList entities = this.list;
        int idx = entities.FindValue(id, 0);
        return (idx != -1) ? entities.Get(idx, 1) : view_as<Entity>(null);
    }
    
    public bool TryGet(int id, Entity &ent)
    {
        ArrayList entities = this.list;
        int idx = entities.FindValue(id, 0);
        if (idx == -1) return false;
        ent = entities.Get(idx, 1);
        return true;
    }

    public native Entity Create(char[] EntName, bool register = true);

    public native Entity Push(Entity ent);

    public native void Remove(Entity ent);

    public native void RemoveByID(int id);

    public native void Dissolve(Entity ent);

    public native bool IndexUpdate(Entity ent);

    public native void Clear();

    public void SpawnedClear() {
        ArrayList list = this.list;

        for (int i=0; i < list.Length; i++)
        {
            Entity ent = list.Get(i, 1);

            if (ent && ent.id > MaxClients && ent.id != 5000 && ent.IsValid() && (ent.HasProp("m_hOwnerEntity") && (GetEntPropEnt(ent.id, Prop_Send, "m_hOwnerEntity") == -1)))
            {
                if (ent.IsClass("point_worldtext")) continue;
                
                list.Erase(i);
                ent.Remove();
                i--;
            }
        }
    }

    public void FullClear() {
        ArrayList list = this.list;

        for (int i=0; i < list.Length; i++)
        {
            Entity ent = list.Get(i, 1);

            if (ent) ent.Dispose();
        }

        list.Clear();
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

    public ArrayList FindInPVS(Entity ent, int distance = 100, int fov = 90, char filter[1][32] = {"player"}) {
        ArrayList entarr;
        float firstentpos[3], secondentpos[3];
        Handle tr;
        bool isplayer = ent.IsClass("player");

        if (isplayer)
            entarr = this.FindInCone(view_as<Player>(ent).EyePos(), ent.GetAng().GetForwardVectorScaled(view_as<Player>(ent).EyePos(), float(distance)), fov, filter, sizeof(filter));
        else
            entarr = this.FindInCone(ent.GetPos(), ent.GetAng().GetForwardVectorScaled(ent.GetPos(), float(distance)), fov, filter, sizeof(filter));

        if (isplayer) view_as<Player>(ent).EyePos().GetArrD(firstentpos); else ent.GetPos().GetArrD(firstentpos);

        for (int i=0; i < entarr.Length; i++)
        {
            Entity entity = entarr.Get(i);

            if (entity.IsClass("player")) view_as<Player>(entity).EyePos().GetArrD(secondentpos); else entity.GetPos().GetArrD(secondentpos);
            
            ArrayList trargs = new ArrayList();
            trargs.Push(ent.id);
            trargs.Push(entity.id);

            tr = TR_TraceRayFilterEx(firstentpos, secondentpos, MASK_SOLID_BRUSHONLY, RayType_EndPoint, FindInPVSFilter, trargs);
            
            if (TR_DidHit(tr))
            {
                entarr.Erase(i);
                i--;
            }

            delete trargs;
            delete tr;
        }

        return entarr;
    }

    public void Dispose() {
        this.FullClear();
        delete this.GetArrayList("entities");
        this.RemoveValue("entities");
    }
}

stock bool FindInPVSFilter(int entid, int mask, ArrayList trargs)
{
    if (entid == trargs.Get(0) || entid == trargs.Get(1)) return false;
    return true;
}

methodmap PlayerSingleton < Base {

    public PlayerSingleton() {
        return view_as<PlayerSingleton>(new Base());
    }
    
    property ArrayList list {
        public native get();
    }

    public native Player Add(int id);

    public native void Remove(int id);

    public ArrayList GetAll()
    {
        ArrayList entities = this.list;

        ArrayList players = new ArrayList();
        for (int i=0; i < entities.Length; i++)
        {
            int id = entities.Get(i, 0);
            if (id <= MaxClients && id > 0 && IsClientInGame(id))
            {
                Player ply = entities.Get(i, 1);
                if (ply) players.Push(ply);
            }
        }
        return players;
    }

    public Player GetByID(int id)
    {
        ArrayList entities = this.list;
        int idx = entities.FindValue(id, 0);
        if (idx == -1) return view_as<Player>(null);
        return entities.Get(idx, 1);
    }

    public Player GetBySteamID(char[] authcheck, AuthIdType type = AuthId_Steam2)
    {
        ArrayList players = this.GetAll();
        
        for (int i=0; i < players.Length; i++)
        {
            char authstr[64];
            Player ply = players.Get(i);
            ply.GetAuth(authstr, sizeof(authstr), type);

            if (StrEqual(authstr, authcheck))
                return ply;
        }

        return view_as<Player>(null);
    }

    public Player GetRandom()
    {
        ArrayList sortedPlayers = new ArrayList();
        ArrayList players = this.GetAll();
        for (int i=0; i < players.Length; i++) {
            sortedPlayers.Push(players.Get(i));
        }

        delete players;

        return sortedPlayers.Get(GetRandomInt(0, sortedPlayers.Length - 1));
    }

    public int InGame()
    {
        ArrayList players = this.GetAll();
        int len = players.Length;
        delete players;
        return len;
    }

    public int InSpec()
    {
        ArrayList players = this.GetAll();
        for (int i=0; i < players.Length; i++)
            if (view_as<Player>(players.Get(i)).ready)
            {
                players.Erase(i);
                i--;
            }
        int len = players.Length;
        delete players;
        return len;
    }

    public int Alive()
    {
        ArrayList alivePlayers = new ArrayList();
        ArrayList players = this.GetAll();
        for (int i=0; i < players.Length; i++) {
            Player ply = players.Get(i);
            if (ply.IsAlive())
                alivePlayers.Push(ply);
        }

        int len = alivePlayers.Length;

        delete players;
        delete alivePlayers;

        return len;
    }

    property int Length {
        public get() { return this.InGame(); }
    }
}

methodmap WorldTextSingleton < Base {
    
    public WorldTextSingleton() {
        return view_as<WorldTextSingleton>(new Base());
    }

    property ArrayList list {
        public native get();
    }

    public native WorldText Create(Vector vec, Angle ang, TypeWorldText type = Text);

    public native void Remove(WorldText ent);

    public WorldText Get(int id) {
        return this.list.Get(id);
    }

    public ArrayList GetAll(TypeWorldText type = Text) {
        ArrayList list = this.list;
        ArrayList sortedlist = new ArrayList();

        for (int i=0; i < list.Length; i++) {
            char class[32];
            view_as<Entity>(list.Get(i, 1)).GetClass(class, sizeof(class));
            if (!StrEqual(class, "point_worldtext")) continue;

            WorldText wt = list.Get(i, 1);

            if (wt && (wt.type == type || type == All))
                sortedlist.Push(wt);
        }

        return sortedlist;
    }
}