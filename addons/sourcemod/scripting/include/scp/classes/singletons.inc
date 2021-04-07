methodmap ClientSingleton < Base {

    public ClientSingleton() {
        ClientSingleton self = view_as<ClientSingleton>(new StringMap());
        Client clients[MAXPLAYERS + 1];
        self.SetArray("Clients", clients, sizeof(clients));
        
        return self;
    }
    
    public native Client Get(int id);

    public Client Add(int id) {
        Client clients[MAXPLAYERS + 1];
        this.GetArray("Clients", clients, sizeof(clients));
        clients[id] = view_as<Client>(new Client(id));
        this.SetArray("Clients", clients, sizeof(clients));
    }

    public Client Remove(int id) {
        Client clients[MAXPLAYERS + 1];
        this.GetArray("Clients", clients, sizeof(clients));
        delete clients[id];
        this.SetArray("Clients", clients, sizeof(clients));
    }

    property int len { 
        public get() { return MAXPLAYERS + 1; }
    }

    public native int InGame();

    public native int Alive();

    public native Client GetRandom();

    public Client GetRandomWithoutClass() {
        Client client = this.GetRandom();
        int crutch = 0; // :c
        while (view_as<int>(client) != 0 && client.haveclass) {
            if (crutch == 500) break;
            client = this.GetRandom();
            crutch++;
        }

        return view_as<Client>(client);
    }
}

methodmap EntitySingleton < Base {

    public EntitySingleton() {
        EntitySingleton self = view_as<EntitySingleton>(new StringMap());
        ArrayList entArr = new ArrayList();
        self.SetValue("Entities", entArr);
        return self;
    }

    public ArrayList GetAll() {
        ArrayList entArr;
        this.GetValue("Entities", entArr);
        return entArr;
    }

    public Entity Add(int id) {
        ArrayList entArr;
        this.GetValue("Entities", entArr);

        Entity ent = new Entity(id);
        ent.active = true;

        char entName[32];
        GetEntityClassname(id, entName, sizeof(entName));

        if (StrContains(entName, "weapon") != -1)
            switch (GetEntProp(ent.id, Prop_Send, "m_iItemDefinitionIndex")) {
                case 60: {
                    entName = "weapon_m4a1_silencer";
                }
                case 61: {
                    entName = "weapon_usp_silencer";
                }
                case 63: {
                    entName = "weapon_cz75a";
                }
                case 64: {
                    entName = "weapon_revolver";
                }
                case 23: {
                    entName = "weapon_mp5sd";
                }
            }

        ent.SetString("class", entName);

        entArr.Push(ent);

        return ent;
    }
    
    public native Entity Get(int id);
    public native Entity TryGetOrAdd(int id);

    public native Entity Create(char[] EntName, bool register = true);

    public native void Remove(int id);

    public void Clear() {
        ArrayList entArr;
        this.GetValue("Entities", entArr);

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
                        PushArrayCell(entArr, this.TryGetOrAdd(entId));
                    }

                    delete vecEnt;
                }
            }
        }

        delete vecMin;
        delete vecMax;

        return entArr;
    }
}