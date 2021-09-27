methodmap ClientSingleton < Base {

    public ClientSingleton() {
        ClientSingleton self = view_as<ClientSingleton>(new Base());
        self.CreateArrayList("Clients", 2);
        
        return self;
    }

    public Client Add(int id) {
        any data[2];
        data[0] = id;
        data[1] = new Client(id);
        this.GetArrayList("Clients").PushArray(data);
    }

    public Client Remove(int id) {
        int idx = this.GetArrayList("Clients").FindValue(id, 0);
        this.GetArrayList("Clients").Erase(idx);
    }

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
        self.CreateArrayList("entities");
        self.CreateArrayList("metadata");
        return self;
    }

    public ArrayList GetAll() { return this.GetArrayList("entities"); }

    public Entity Add(char[] entclass) {
        EntityMeta entdata = gamemode.meta.GetEntity(entclass);
    
        if (entdata != null)
        {
            Entity ent = new Entity();
            ent.meta = entdata;
            ent.spawned = false;
            ent.SetString("class", entclass);
            this.GetArrayList("entities").Push(ent);
            return ent;
        }

        return null;
    }

    public Entity AddNativeEnt(int id) {
        ArrayList entArr;
        this.GetValue("entities", entArr);

        Entity ent = new Entity(id);
        ent.spawned = true;

        char entName[32];
        GetEntityClassname(id, entName, sizeof(entName));

        if (StrContains(entName, "weapon") != -1)
            switch (GetEntProp(ent.id, Prop_Send, "m_iItemDefinitionIndex")) {
                case 60: { entName = "weapon_m4a1_silencer"; }
                case 61: { entName = "weapon_usp_silencer"; }
                case 63: { entName = "weapon_cz75a"; }
                case 64: { entName = "weapon_revolver"; }
                case 23: { entName = "weapon_mp5sd"; }
            }

        ent.SetString("class", entName);

        entArr.Push(ent);

        return ent;
    }
    
    public native Entity Get(int id);
    
    public native Entity TryGetOrAdd(int id);

    public native Entity TryGetOrNew(int id);

    public native Entity Create(char[] EntName, bool register = true);

    public native void Remove(int id);

    public void Clear() {
        ArrayList entArr;
        this.GetValue("entities", entArr);

        for(int i=0; i < entArr.Length; i++)
            delete view_as<Entity>(entArr.Get(i, 0));

        entArr.Clear();
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
                        entArr.Push(this.TryGetOrNew(entId));
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

                    if (!isInInfCone && isInCone)
                        entArr.Push(this.TryGetOrNew(entId));
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