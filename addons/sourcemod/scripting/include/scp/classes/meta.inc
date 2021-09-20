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
        public get() { return this.GetInt("spawnflags"); }
    }

    property bool usable {
        public set(bool val) { this.SetBool("usable", val); }
        public get() { this.GetBool("usable"); }
    }

    property OnUseMeta onuse {
        public set(OnUseMeta val) { this.SetValue("onuse", val); }
        public get() { OnUseMeta val; return this.GetValue("onuse", val) ? val : null; }
    }

    property OnPickupMeta onpickup {
        public set(OnPickupMeta val) { this.SetValue("onpickup", val); }
        public get() { OnPickupMeta val; return this.GetValue("onpickup", val) ? val : null; }
    }

    public void class(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("class", buffer) : this.GetString("class", buffer, max_size); }
    
    public void name(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("name", buffer) : this.GetString("name", buffer, max_size); }

    public void model(char[] buffer, int max_size = 0) { (max_size == 0) ? this.SetString("model", buffer) : this.GetString("model", buffer, max_size); }
}

methodmap Meta < Base {

    public Meta() {
        Meta meta = view_as<Meta>(new Base());
        meta.CreateArrayList("entities");
        return meta;
    }

    public void RegisterEntity(EntityMeta entmeta) {
        this.GetArrayList("entities").Push(entmeta);
    }

    public EntityMeta GetEntity(char[] entclass) {
        ArrayList entsmeta = this.GetArrayList("entities");

        for (int i=0; i < entsmeta.Length; i++)
        {
            EntityMeta entdata = entsmeta.Get(i);

            char entmetaclass[32];
            entdata.class(entmetaclass, sizeof(entmetaclass));

            if (StrEqual(entclass, entmetaclass))
            {
                return entdata;
            }
        }

        return view_as<EntityMeta>(null);
    }

    public void RegEntOnUse(char[] entclass, char[] funcname) {
        EntityMeta entdata = this.GetEntity(entclass);
        entdata.onuse = new OnUseMeta();

        entdata.onuse.hndl = GetMyHandle();
        entdata.onuse.name(funcname);
    }

    public void RegEntOnPickup(char[] entclass, char[] funcname, bool invblock = false) {
        EntityMeta entdata = this.GetEntity(entclass);
        entdata.onpickup = new OnPickupMeta();

        entdata.onpickup.hndl = GetMyHandle();
        entdata.onpickup.name(funcname);
        entdata.onpickup.invblock = invblock;
    }
}