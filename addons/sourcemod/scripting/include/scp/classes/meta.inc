enum InventoryEvents {
    ON_PICKUP = 0,
    ON_DROP,
    ON_USE
}

methodmap OnUseMeta < Base {

    public OnUseMeta() {
        return view_as<OnUseMeta>(new Base());
    }
    
    property Handle hndl {
        public set(Handle val) { this.SetValue("hndl", val); }
        public get() { Handle val; return this.GetValue("hndl", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }
}

methodmap OnPickupMeta < Base {

    public OnPickupMeta() {
        return view_as<OnPickupMeta>(new Base());
    }

    property bool invblock {
        public set(bool val) { this.SetBool("invblock", val); }
        public get() { return this.GetBool("invblock"); }
    }
    
    property Handle hndl {
        public set(Handle val) { this.SetValue("hndl", val); }
        public get() { Handle val; return this.GetValue("hndl", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }
}

methodmap OnDropMeta < Base {

    public OnDropMeta() {
        return view_as<OnDropMeta>(new Base());
    }
    
    property Handle hndl {
        public set(Handle val) { this.SetValue("hndl", val); }
        public get() { Handle val; return this.GetValue("hndl", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }
}

methodmap EntityMeta < Base {

    public EntityMeta() {
        return view_as<EntityMeta>(new Base());
    }
    
    property float mass {
        public set(float val) { this.SetFloat("mass", val); }
        public get() { return this.GetFloat("mass"); }
    }

    property int bodygroup {
        public set(int val) { this.SetInt("bodygroup", val); }
        public get() { return this.GetInt("bodygroup"); }
    }

    property int skin {
        public set(int val) { this.SetInt("skin", val); }
        public get() { return this.GetInt("skin"); }
    }

    property int spawnflags {
        public set(int val) { this.SetInt("spawnflags", val); }
        public get() { return this.GetInt("spawnflags", 4356); }
    }

    property bool usable {
        public set(bool val) { this.SetBool("usable", val); }
        public get() { return this.GetBool("usable"); }
    }

    property int cd {
        public set(int val) { this.SetInt("cooldown", val); }
        public get() { return this.GetInt("cooldown"); }
    }

    property OnUseMeta onuse {
        public set(OnUseMeta val) { this.SetValue("onuse", val); }
        public get() { OnUseMeta val; return this.GetValue("onuse", val) ? val : null; }
    }

    property OnPickupMeta onpickup {
        public set(OnPickupMeta val) { this.SetValue("onpickup", val); }
        public get() { OnPickupMeta val; return this.GetValue("onpickup", val) ? val : null; }
    }
    
    property OnDropMeta ondrop {
        public set(OnDropMeta val) { this.SetValue("ondrop", val); }
        public get() { OnDropMeta val; return this.GetValue("ondrop", val) ? val : null; }
    }

    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }

    public void model(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("model", buffer) : this.GetString("model", buffer, max_size); }
}

methodmap ModelMeta < Base {

    public ModelMeta() {
        return view_as<ModelMeta>(new Base());
    }
    
    property ArrayList bg {
        public get() { return this.GetArrayList("bg"); }
    }

    public void Path(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("path", buffer) : this.GetString("path", buffer, max_size); }

    public int GetBgIdx(char[] name, int idx) {
        int bgidx = 1;
        char bgnames[11][12] = {"base","head","eyes","helmet","mask",/*"flag",*/"rank","body","belt","legs","pl","pr"};

        for (int i=0; i < this.bg.Length; i++)
        {
            if (StrEqual(bgnames[i], name)) break;
            
            int id = this.bg.Get(i);
            
            if (id == 0) continue;

            bgidx += bgidx * id;
            
            if (StrEqual(bgnames[i], name))
            {
                break;
            }
        }

        return bgidx * idx;
    }

    public int GetBgId(char[] name) {
        int id = 0;
        char bgnames[11][12] = {"base","head","eyes","helmet","mask",/*"flag",*/"rank","body","belt","legs","pl","pr"};
        for (int i=0; i < this.bg.Length; i++) {
            if (StrEqual(bgnames[i], name)) break;
            id++;
        }

        return id;
    }
}

methodmap Meta < Base {

    public Meta() {
        Meta meta = view_as<Meta>(new Base());
        meta.CreateList("entities");
        meta.CreateList("models");
        return meta;
    }

    public void RegisterEntity(char[] entclass, EntityMeta entmeta) { this.GetList("entities").Set(entclass, entmeta); }

    public EntityMeta GetEntity(char[] entclass) { return this.GetList("entities").Get(entclass); }

    public void RegEntEvent(InventoryEvents eid, char[] entclass, char[] funcname, bool invblock = false) {
        EntityMeta entdata = this.GetEntity(entclass);
        
        switch (eid) {
            case ON_PICKUP: {
                entdata.onpickup = new OnPickupMeta();

                entdata.onpickup.hndl = GetMyHandle();
                entdata.onpickup.name(funcname);
                entdata.onpickup.invblock = invblock;
            }
            case ON_DROP: {
                entdata.ondrop = new OnDropMeta();

                entdata.ondrop.hndl = GetMyHandle();
                entdata.ondrop.name(funcname);
            }
            case ON_USE: {
                entdata.onuse = new OnUseMeta();

                entdata.onuse.hndl = GetMyHandle();
                entdata.onuse.name(funcname);
            }
        }
    }

    public void RegisterModel(char[] modelid, ModelMeta mdlmeta) { this.GetList("models").Set(modelid, mdlmeta); }

    public ModelMeta GetModel(char[] modelid) { return this.GetList("models").Get(modelid); }
}